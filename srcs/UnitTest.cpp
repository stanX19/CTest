#include "UnitTest.hpp"

UnitTest::UnitTest(std::string directory, int timeout)
    : directory_(directory), CC_(UnitTestconfig::CC), CFLAGS_(UnitTestconfig::CFLAGS), timeout_(timeout) {}

// .c type file will be compiled together
void UnitTest::addRequiredFile(const std::string &filename) {
	requiredFilePaths_.push_back(directory_ + "/" + filename);
}

// won't be compiled together
void UnitTest::addTemporaryFile(const std::string &content) {
	TemporaryFile tempFile(content);
	allTemporaryFiles_.push_back(tempFile);
}

// will be compiled together
void UnitTest::addTemporaryMainFile(const std::string &templates, const std::string &contents) {
    std::string code = 
		"\n#define TIMEOUT " + std::to_string(timeout_) + "\n"
		"\n"
        "void timeout_handler(int signum) {\n"
        "    fprintf(stderr, \"Timeout (t > %i.00s)\", TIMEOUT);\n"
        "    exit(1);\n"
        "    (void)signum;\n"
        "}\n"
        "\n\nint main(int argc, char **argv) {\n"
        "    signal(SIGALRM, timeout_handler);\n"
        "    alarm(TIMEOUT);\n"
        "    " + contents + ";\n"
        "    alarm(0)\n;"
        "    (void)argc; (void)argv;\n"
        "}";
    
    addTemporaryCodeFile(templates + code);
}

// will be compiled together
void UnitTest::addTemporaryCodeFile(const std::string &content) {
	allTempCodeFiles_.push_back({UnitTestconfig::headers + content, ".c"});
}

void UnitTest::addTestCase(const std::string &argv, const std::string &expectedOutput) {
	t_test_case test_case;
	test_case.expectedOutput = expectedOutput;
	test_case.argv = argv;
	allTestCase_.push_back(test_case);
}

void UnitTest::addSameInOutCase(const std::string& inOutStr) {
	addTestCase(inOutStr, inOutStr);
}

void UnitTest::printStatus() const {
	if (AllTestCaseOk())
		utils::printOK(directory_);
	else {
		utils::printKO(directory_);
	}
}

bool UnitTest::run() {
	try	{
		validateRequiredFiles();
		compile();
		runAllTestCase();
		printStatus();
		printTestCaseInfo();
	}
	catch (const UnitTestException &exc)
	{
		handleException(exc);
	}
	return AllTestCaseOk();
}

void UnitTest::handleException(const UnitTestException &e) {
	std::cout << color::redText(directory_ + ": " + e.type()) << std::endl;
	if ((UnitTestconfig::showKO || UnitTestconfig::showDetails) && *e.what())
		std::cout << ": " << e.what() << std::endl;
}

void UnitTest::validateRequiredFiles() {
	if (!utils::pathExists(directory_))
		throw NothingTurnedIn();

	std::string message;
	for (auto &path: requiredFilePaths_) {
		if (!utils::pathExists(path))
			message += path + " ";
	}
	if (!message.empty())
		throw FileNotFoundError(message);
}

void UnitTest::compile()
{
	std::string compileCommand = CC_ + " " + CFLAGS_;
	for (const std::string &filePath : requiredFilePaths_)
	{
		if (!utils::pathExists(filePath))
			throw FileNotFoundError(filePath);
		if (utils::getFileExtension(filePath) != "c")
			continue;
		compileCommand += " " + filePath;
	}
	for (const TemporaryFile &file : allTempCodeFiles_)
	{
		compileCommand += " " + file.filename();
	}
	compileCommand += " -o " + executableFile_.filename() + " 2> " + errorFile_.filename();
	int compileResult = std::system(compileCommand.c_str());

	if (compileResult != 0)
	{
		throw CompilationError(errorFile_.readContent());
	}
}

bool UnitTest::runAllTestCase()
{
	for (t_test_case &test_case : allTestCase_)
	{
		runTestCase(test_case);
		if (!test_case.ok && !(UnitTestconfig::showAll || UnitTestconfig::showListCase))
			break ;
	}
	// if (!AllTestCaseOk())
	// 	throw TestCaseKO(getKOMessage());
	return AllTestCaseOk();
}

bool UnitTest::runTestCase(t_test_case &test_case)
{
	std::string redirect = " > " + outputFile_.filename() + " 2> " + errorFile_.filename();
	std::string runCommand = "{ ./" + executableFile_.filename() + " " + test_case.argv + " ;}" + redirect;
	std::system(runCommand.c_str());

	test_case.actualOutput = outputFile_.readContent();
	test_case.stdError = errorFile_.readContent();

	test_case.error = !test_case.stdError.empty();
	test_case.ok = (test_case.actualOutput == test_case.expectedOutput) && (!test_case.error);
	return test_case.ok;
}


bool UnitTest::AllTestCaseOk() const {
	return std::all_of(allTestCase_.begin(), allTestCase_.end(),
		[](const t_test_case &test_case){ return test_case.ok; }
	);
}


void UnitTest::printAllTestCase() const {
	for (size_t i = 0; i < allTestCase_.size(); i++)
	{
		if (allTestCase_[i].ok)
			std::cout << color::CYAN << i + 1 << ".OK ";
		else
			std::cout << color::YELLOW << i + 1 << ".KO ";
	}
	std::cout << color::RESET <<std::endl;
}

void UnitTest::printTestCaseInfo() const {
	if (UnitTestconfig::showListCase)
		printAllTestCase();
	std::cout << getTestCaseInfo();
}

std::string UnitTest::getTestCaseInfo() const {
	if (AllTestCaseOk() && UnitTestconfig::showKO)
		return "";
	if (!UnitTestconfig::showAll && !UnitTestconfig::showKO && !UnitTestconfig::showDetails)
		return "";
	if (UnitTestconfig::showDetails)
		return getTestCaseDetailed();
	else
		return getTestCaseOneLine();
}

std::string UnitTest::getTestCaseOneLine() const
{
	std::stringstream ret;

	if (UnitTestconfig::showKO)
		ret << "Failed: [";
	else
		ret << "All Cases: [\n";
	for (size_t i = 0; i < allTestCase_.size(); i++)
	{
		auto &test_case = allTestCase_[i];
		// skip ok if only show KO
		if (test_case.ok && UnitTestconfig::showKO)
			continue;
		ret << "(Case " << i + 1 << ": Input: '"
			<< utils::reduceStringTo(test_case.argv, UnitTestconfig::lineLength)
			<< "'; Output: '" << utils::reduceStringTo(test_case.actualOutput, UnitTestconfig::lineLength) << "'),\n";
		if (!UnitTestconfig::showAll)
			break ;
	}
	ret << "]\n";
	return ret.str();
}

std::string UnitTest::getTestCaseDetailed() const {
	std::stringstream ret;

	for (size_t i = 0; i < allTestCase_.size(); i++)
	{
		auto &testCase = allTestCase_[i];
		// skip ok if only show KO
		if (testCase.ok && UnitTestconfig::showKO)
			continue;
		ret << "==Case " << i + 1;
		utils::fillLine(ret, '=', UnitTestconfig::lineLength);
		ret << "Input    : " << testCase.argv
			<< "\nOutput   : " << testCase.actualOutput << "\n";
		utils::fillLine(ret, '=', UnitTestconfig::lineLength);
		if (!testCase.error) {
			ret << "Expected : " << testCase.expectedOutput;
		}
		else
			ret << "Error    : " << testCase.stdError;
		if (!UnitTestconfig::showKO)
			ret << "\nStatus   : " << ((testCase.ok)? "OK": "KO");
		ret << "\n\n";

		if (!UnitTestconfig::showAll)
			break ;
	}
	return ret.str();
}