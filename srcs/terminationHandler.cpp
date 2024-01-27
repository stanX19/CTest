#include "TerminationHandler.hpp"

namespace {
    using HookedFunction = TerminationHandler::HookedFunction;
	using HookedSignalHandler = TerminationHandler::HookedSignalHandler;

    std::vector<HookedFunction>& getHookedFunctions() {
        static std::vector<HookedFunction> hookedFunctions;
        return hookedFunctions;
    }
	std::vector<HookedSignalHandler>& getHookedSignalHandlers() {
        static std::vector<HookedSignalHandler> hookedSignalHandler;
        return hookedSignalHandler;
    }

    void signalHandler(int signum) {
        for (const auto& hookedFunc : getHookedFunctions()) {
            hookedFunc();
        }
		for (const auto& handlerFunc : getHookedSignalHandlers()) {
            handlerFunc(signum);
        }
        std::exit(signum);
    }
}

namespace TerminationHandler {
    void hookFunction(HookedFunction function) {
        getHookedFunctions().push_back(function);
    }

	void addSignalHandler(HookedSignalHandler function) {
        getHookedSignalHandlers().push_back(function);
    }

    void setAsSignalHandler() {
        signal(SIGINT, signalHandler);
        signal(SIGTERM, signalHandler);
    }
} // namespace TerminationHandler
