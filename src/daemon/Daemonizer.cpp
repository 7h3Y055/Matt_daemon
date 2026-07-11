#include "Daemonizer.hpp"
#include "Log.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdlib>
#include <errno.h>
#include <cstring>

Daemonizer::Daemonizer() {}

Daemonizer::Daemonizer(const Daemonizer& other) {
    (void)other;
}

Daemonizer& Daemonizer::operator=(const Daemonizer& other) {
    (void)other;
    return *this;
}

Daemonizer::~Daemonizer() {}

bool Daemonizer::daemonize() {
    pid_t pid = ::fork();
    if (pid < 0) {
        Log::error(std::string("fork failed: ") + std::strerror(errno));
        return false;
    }
    // Parent process exits successfully
    if (pid > 0) {
        std::exit(0);
    }

    // Create a new session and become group leader
    if (::setsid() < 0) {
        Log::error(std::string("setsid failed: ") + std::strerror(errno));
        return false;
    }

    // Reset file creation mask
    ::umask(0);

    // Change the working directory to root to release the directory lock
    if (::chdir("/") < 0) {
        Log::error(std::string("chdir failed: ") + std::strerror(errno));
        return false;
    }

    // Close and redirect standard file descriptors to /dev/null
    int devNull = ::open("/dev/null", O_RDWR);
    if (devNull >= 0) {
        ::dup2(devNull, STDIN_FILENO);
        ::dup2(devNull, STDOUT_FILENO);
        ::dup2(devNull, STDERR_FILENO);
        if (devNull > STDERR_FILENO) {
            ::close(devNull);
        }
    } else {
        // Fallback: if open fails, close them directly
        ::close(STDIN_FILENO);
        ::close(STDOUT_FILENO);
        ::close(STDERR_FILENO);
    }

    return true;
}
