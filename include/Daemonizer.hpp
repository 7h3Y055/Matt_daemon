#ifndef DAEMONIZER_HPP
#define DAEMONIZER_HPP

class Daemonizer {
public:
    Daemonizer();
    Daemonizer(const Daemonizer& other);
    Daemonizer& operator=(const Daemonizer& other);
    ~Daemonizer();

    static bool daemonize();
};

#endif // DAEMONIZER_HPP
