#ifndef C_TEST_HPP
# define C_TEST_HPP
# include "common.hpp"
# include "TemporaryFile.hpp"
# include "UnitTestException.hpp"
# include "utils.hpp"
# include "color.hpp"

typedef struct s_test_case {
	std::string argv;
	std::string expectedOutput;
	std::string actualOutput;
	std::string stdError;
	bool ok;
	bool error;
} t_test_case;

namespace UnitTestconfig {
	inline bool showKO = 0;
	inline bool showKODetails = 0;
	inline bool showAll = 0;

	bool parseArgv(int argc, char**argv);
}

class UnitTest {
public:
	UnitTest(std::string directory, std::string CC="gcc", std::string CFLAGS="-Wall -Wextra -Werror -fmax-errors=1 -Qunused-arguments");
    void addRequiredFile(const std::string& filename);
    void addTemporaryFile(const std::string& content);
	void addTemporaryMainFile(const std::string& function_templates, const std::string& main_content);
    void addTemporaryCodeFile(const std::string& content);
	void addTestCase(const std::string& argv, const std::string& expectedOutput);
	void printStatus() const;
    bool run();
private:
    std::vector<std::string> requiredFilePaths_;
	std::vector<TemporaryFile> allTempCodeFiles_;
	std::vector<TemporaryFile> allTemporaryFiles_;
	std::vector<t_test_case> allTestCase_;
    std::string directory_;
	TemporaryFile executableFile_;
	TemporaryFile outputFile_;
	TemporaryFile errorFile_;
    std::string CC_;
    std::string CFLAGS_;

	void validateRequiredFiles();
	void compile();
	bool runTestCase(t_test_case &test_case);
	bool runAllTestCase();
	void handleException(const UnitTestException &exc);
	void printAllTestCase();
	void printTestCase();
	std::string getKOMessage() const;
	std::string getKOTestCaseDetailed() const;
	std::string getKOTestCaseSimplified() const;
	bool OverallOk() const;
};

#endif