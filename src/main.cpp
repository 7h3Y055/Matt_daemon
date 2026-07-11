#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include "MattDaemon.hpp"
#include "Daemonizer.hpp"
#include "Log.hpp"

int main (int argc, char **argv, char **envp){
    (void)argc;
    (void)argv;
    (void)envp;

    if (std::getenv("BYPASS_ROOT_CHECK") == NULL && geteuid() != 0) {
        std::cerr << "You must be root to run this program" << std::endl;
        return (1);
    }

    if (!Log::init()) {
        return (1);
    }

    MattDaemon daemon;
    if (!daemon.lock()) {
        Log::close();
        return (1);
    }

    if (!Daemonizer::daemonize()) {
        std::cerr << "Failed to daemonize" << std::endl;
        Log::close();
        return (1);
    }

    Log::info("Started.");

    if (!daemon.start()) {
        Log::close();
        return (1);
    }

    Log::info("Quitting.");
    Log::close();
    return (0);
}

