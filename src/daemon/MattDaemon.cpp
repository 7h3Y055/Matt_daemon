#include "MattDaemon.hpp"
#include "SignalHandler.hpp"
#include <unistd.h>

MattDaemon::MattDaemon() : _lockFile("/var/lock/matt_daemon.lock") {}

MattDaemon::MattDaemon(const MattDaemon& other) : _lockFile(other._lockFile) {}

MattDaemon& MattDaemon::operator=(const MattDaemon& other) {
    if (this != &other) {
        _lockFile = other._lockFile;
    }
    return *this;
}

MattDaemon::~MattDaemon() {
    _lockFile.unlock();
}

bool MattDaemon::start() {
    if (!_lockFile.lock()) {
        return false;
    }

    SignalHandler::setup();

    while (!SignalHandler::shouldExit()) {
        sleep(1);
    }

    // The program must quit with the sending of a simple "quit" character chain on the opened socket.
    return true;
}
