#ifndef MATTDAEMON_HPP
#define MATTDAEMON_HPP

#include "LockFile.hpp"
#include "Server.hpp"

class MattDaemon {
public:
    MattDaemon();
    MattDaemon(const MattDaemon& other);
    MattDaemon& operator=(const MattDaemon& other);
    ~MattDaemon();

    bool start();

private:
    LockFile _lockFile;
    Server _server;
};

#endif // MATTDAEMON_HPP
