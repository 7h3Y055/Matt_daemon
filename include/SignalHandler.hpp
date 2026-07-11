#ifndef SIGNALHANDLER_HPP
#define SIGNALHANDLER_HPP

#include <csignal>

class SignalHandler {
public:
    SignalHandler();
    SignalHandler(const SignalHandler& other);
    SignalHandler& operator=(const SignalHandler& other);
    ~SignalHandler();

    static void setup();
    static bool shouldExit();

private:
    static volatile std::sig_atomic_t _shouldExit;
    static void handleSignal(int signal);
};

#endif // SIGNALHANDLER_HPP
