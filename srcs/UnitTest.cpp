#include "UnitTest.hpp"

std::ostream &operator<<(std::ostream &os, const t_test_case &testCase)
{
	os << "    Input    : " << testCase.argv;
	os << "\n    Output   : " << testCase.actualOutput << "\n------------------------------";
	if (!testCase.error)
	{
		os << "\n    Expected : " << testCase.expectedOutput;
	}
	else
		os << "\n    Error    : " << testCase.stdError;
	os << '\n' << std::endl;
	return os;
}

UnitTest::UnitTest(std::string directory, std::string execute) : directory_(directory), CC_CFlags_(execute) {}

void UnitTest::addRequiredFile(const std::string &filename)
{
	requiredFilePaths_.push_back(directory_ + "/" + filename);
}

void UnitTest::addTemporaryFile(const std::string &content)
{
	TemporaryFile tempFile(content);
	allTemporaryFiles_.push_back(tempFile);
}

void UnitTest::addTemporaryMainFile(const std::string &templates, const std::string &contents)
{
	static const char *MAIN_START = "int main(int argc, char **argv)\n{(void)argc;(void)argv;";
	static const char *MAIN_END = "\n}\n";
	addTemporaryCodeFile(templates + MAIN_START + contents + MAIN_END);
}

void UnitTest::addTemporaryCodeFile(const std::string &content)
{
	static const std::string headers = "#include <stdio.h>\n#include <stdlib.h>\n#include <unistd.h>\n#include <string.h>\n#include <math.h>\n";
	allTempCodeFiles_.push_back(headers + content);
}

void UnitTest::addTestCase(const std::string &argv, const std::string &expectedOutput)
{
	t_test_case test_case;
	test_case.expectedOutput = expectedOutput;
	test_case.argv = argv;
	allTestCase_.push_back(test_case);
}

bool UnitTest::run()
{
	try
	{
		compile();
		bool ok = runAllTestCase();
		if (ok)
		{
			std::cout << color::GREEN << directory_ << ": OK\n"
					  << color::RESET;
		}
		else
		{
			std::cout << color::RED << directory_ << ": KO\n"
					  << color::RESET;
			printKOTestCase();
		}
		return true;
	}
	catch (const std::exception &e)
	{
		std::cout << directory_ << ": KO\n"
				  << e.what() << std::endl;
		return false;
	}
}

void UnitTest::compile()
{
	std::string compileCommand = CC_CFlags_;

	for (const std::string &filePath : requiredFilePaths_)
	{
		if (*(----filePath.end()) != '.' && *(--filePath.end()) != 'c')
			continue;
		compileCommand += " " + filePath;
	}
	for (const TemporaryFile &file : allTempCodeFiles_)
	{
		compileCommand += " " + file.filename();
	}
	compileCommand += " -o a.out 2> /dev/null";
	int compileResult = std::system(compileCommand.c_str());

	if (compileResult != 0)
	{
		throw std::runtime_error("Compilation failed");
	}
}

bool UnitTest::runAllTestCase()
{
	bool ok = true;
	for (t_test_case &test_case : allTestCase_)
	{
		ok = runTestCase(test_case) && ok;
	}
	return ok;
}

bool UnitTest::runTestCase(t_test_case &test_case)
{
	TemporaryFile outputFile;
	TemporaryFile errorFile;
	std::string runCommand = "./a.out " + test_case.argv + " > " + outputFile.filename() + " 2> " + errorFile.filename();
	std::system(runCommand.c_str());
	test_case.actualOutput = outputFile.readContent();
	test_case.stdError = errorFile.readContent();
	test_case.ok = (test_case.actualOutput == test_case.expectedOutput);
	test_case.error = test_case.stdError != "";
	return test_case.ok;
}

void UnitTest::printKOTestCase()
{
	for (size_t i = 0; i < allTestCase_.size(); i++)
	{
		auto &test_case = allTestCase_[i];
		if (test_case.ok)
			continue;
		std::cout << "Case " << i << ":\n";
		std::cout << test_case;
	}
}