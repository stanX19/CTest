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
	inline bool showDetails = 0;
	inline bool showAll = 0;
	inline bool showListCase = 0;
	inline bool formatUnprintable = 0;
	inline int lineLength = 100;
	inline std::string targetDir = ".";
	inline static const std::string headers =
		"\n#include <stdio.h>\n\
		#include <stdlib.h>\n\
		#include <unistd.h>\n\
		#include <fcntl.h>\n\
		#include <string.h>\n\
		#include <math.h>\n\
		#include <signal.h>\n";
	inline static const std::string CC = "clang";
	inline static const std::string CFLAGS = "-Wall -Wextra -Werror -fmax-errors=1 -Qunused-arguments -fsanitize=address";
	bool parseArgv(int argc, char**argv);
}

class UnitTest {
public:
	UnitTest(std::string directory=".", int timeout=1, bool displayLineBreak=false);  // , std::string CC="gcc", std::string CFLAGS="-Wall -Wextra -Werror -fmax-errors=1 -Qunused-arguments");
    virtual ~UnitTest() = default;
	void configure(std::string directory=".", int timeout=1, bool displayLineBreak=false);
	void addRequiredFile(const std::string& filename);
    void addTemporaryFile(const std::string& content);
    void addTemporaryCodeFile(const std::string& content);
	virtual void addTemporaryMainFile(const std::string& function_templates, const std::string main_content, const std::string ignore_this="");
	void addTestCase(const std::string& argv, const std::string& expectedOutput);
	void addTestCaseSameInOut(const std::string& inOutStr);
	void setExecutableName(const std::string& name);
	void setDirectory(const std::string& path);
	void setName(const std::string& name);
	void printStatus() const;
    bool run();

protected:
	TemporaryFile executableFile_;
	TemporaryFile actualOutputFile_;
	TemporaryFile errorFile_;
    std::vector<std::string> requiredFilePaths_;
	std::vector<TemporaryFile> allTempCodeFiles_;
	std::vector<TemporaryFile> allTemporaryFiles_;
	std::vector<t_test_case> allTestCase_;
	std::vector<t_test_case> allGenExpectedTestCase_;
    std::string directory_;
    std::string name_;
    std::string CC_;
    std::string CFLAGS_;
	size_t timeout_;
	bool displayLineBreak_;

	virtual bool runTestCase(t_test_case &test_case);

private:
	void validateRequiredFiles();
	void validateNorme();
	bool norminette(const std::string &path);
	void compile();
	void handleException(const UnitTestException &exc);
	bool AllTestCaseOk() const;
	bool runAllTestCase();
	void printAllTestCase() const;
	void printTestCaseInfo() const;
	std::string getTestCaseInfo() const;
	std::string getTestCaseDetailed() const;
	std::string getTestCaseOneLine() const;
	std::string getFormatDisplay(std::string str) const;
	std::string getFormatDisplay(std::string str, std::string cmp) const;
};

class UnitTestGenExpected : public UnitTest {
public:
	UnitTestGenExpected(std::string directory, int timeout=1, bool displayLineBreak=false);
	virtual ~UnitTestGenExpected() = default;
	void addTestCase(const std::string& argv);
	void addTemporaryMainFile(const std::string& function_templates, const std::string printExpected, const std::string printOutput) override;
protected:
	TemporaryFile expectedOutputFile_;

	bool runTestCase(t_test_case &test_case) override;
private:
};

#endif