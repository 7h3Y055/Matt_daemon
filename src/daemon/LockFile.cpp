#include "LockFile.hpp"
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/file.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <sstream>
#include <iostream>

LockFile::LockFile(const std::string& path)
    : _path(path), _fd(-1), _isLocked(false) {}

LockFile::LockFile(const LockFile& other)
    : _path(other._path), _fd(-1), _isLocked(false) {}

LockFile& LockFile::operator=(const LockFile& other) {
    if (this != &other) {
        unlock();
        _path = other._path;
        _fd = -1;
        _isLocked = false;
    }
    return *this;
}

LockFile::~LockFile() {
    unlock();
}

bool LockFile::lock() {
    if (_isLocked) {
        return true;
    }

    size_t lastSlash = _path.find_last_of('/');
    if (lastSlash != std::string::npos) {
        std::string dirPath = _path.substr(0, lastSlash);
        struct stat st = {};
        if (stat(dirPath.c_str(), &st) == -1) {
            mkdir(dirPath.c_str(), 0755);
        }
    }

    _fd = ::open(_path.c_str(), O_RDWR | O_CREAT, 0640);

    if (_fd < 0) {
        std::cerr << "Can't open :" << _path << std::endl;
        return false;
    }

    if (::flock(_fd, LOCK_EX | LOCK_NB) < 0) {
        if (errno == EWOULDBLOCK || errno == EAGAIN) {
            std::cerr << "Can't open :" << _path << std::endl;
        } else {
            std::cerr << "Can't open :" << _path << std::endl;
        }
        ::close(_fd);
        _fd = -1;
        return false;
    }

    if (::ftruncate(_fd, 0) == 0) {
        std::stringstream ss;
        ss << ::getpid() << "\n";
        std::string pidStr = ss.str();
        ssize_t written = ::write(_fd, pidStr.c_str(), pidStr.size());
        (void)written;
    }

    _isLocked = true;
    return true;
}

void LockFile::unlock() {
    if (_isLocked && _fd >= 0) {
        ::flock(_fd, LOCK_UN);
        ::close(_fd);
        _fd = -1;
        _isLocked = false;
        ::unlink(_path.c_str());
    }
}
