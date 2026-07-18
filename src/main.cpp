#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include "MattDaemon.hpp"
#include "Daemonizer.hpp"
#include "Tintin_reporter.hpp"

int main (int argc, char **argv, char **envp){
    (void)argc;
    (void)argv;
    (void)envp;

    if (std::getenv("BYPASS_ROOT_CHECK") == NULL && geteuid() != 0) {
        std::cerr << "You must be root to run this program" << std::endl;
        return (1);
    }

    if (!Tintin_reporter::init()) {
        return (1);
    }

    MattDaemon daemon;
    if (!daemon.lock()) {
        Tintin_reporter::close();
        return (1);
    }

    if (!Daemonizer::daemonize()) {
        std::cerr << "Failed to daemonize" << std::endl;
        Tintin_reporter::close();
        return (1);
    }

    Tintin_reporter::info("Started.");

    if (!daemon.start()) {
        Tintin_reporter::close();
        return (1);
    }

    Tintin_reporter::info("Quitting.");
    Tintin_reporter::close();
    return (0);
}

