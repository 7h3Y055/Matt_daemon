#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include "MattDaemon.hpp"

int main (int argc, char **argv, char **envp){
    (void)argc;
    (void)argv;
    (void)envp;

    if (std::getenv("BYPASS_ROOT_CHECK") == NULL && geteuid() != 0) {
        std::cerr << "You must be root to run this program" << std::endl;
        return (1);
    }



    int pid = fork();
    if (pid < 0) {
        std::cerr << "Cant fork" << std::endl;
        return (1);
    }
    if (pid == 0) {
        MattDaemon daemon;
        if (!daemon.start()) {
            return (1);
        }
    }

    return (0);
}

