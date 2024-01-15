#include "UnitTest.hpp"

static bool processOption(char c) {
    using namespace UnitTestconfig;

	switch (c)
	{
	case 'a':
		showAll = true;
		return 0;
	case 'f':
		showKO = true;
		return 0;
	case 'd':
		showKODetails = true;
		return 0;
	}
	return 1;
}

static bool processArgv(int argc, char** argv, std::string &error) {
	for (int i = 1; i < argc; i++) {
        std::string arg(argv[i]);
        if (arg[0] != '-') 
			return 1;
		for (const char &c: arg.substr(1)) {
			if (processOption(c) == 0)
				continue;
			error = std::string("-") + c;
			return 1;
		}
	}
	return 0;
}

bool UnitTestconfig::parseArgv(int argc, char** argv) {
	std::string error;
    if (processArgv(argc, argv, error)) {
		if (error != "") {}
			std::cerr << "Error: Unrecognized option '" << error << "'\n";
		std::cerr << "Usage: " << argv[0] << " [-a] [-f] [-d]\n"
				  << "Options:\n"
				  << "  -a : Show all test results\n"
				  << "  -f : Show only failed test results\n"
				  << "  -d : Show details of failed test cases\n";
		return false; // Indicate error
    }
    return true; // Parsing successful
}