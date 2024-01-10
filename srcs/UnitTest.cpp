#include "UnitTest.hpp"

bool UnitTestconfig::parseArgv(int argc, char** argv) {
    using namespace UnitTestconfig;
    for (int i = 1; i < argc; i++) {
        std::string arg(argv[i]);
        if (arg == "-a") {
            showAll = true;
        } else if (arg == "-k") {
            showKO = true;
        } else if (arg == "-d") {
            showKODetails = true;
        } else {
            std::cerr << "Error: Unrecognized option '" << arg << "'\n";
            std::cerr << "Usage: " << argv[0] << " [-a] [-k] [-d]\n"
					  << "Options:\n"
					  << "  -a : Show all test results\n"
					  << "  -k : Show only failed test results\n"
					  << "  -d : Show details of failed test cases\n";
            return false; // Indicate error
        }
    }
    return true; // Parsing successful
}

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
	static const char *MAIN_START = "\nint main(int argc, char **argv)\n{(void)argc;(void)argv;";
	static const char *MAIN_END = "}";
	addTemporaryCodeFile(templates + MAIN_START + contents + MAIN_END);
}

// will be compiled together
void UnitTest::addTemporaryCodeFile(const std::string &content) {
	static const std::string headers = "\n#include <stdio.h>\n#include <stdlib.h>\n#include <unistd.h>\n#include <string.h>\n#include <math.h>\n";
	allTempCodeFiles_.push_back(headers + content);
}

void UnitTest::addTestCase(const std::string &argv, const std::string &expectedOutput) {
	t_test_case test_case;
	test_case.expectedOutput = expectedOutput;
	test_case.argv = argv;
	allTestCase_.push_back(test_case);
}

bool UnitTest::OverallOk() {
	return std::all_of(allTestCase_.begin(), allTestCase_.end(),
		[](const t_test_case &test_case){ return test_case.ok; }
	);
}

bool UnitTest::printStatus() {
	bool Allok = OverallOk();
	if (Allok)
		utils::printOK(directory_);
	else
		utils::printKO(directory_);
	printTestCase();
	return Allok;
}

bool UnitTest::run() {
	try	{
		compile();
		runAllTestCase();
		printStatus();
	}
	catch (const std::exception &e)
	{
		utils::printKO(directory_);
		std::cout << e.what() << std::endl;
	}
	return OverallOk();
}

void UnitTest::compile()
{
	std::string compileCommand = CC_ + " " + CFLAGS_;

	for (const std::string &filePath : requiredFilePaths_)
	{
		if (utils::getFileExtension(filePath) != "c")
			continue;
		compileCommand += " " + filePath;
	}
	for (const TemporaryFile &file : allTempCodeFiles_)
	{
		compileCommand += " " + file.filename();
	}
	compileCommand += " -o " + executableFile_.filename() + " 2> /dev/null";
	int compileResult = std::system(compileCommand.c_str());

	if (compileResult != 0)
	{
		throw std::runtime_error("Compilation failed");
	}
}

bool UnitTest::runAllTestCase()
{
	for (t_test_case &test_case : allTestCase_)
	{
		runTestCase(test_case);
	}
	return OverallOk();
}

bool UnitTest::runTestCase(t_test_case &test_case)
{
	std::string runCommand = "./" + executableFile_.filename() + " " + test_case.argv + " > " + outputFile_.filename() + " 2> " + errorFile_.filename();
	std::system(runCommand.c_str());

	test_case.actualOutput = outputFile_.readContent();
	test_case.stdError = errorFile_.readContent();

	test_case.error = test_case.stdError != "";
	test_case.ok = (test_case.actualOutput == test_case.expectedOutput) && !test_case.error;
	return test_case.ok;
}

void UnitTest::printTestCase() {
	if (UnitTestconfig::showAll)
		printAllTestCase();
	if (UnitTestconfig::showKO) {
		if (UnitTestconfig::showKODetails)
			printKOTestCaseDetailed();
		else
			printKOTestCaseSimplified();
	}
	
}

void UnitTest::printAllTestCase() {
	for (size_t i = 0; i < allTestCase_.size(); i++)
	{
		if (allTestCase_[i].ok) {
			std::cout << color::GREEN << i + 1 << ".OK ";
		} 
		else
			std::cout << color::RED << i + 1 << ".KO ";
	}
	std::cout << color::RESET <<std::endl;
}

void UnitTest::printKOTestCaseSimplified()
{
	if (OverallOk())
		return ;
	std::cout << "Failed: [";
	for (size_t i = 0; i < allTestCase_.size(); i++)
	{
		auto &test_case = allTestCase_[i];
		if (test_case.ok)
			continue;
		std::cout << "(Case " << i + 1 << ": Input: '" << test_case.argv << "'; Output: '"
				  << test_case.actualOutput << "'), ";
	}
	std::cout << "\b\b]\n";
}

void UnitTest::printKOTestCaseDetailed() {
	for (size_t i = 0; i < allTestCase_.size(); i++)
	{
		auto &testCase = allTestCase_[i];
		if (testCase.ok)
			continue;
		std::cout << "==Case " << i + 1 << std::setw(100 - 6) << std::setfill('=') << "\n"
				  << "Input    : " << testCase.argv
				  << "\nOutput   : " << testCase.actualOutput << "\n"
				  << std::setw(100) << std::setfill('=');
		if (!testCase.error) {
			std::cout << "\nExpected : " << testCase.expectedOutput;
		}
		else
			std::cout << "\nError    : " << testCase.stdError;
	}
}