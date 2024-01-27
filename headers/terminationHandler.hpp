#ifndef TERMINATION_HANDLER_HPP
#define TERMINATION_HANDLER_HPP
#include "common.hpp"
#include <csignal>

namespace TerminationHandler {
    using HookedFunction = std::function<void()>;
    using HookedSignalHandler = std::function<void(int)>;

    void hookFunction(HookedFunction function);
    void addSignalHandler(HookedSignalHandler handler);
    void setAsSignalHandler();

} // namespace TerminationHandler

#endif // TERMINATION_HANDLER_HPP

