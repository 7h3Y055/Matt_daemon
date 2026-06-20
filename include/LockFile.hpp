#ifndef LOCKFILE_HPP
#define LOCKFILE_HPP

#include <string>

class LockFile {
public:
    LockFile(const std::string& path = "/var/lock/matt_daemon.lock");
    LockFile(const LockFile& other);
    LockFile& operator=(const LockFile& other);
    ~LockFile();

    bool lock();
    void unlock();

private:
    std::string _path;
    int _fd;
    bool _isLocked;
};

#endif // LOCKFILE_HPP
