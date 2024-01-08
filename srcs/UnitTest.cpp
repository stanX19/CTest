#include "UnitTest.hpp"

std::ostream& operator<<(std::ostream& os, const t_test_case& testCase) {
	os << "    Input    : " << testCase.argv;
	if (!testCase.error) {
		os << "\n    Output   : " << testCase.actualOutput;
		os << "\n    Expected : " << testCase.expectedOutput;
		os << "\n    Status   : " << ((testCase.ok)? "OK": "KO");
	}
	else
		os << "\n    Error    : " << testCase.actualOutput;
	os << std::endl;
	return os;
}

UnitTest::UnitTest(std::string directory, std::string CC, std::string CFLAGS)
    : directory_(directory), CC_(CC), CFLAGS_(CFLAGS) {}

void UnitTest::addRequiredFile(const std::string& filename) {
    requiredFilePaths_.push_back(directory_ + "/" + filename);
}

void UnitTest::addTemporaryFile(const std::string& content) {
    TemporaryFile tempFile(content);
    temporaryFiles_.push_back(tempFile);
}

void UnitTest::addTempCodeFile(const std::string& content) {
	static const std::string headers = "#include <stdio.h>\n#include <stdlib.h>\n#include <unistd.h>\n#include <string.h>\n#include <math.h>";
    TemporaryFile tempFile(headers + content);
    temporaryFiles_.push_back(tempFile);
}

void UnitTest::addTempMainFile(const std::string& content) {
	static const std::string main_start = "int main(int argc, char **argv)\n{\n";
	static const std::string main_end = "\n}";
    TemporaryFile tempFile(main_start + content + main_end);
    temporaryFiles_.push_back(tempFile);
}

void UnitTest::addTestCase(const std::string& argv, const std::string& expectedOutput) {
    t_test_case test_case;
	test_case.expectedOutput = expectedOutput;
    test_case.argv = argv;
    allTestCase_.push_back(test_case);
}

bool UnitTest::run() {
    try {
        compile();
		bool ok = runAllTestCase();
		if (ok) {
			std::cout << directory_ << ": OK\n";
		} else {
			std::cout << directory_ << ": KO\n";
			printKOTestCase();
		}
        return true;
    } catch (const std::exception& e) {
        std::cout << directory_ << ": KO\n" << e.what() << std::endl;
        return false;
    }
}

void UnitTest::compile() {
    std::string compileCommand = CC_ + " " + CFLAGS_;

    for (const auto& filePath : requiredFilePaths_) {
        compileCommand += " " + filePath;
    }
    int compileResult = std::system(compileCommand.c_str());

    if (compileResult != 0) {
        throw std::runtime_error("Compilation failed");
    }
}

bool UnitTest::runAllTestCase() {
	bool ok = true;
    for (auto& test_case : allTestCase_) {
        ok = runTestCase(test_case) && ok;
    }
	return ok;
}

bool UnitTest::runTestCase(t_test_case &test_case) {
	TemporaryFile tempFile;
    std::string runCommand = "./a.out " + test_case.argv + " > " + tempFile.filename();
    int runResult = std::system(runCommand.c_str());
    if (runResult != 0) {
        test_case.error = true;
	}
	test_case.actualOutput = tempFile.readContent();
	if (test_case.actualOutput != test_case.expectedOutput) {
		test_case.ok = false;
	}
	return test_case.ok;
}

void UnitTest::printKOTestCase() {
	for (size_t i = 0; i < allTestCase_.size(); i++) {
		auto &test_case = allTestCase_[i];
		if (test_case.ok)
			continue;
		std::cout << test_case;
	}
}