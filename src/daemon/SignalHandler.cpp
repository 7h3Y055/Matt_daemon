#include "SignalHandler.hpp"
#include <cstring>

volatile std::sig_atomic_t SignalHandler::_shouldExit = 0;
volatile std::sig_atomic_t SignalHandler::_signalReceived = 0;

SignalHandler::SignalHandler() {}

SignalHandler::SignalHandler(const SignalHandler& other) {
    (void)other;
}

SignalHandler& SignalHandler::operator=(const SignalHandler& other) {
    (void)other;
    return *this;
}

SignalHandler::~SignalHandler() {}

void SignalHandler::handleSignal(int signal) {
    _shouldExit = 1;
    _signalReceived = signal;
}

void SignalHandler::setup() {
    struct sigaction sa;
    std::memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SignalHandler::handleSignal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    // Register handler for all catchable signals (skip SIGKILL=9 and SIGSTOP=19)
    for (int sig = 1; sig <= 31; ++sig) {
        if (sig == SIGKILL || sig == SIGSTOP)
            continue;
        sigaction(sig, &sa, nullptr);
    }
}

bool SignalHandler::shouldExit() {
    return _shouldExit == 1;
}

int SignalHandler::getSignalReceived() {
    return _signalReceived;
}
