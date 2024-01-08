#ifndef C_TEST_HPP
# define C_TEST_HPP
#include <iostream>
#include <vector>
#include <sstream>
#include <cstring>
#include <functional>

class CTest {
public:
    // Constructor
    CTest();

    // Function to set required files
    void addRequiredFile(const std::string& filename);

    // Function to set command-line arguments
    void setArgv(const std::vector<std::string>& arguments);

    // Function to set the main function
    void setMainFunction(const std::function<void()>& mainFunction);

    // Function to set expected output
    void setExpectedOutput(const std::string& expectedOutput);

    // Function to add an integer to the main function
    void newInt(int value);

    // Function to add a character to the main function
    void newChar(char value);

    // Function to add a pointer to the main function
    template <typename T>
    void newPtr(const T* ptr);

    // Function to add a printf statement to the main function
    void printf(const char* format, ...);

    // Run the test and check the result
    bool run();

    std::string genMainFunction();

private:
    std::vector<std::string> requiredFiles;
    std::string mainFunction;
    std::string argv;
    std::string expectedOutput;
};

#endif