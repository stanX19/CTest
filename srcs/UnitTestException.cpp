#include "UnitTestException.hpp"

UnitTestException::UnitTestException(const std::string& type, const std::string& message)
    : type_(type), message_(message) {}

const char* UnitTestException::what() const noexcept {
    return message_.c_str();
}

const char* UnitTestException::type() const {
    return type_.c_str();
}

TestCaseKO::TestCaseKO(const std::string& message)
    : UnitTestException("KO!", message) {}

NothingTurnedIn::NothingTurnedIn()
    : UnitTestException("Nothing turned in", "") {}

FileNotFoundError::FileNotFoundError(const std::string& message)
    : UnitTestException("Required file not Found", message) {}

CompilationError::CompilationError(const std::string& message)
	: UnitTestException("Compilation error", message) {}

InvalidDirectory::InvalidDirectory(const std::string& message)
	: UnitTestException("No such file or directory", message) {}

InvalidFlagError::InvalidFlagError(const std::string& message)
	: UnitTestException("Invalid flag", message) {}