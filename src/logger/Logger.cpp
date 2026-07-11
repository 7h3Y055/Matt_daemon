#include "Logger.hpp"
#include <errno.h>
#include <cstring>
#include <iostream>

std::ofstream Logger::_fileStream;
std::string Logger::_filepath;
bool Logger::_initialized = false;

Logger::Logger() {}
Logger::Logger(const Logger& other) { (void)other; }
Logger& Logger::operator=(const Logger& other) { (void)other; return *this; }
Logger::~Logger() {}

bool Logger::init(const std::string& filepath) {
    if (_initialized) {
        return true;
    }

    _filepath = filepath;

    _fileStream.open(_filepath.c_str(), std::ios_base::out | std::ios_base::app);
    if (!_fileStream.is_open()) {
        std::cerr << "Failed to open log file " << _filepath << ": " << std::strerror(errno) << std::endl;
        return false;
    }

    _initialized = true;
    return true;
}

void Logger::log(const std::string& message) {
    if (_initialized && _fileStream.is_open()) {
        _fileStream << message << std::endl;
    }
}

void Logger::close() {
    if (_initialized) {
        if (_fileStream.is_open()) {
            _fileStream.close();
        }
        _initialized = false;
    }
}
