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
    : UnitTestException("Nothing Turned In", "") {}

RequiredFileNotFound::RequiredFileNotFound(const std::string& message)
    : UnitTestException("Required File Not Found", message) {}

CompilationError::CompilationError(const std::string& message)
    : UnitTestException("Cannot Compile", message) {}
