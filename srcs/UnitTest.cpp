#include "UnitTest.hpp"

UnitTest::UnitTest(std::string directory, int timeout, bool displayLineBreak)
    : directory_(directory), name_(directory), CC_(UnitTestconfig::CC), CFLAGS_(UnitTestconfig::CFLAGS),
		timeout_(timeout), displayLineBreak_(displayLineBreak) {}

void UnitTest::configure(std::string directory, int timeout, bool displayLineBreak) {
	directory_ = directory;
	name_ = directory;
	timeout_ = timeout;
	displayLineBreak_ = displayLineBreak;
}

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
void UnitTest::addTemporaryMainFile(const std::string &templates, std::string contents, std::string filler) {
    std::string code = 
		"\n#define TIMEOUT " + std::to_string(timeout_) + "\n"
		"\n"
        "static void timeout_handler(int signum) {\n"
        "	fprintf(stderr, \"Timeout (t > %i.00s)\", TIMEOUT);\n"
        "	exit(1);\n"
        "	(void)signum;\n"
        "}\n"
        "\n\nint main(int argc, char **argv) {\n"
        "	signal(SIGALRM, timeout_handler);\n"
        "	alarm(TIMEOUT);\n"
        "	" + contents + ";\n"
        "	alarm(0)\n;"
		"	fflush(stdout);\n"
        "	(void)argc; (void)argv;\n"
        "}";
    
    addTemporaryCodeFile(templates + code);
	(void)filler;
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

void UnitTest::addTestCaseSameInOut(const std::string& inOutStr) {
	addTestCase(inOutStr, inOutStr);
}

void UnitTest::setExecutableName(const std::string& name) {
	executableFile_.setFilename(name);
}

void UnitTest::setDirectory(const std::string& path) {
	directory_ = path;
}

void UnitTest::setName(const std::string& name) {
	name_ = name;
}

void UnitTest::printStatus() const {
	if (AllTestCaseOk())
		utils::printOK(name_);
	else {
		utils::printKO(name_);
	}
}

bool UnitTest::run() {
	try	{
		validateRequiredFiles();
		validateNorme();
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
	std::cout << color::redText(name_ + ": " + e.type()) << std::endl;
	if ((UnitTestconfig::showKO || UnitTestconfig::showDetails) && *e.what())
		std::cout << ": " << e.what() << std::endl;
}

// 0 = OK, 1 = KO
bool UnitTest::norminette(const std::string &path) {
	std::string norm = "python3 -m norminette";
	std::string flags;
	std::string extension = utils::getFileExtension(path);
	if (extension != "h" && extension != "c")
		return 0;
	if (extension == "h")
		flags += "-R CheckDefine";
	std::string command = "(" + norm + " " + flags + " " + path +
		" | grep Error:) > " + errorFile_.filename();
	std::system(command.c_str());
	return !errorFile_.readContent().empty();
}

void UnitTest::validateNorme() {
	std::string message;
	for (auto &path: requiredFilePaths_) {
		if (norminette(path))
			message += path + "\n" + errorFile_.readContent();
	}
	if (!message.empty())
		throw NorminetteError(message);
}

void UnitTest::validateRequiredFiles() {
	if (!utils::pathExists(directory_))
		throw NothingTurnedIn();

	std::string message;
	for (auto &path: requiredFilePaths_) {
		if (!utils::pathExists(path))
			message += path + "\n";
	}
	if (!message.empty())
		throw FileNotFoundError(message);
}

void UnitTest::compile() {
	std::vector<std::string> all_file_path = requiredFilePaths_;
	// merge
	for (const TemporaryFile &file : allTempCodeFiles_) {
		all_file_path.push_back(file.filename());
	}

	std::string SRCS;
	std::string IFLAGS;
	std::set<std::string> headers;

	// processing
	for (const std::string &filePath : all_file_path) {
		std::string extension = utils::getFileExtension(filePath);
		if (!utils::pathExists(filePath))
			throw FileNotFoundError(filePath);
		if (extension == "c")
			SRCS += filePath + " ";
		if (extension == "h")
			headers.insert(utils::getParentDirectory(filePath));
	}

	for (const std::string &dirPath : headers) {
		IFLAGS += "-I" + dirPath + " ";
	}
	std::string redirect = " -o " + executableFile_.filename() + " 2> " + errorFile_.filename();
	std::string compileCommand = CC_ + " " + CFLAGS_ + " " + SRCS + " " + IFLAGS + " " + redirect;
	int compileResult = std::system(compileCommand.c_str());
	if (compileResult != 0)
	{
		throw CompilationError(compileCommand + "\n" + errorFile_.readContent());
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
	//  	throw TestCaseKO(getTestCaseInfo());
	return AllTestCaseOk();
}

bool UnitTest::runTestCase(t_test_case &test_case) {
	std::string redirect = " > " + actualOutputFile_.filename() + " 2> " + errorFile_.filename();
	std::string runCommand = "{ ./" + executableFile_.filename() + " " + test_case.argv + " ;}" + redirect;
	std::system(runCommand.c_str());

	test_case.actualOutput = actualOutputFile_.readContent();
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
		ret << "Failed: [\n";
	else
		ret << "All Cases: [\n";
	for (size_t i = 0; i < allTestCase_.size(); i++)
	{
		auto &test_case = allTestCase_[i];
		// skip ok if only show KO
		if (test_case.ok && UnitTestconfig::showKO)
			continue;
		ret << "(Case " << i + 1 << ": Input: '"
			<< getFormatDisplay(utils::reduceStringTo(test_case.argv, UnitTestconfig::lineLength))
			<< "'; Output: '" << getFormatDisplay(utils::reduceStringTo(test_case.actualOutput, UnitTestconfig::lineLength)) << "'),\n";
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
		ret << "Input    : " << getFormatDisplay(testCase.argv)
			<< "\nOutput   : " << getFormatDisplay(testCase.actualOutput, testCase.expectedOutput) << "\n";
		utils::fillLine(ret, '=', UnitTestconfig::lineLength);
		if (!testCase.error) {
			ret << "Expected : " << getFormatDisplay(testCase.expectedOutput, testCase.actualOutput);
		}
		else
			ret << "Error    : " << utils::reduceStringTo(testCase.stdError, UnitTestconfig::lineLength * 10);
		if (!UnitTestconfig::showKO)
			ret << "\nStatus   : " << ((testCase.ok)? "OK": "KO");
		ret << "\n\n\n";

		if (!UnitTestconfig::showAll)
			break ;
	}
	return ret.str();
}

std::string UnitTest::getFormatDisplay(std::string str) const {
	return getFormatDisplay(str, str);
}

std::string UnitTest::getFormatDisplay(std::string str, std::string cmp) const {
	std::ostringstream oss;

	int is_colored = 0;
	for (size_t i = 0; i < str.size(); i++) {
		if ((i >= cmp.size() || str[i] != cmp[i])) {
			if (!is_colored) {
				oss << color::BG_YELLOW;
				is_colored = 1;
			}
		} else if (is_colored) {
			oss << color::RESET;
			is_colored = 0;
		}
		if (str[i] < 32 || str[i] > 126) {
			if (!is_colored)
				oss << color::CYAN;
			else
				oss << color::BG_YELLOW_CLR_BLUE;
			if (str[i] == '\n' && displayLineBreak_)
				oss << color::RESET <<"$\n";
			else
            	oss << std::hex << "\\" << std::setw(2) << std::setfill('0') << (int)(unsigned char)(str[i]) << std::dec << color::RESET;
			is_colored = 0;
        } else {
            oss << str[i];
        }
	}
	if (displayLineBreak_)
		oss << '$';
	oss << color::RESET;
	return oss.str();
}

UnitTestGenExpected::UnitTestGenExpected(std::string directory, int timeout, bool displayLineBreak)
	: UnitTest(directory, timeout, displayLineBreak) {}

void UnitTestGenExpected::addTemporaryMainFile(const std::string &templates, const std::string printExpected, const std::string printOutput) {
    std::string code = 
		"\n#define TIMEOUT " + std::to_string(timeout_) + "\n"
		"\n"
        "static void timeout_handler(int signum) {\n"
        "	fprintf(stderr, \"Timeout (t > %i.00s)\", TIMEOUT);\n"
        "	exit(1);\n"
        "	(void)signum;\n"
        "}\n"
        "\n\nint main(int argc, char **argv) {\n"
        "	signal(SIGALRM, timeout_handler);\n"
        "	alarm(TIMEOUT);\n"
        "	" + printOutput + ";\n"
        "	alarm(0)\n;"
		"	int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC);\n"
		"	if (fd == -1) { printf(\" Error opening argv[1] (for expected output)\"); return 1; }\n"
		"	fflush(stdout);\n"
		"	dup2(fd, 1)\n;"
		"	close(fd);"
		"	" + printExpected + ";\n"
		"	fflush(stdout);\n"
        "	(void)argc; (void)argv;\n"
        "}";
    addTemporaryCodeFile(templates + code);
}

void UnitTestGenExpected::addTestCase(const std::string &argv) {
	t_test_case test_case;
	test_case.argv = argv;
	allTestCase_.push_back(test_case);
}

bool UnitTestGenExpected::runTestCase(t_test_case &test_case) {
	std::string redirect = " > " + actualOutputFile_.filename() + " 2> " + errorFile_.filename();
	std::string runCommand = "{ ./" + executableFile_.filename() + " " + expectedOutputFile_.filename()
								+ " " + test_case.argv + " ;}" + redirect;
	std::system(runCommand.c_str());

	test_case.actualOutput = actualOutputFile_.readContent();
	test_case.stdError = errorFile_.readContent();
	test_case.expectedOutput = expectedOutputFile_.readContent();

	test_case.error = !test_case.stdError.empty();
	test_case.ok = (test_case.actualOutput == test_case.expectedOutput) && (!test_case.error);
	return test_case.ok;
}
