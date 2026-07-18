#include "MattDaemon.hpp"
#include "SignalHandler.hpp"
#include "Tintin_reporter.hpp"
#include <unistd.h>
#include <sstream>

MattDaemon::MattDaemon() : _lockFile("/var/lock/matt_daemon.lock"), _server(4242) {}

MattDaemon::MattDaemon(const MattDaemon& other) : _lockFile(other._lockFile), _server(4242) {}

MattDaemon& MattDaemon::operator=(const MattDaemon& other) {
    if (this != &other) {
        _lockFile = other._lockFile;
    }
    return *this;
}

MattDaemon::~MattDaemon() {
    _lockFile.unlock();
}

bool MattDaemon::lock() {
    return _lockFile.lock();
}

bool MattDaemon::start() {
    if (!_lockFile.lock()) {
        return false;
    }

    Tintin_reporter::info("Creating server.");
    if (!_server.init()) {
        return false;
    }
    Tintin_reporter::info("Server created.");

    Tintin_reporter::info("Entering Daemon mode.");

    SignalHandler::setup();

    std::stringstream ss;
    ss << "started. PID: " << ::getpid() << ".";
    Tintin_reporter::info(ss.str());

    _server.run();

    // The program must quit with the sending of a simple "quit" character chain on the opened socket.
    return true;
}
