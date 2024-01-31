#include "UnitTest.hpp"

static void processLetterFlag(char c) {
    using namespace UnitTestconfig;

	switch (c)
	{
	case 'a':
		showAll = true;
		return ;
	case 'l':
		showListCase = true;
		return ;
	case 'f':
		showKO = true;
		return ;
	case 'd':
		showDetails = true;
		return ;
	}
	throw InvalidFlagError(std::string("-") + c);
}

static void processLetterFlags(const std::string &flags) {
	for (const char &c : flags) {
		if (c == '-')
			continue;
		processLetterFlag(c);
	}
}

static void processStringFlags(const std::string &_flag) {
	std::string flag = _flag.substr(2);
	if (flag == "sn") {
		UnitTestconfig::silenceNorm = 1;
		return ;
	}
	throw InvalidFlagError(flag);
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
        // if (arg[0] == '-' && arg[1] == '-' && arg[2] == 0)
		// 	return ;
        if (arg[0] == '-' && arg[1] == '-')
			processStringFlags(arg);
        else if (arg[0] == '-')
			processLetterFlags(arg);
		else
			setTargetDir(arg);
	}
	return ;
}

static void printUsage(char *name) {
	std::cout << "Usage: " << name << " [target_directory] [-a] [-f] [-d] [-l] [--sn]\n"
			  << "\n"
			  << "Runs test case on c language files\n"
			  << "\n"
			  << "Options:\n"
			  << "  -l : List - Display test case status\n"
			  << "  -a : All - Display all test cases\n"
			  << "  -f : Fail - Display only failed test results\n"
			  << "  -d : Details - Display detailed test cases\n"
			  << "  --sn : Silence Norm - ignore norm errors\n"
			  << "\n"
			  << "Arguments:\n"
			  << "  [target_directory] : Optional. Specify the directory to be tested. Default is '.' (current directory).\n"
			  << "\n"
			  << "Example:\n"
			  << "  " << name << " ~/Desktop/C0X -a\n"
			  << "  " << name << " -afd\n"
			  << "  " << name << " -l\n"
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