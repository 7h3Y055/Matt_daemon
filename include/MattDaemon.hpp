#ifndef MATTDAEMON_HPP
#define MATTDAEMON_HPP

#include "LockFile.hpp"

class MattDaemon {
public:
    MattDaemon();
    MattDaemon(const MattDaemon& other);
    MattDaemon& operator=(const MattDaemon& other);
    ~MattDaemon();

    bool start();

private:
    LockFile _lockFile;
};

#endif // MATTDAEMON_HPP
