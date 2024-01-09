#ifndef C_TEST_HPP
# define C_TEST_HPP
# include "common.hpp"
# include "TemporaryFile.hpp"

typedef struct s_test_case {
	std::string argv;
	std::string expectedOutput;
	std::string actualOutput;
	bool ok;
	bool error;
} t_test_case;

class UnitTest {
public:
    UnitTest(std::string directory, std::string CC="gcc", std::string CFLAGS="-Wall -Wextra -Werror");
    void addRequiredFile(const std::string& filename);
    void addTemporaryFile(const std::string& content);
    void addTempCodeFile(const std::string& content);
	void addTestCase(const std::string& argv, const std::string& expectedOutput);
    bool run();
private:
    std::vector<std::string> requiredFilePaths_;
	std::vector<TemporaryFile> allTempCodeFiles_;
	std::vector<TemporaryFile> allTemporaryFiles_;
	std::vector<t_test_case> allTestCase_;
    std::string directory_;
    std::string CC_;
	std::string CFLAGS_;

	void compile();
	bool runTestCase(t_test_case &test_case);
	bool runAllTestCase();
	void printKOTestCase();
};

#endif