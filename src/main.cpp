#include <iostream>
#include <unistd.h>
#include <cstdlib>

int main (int argc, char **argv, char **envp){
    (void)argc;
    (void)argv;
    (void)envp;

    if (geteuid() != 0) {
        std::cerr << "You must be root to run this program" << std::endl;
        return (1);
    }


    std::cout << "Hello World: " << geteuid() << std::endl;
    return (0);
}

