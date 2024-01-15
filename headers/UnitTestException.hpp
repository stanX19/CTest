#ifndef UNIT_TEST_EXCEPTION_HPP
#define UNIT_TEST_EXCEPTION_HPP
#include <stdexcept>
#include <string>

class UnitTestException : public std::exception {
public:
    UnitTestException(const std::string& type, const std::string& message);
    const char* what() const noexcept override;
    const char* type() const;
private:
    std::string type_;
    std::string message_;
};

class TestCaseKO : public UnitTestException {
public:
    TestCaseKO(const std::string& message="");
};

class NothingTurnedIn : public UnitTestException {
public:
    NothingTurnedIn();
};

class RequiredFileNotFound : public UnitTestException {
public:
    RequiredFileNotFound(const std::string& fileNames="");
};

class CompilationError : public UnitTestException {
public:
    CompilationError(const std::string& errorMessage="");
};

#endif // UNIT_TEST_EXCEPTION_HPP
