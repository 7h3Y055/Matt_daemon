#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include "LockFile.hpp"

int main (int argc, char **argv, char **envp){
    (void)argc;
    (void)argv;
    (void)envp;

    if (std::getenv("BYPASS_ROOT_CHECK") == NULL && geteuid() != 0) {
        std::cerr << "You must be root to run this program" << std::endl;
        return (1);
    }

    std::string lockPath = "/var/lock/matt_daemon.lock";

    LockFile lockFile(lockPath);
    if (!lockFile.lock()) {
        return (1);
    }


    // Simulate daemon running by sleeping in a loop
    while (true) {
        sleep(1);
    }

    return (0);
}

