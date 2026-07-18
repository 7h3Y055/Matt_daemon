#include "Tintin_reporter.hpp"
#include "Utils.hpp"
#include <errno.h>
#include <cstring>
#include <iostream>

std::ofstream Tintin_reporter::_fileStream;
std::string Tintin_reporter::_filepath;
bool Tintin_reporter::_initialized = false;

Tintin_reporter::Tintin_reporter() {}
Tintin_reporter::Tintin_reporter(const Tintin_reporter& other) { (void)other; }
Tintin_reporter& Tintin_reporter::operator=(const Tintin_reporter& other) { (void)other; return *this; }
Tintin_reporter::~Tintin_reporter() {}

bool Tintin_reporter::init(const std::string& filepath) {
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

void Tintin_reporter::info(const std::string& message) {
    log(message, "INFO");
}

void Tintin_reporter::error(const std::string& message) {
    log(message, "ERROR");
}

void Tintin_reporter::log(const std::string& message, const std::string& level) {
    if (_initialized && _fileStream.is_open()) {
        // Format: [DD/MM/YYYY-HH:MM:SS] [ Level ] - Matt_daemon: Message
        std::string formattedMsg = Utils::getCurrentTimestamp() + " [ " + level + " ] - Matt_daemon: " + message;
        _fileStream << formattedMsg << std::endl;
    }
}

void Tintin_reporter::close() {
    if (_initialized) {
        if (_fileStream.is_open()) {
            _fileStream.close();
        }
        _initialized = false;
    }
}
