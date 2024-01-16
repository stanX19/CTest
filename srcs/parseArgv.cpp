#include "UnitTest.hpp"

static void processFlag(char c) {
    using namespace UnitTestconfig;

	switch (c)
	{
	case 'a':
		showAll = true;
		return ;
	case 'f':
		showKO = true;
		return ;
	case 'd':
		showKODetails = true;
		return ;
	}
	throw InvalidFlagError(std::string("-") + c);
}

static void processFlags(const std::string &flags) {
	for (const char &c : flags) {
		if (c == '-')
			continue;
		processFlag(c);
	}
}

static void setTargetDir(const std::string &path) {
	if (utils::pathExists(path)) {
		UnitTestconfig::targetDir = std::filesystem::absolute(path);
		std::filesystem::current_path(path);
	}
	else
		throw InvalidDirectory(path);
}

static void processArgv(int argc, char** argv) {
	for (int i = 1; i < argc; i++) {
        std::string arg(argv[i]);
        if (arg[0] == '-')
			processFlags(arg);
		else
			setTargetDir(arg);
	}
	return ;
}

static void printUsage(char *name) {
	std::cout << "Usage: " << name << " [target_directory] [-a] [-f] [-d]\n"
			  << "\n"
			  << "Runs test case on c language files\n"
			  << "\n"
			  << "Options:\n"
			  << "  -a : Show all test results\n"
			  << "  -f : Show only failed test results\n"
			  << "  -d : Show details of failed test cases\n"
			  << "\n"
			  << "Arguments:\n"
			  << "  [target_directory] : Optional. Specify the directory to be tested. Default is '.' (current directory).\n"
			  << "\n"
			  << "Example:\n"
			  << "  " << name << " ~/Desktop/C0X -a\n"
			  << "  " << name << " -fd\n"
			  << "\n";
}

bool UnitTestconfig::parseArgv(int argc, char** argv) {
    try {
		processArgv(argc, argv);
	}
	catch (const UnitTestException &exc) {
		std::cerr << "Error: " << exc.type() << ": '" << exc.what() << "'\n"
				  << "\n";
		printUsage(argv[0]);
		return false; // Indicate error
    }
    return true; // Parsing successful
}