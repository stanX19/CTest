#include "UnitTest.hpp"

UnitTest::UnitTest(std::string directory, std::string CC, std::string CFLAGS)
    : directory_(directory), CC_(CC), CFLAGS_(CFLAGS) {}

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
	static const char *MAIN_START = "\nint main(int argc, char **argv){";
	static const char *MAIN_END = ";(void)argc;(void)argv;}";
	addTemporaryCodeFile(templates + MAIN_START + contents + MAIN_END);
}

// will be compiled together
void UnitTest::addTemporaryCodeFile(const std::string &content) {
	static const std::string headers = "\n#include <stdio.h>\n#include <stdlib.h>\n#include <unistd.h>\n#include <string.h>\n#include <math.h>\n";
	allTempCodeFiles_.push_back({headers + content, ".c"});
}

void UnitTest::addTestCase(const std::string &argv, const std::string &expectedOutput) {
	t_test_case test_case;
	test_case.expectedOutput = expectedOutput;
	test_case.argv = argv;
	allTestCase_.push_back(test_case);
}

bool UnitTest::OverallOk() const {
	return std::all_of(allTestCase_.begin(), allTestCase_.end(),
		[](const t_test_case &test_case){ return test_case.ok; }
	);
}

void UnitTest::printStatus() const {
	if (OverallOk())
		utils::printOK(directory_);
	else
		utils::printKO(directory_);
}

bool UnitTest::run() {
	try	{
		validateRequiredFiles();
		compile();
		runAllTestCase();
		printStatus();
		printTestCase();
	}
	catch (const UnitTestException &exc)
	{
		handleException(exc);
	}
	return OverallOk();
}

void UnitTest::handleException(const UnitTestException &e) {
	std::cout << color::redText(directory_ + ": " + e.type()) << std::endl;
	if (UnitTestconfig::showKO || UnitTestconfig::showKODetails)
		std::cout << e.what() << std::endl;
}

void UnitTest::validateRequiredFiles() {
	std::string message;

	if (!utils::dirPathExists(directory_))
		throw NothingTurnedIn();
	for (auto &path: requiredFilePaths_) {
		if (!utils::filePathExists(path))
			message += path + " ";
	}
	if (!message.empty())
		throw RequiredFileNotFound(message);
}

void UnitTest::compile()
{
	std::string compileCommand = CC_ + " " + CFLAGS_;

	for (const std::string &filePath : requiredFilePaths_)
	{
		if (!utils::filePathExists(filePath))
			throw std::runtime_error("required file not found: " + filePath);
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
	}
	if (!OverallOk())
		throw TestCaseKO(getKOMessage());
	return 1;
}

bool UnitTest::runTestCase(t_test_case &test_case)
{
	std::string redirect = " > " + outputFile_.filename() + " 2> " + errorFile_.filename();
	std::string runCommand = "./" + executableFile_.filename() + " " + test_case.argv + redirect;
	std::system(runCommand.c_str());

	test_case.actualOutput = outputFile_.readContent();
	test_case.stdError = errorFile_.readContent();

	test_case.error = !test_case.stdError.empty();
	test_case.ok = (test_case.actualOutput == test_case.expectedOutput) && (!test_case.error);
	return test_case.ok;
}

void UnitTest::printTestCase() {
	if (UnitTestconfig::showAll)
		printAllTestCase();
}

void UnitTest::printAllTestCase() {
	for (size_t i = 0; i < allTestCase_.size(); i++)
	{
		if (allTestCase_[i].ok)
			std::cout << color::CYAN << i + 1 << ".OK ";
		else
			std::cout << color::RED << i + 1 << ".KO ";
	}
	std::cout << color::RESET <<std::endl;
}

std::string UnitTest::getKOMessage() const {
	if (!UnitTestconfig::showKO)
		return "";
	if (UnitTestconfig::showKODetails)
		return getKOTestCaseDetailed();
	else
		return getKOTestCaseSimplified();
}

std::string UnitTest::getKOTestCaseSimplified() const
{
	std::stringstream ret;

	if (OverallOk())
		return "";
	ret << "Failed: [";
	for (size_t i = 0; i < allTestCase_.size(); i++)
	{
		auto &test_case = allTestCase_[i];
		if (test_case.ok)
			continue;
		ret << "(Case " << i + 1 << ": Input: '" << test_case.argv << "'; Output: '"
				  << test_case.actualOutput << "'), ";
	}
	ret << "\b\b]\n";
	return ret.str();
}

std::string UnitTest::getKOTestCaseDetailed() const {
	std::stringstream ret;

	for (size_t i = 0; i < allTestCase_.size(); i++)
	{
		auto &testCase = allTestCase_[i];
		if (testCase.ok)
			continue;
		ret << "==Case " << i + 1 << std::setw(100 - 20) << std::setfill('=') << "\n"
				  << "Input    : " << testCase.argv
				  << "\nOutput   : " << testCase.actualOutput << "\n"
				  << std::setw(100) << std::setfill('=');
		if (!testCase.error) {
			ret << "\nExpected : " << testCase.expectedOutput;
		}
		else
			ret << "\nError    : " << testCase.stdError;
		ret << "\n\n";
	}
	return ret.str();
}