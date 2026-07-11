#include "Log.hpp"
#include "Logger.hpp"
#include "Utils.hpp"

Log::Log() {}
Log::Log(const Log& other) { (void)other; }
Log& Log::operator=(const Log& other) { (void)other; return *this; }
Log::~Log() {}

bool Log::init() {
    return Logger::init();
}

void Log::close() {
    Logger::close();
}

void Log::info(const std::string& message) {
    log(message, "INFO");
}

void Log::error(const std::string& message) {
    log(message, "ERROR");
}

void Log::log(const std::string& message, const std::string& level) {
    // Format: [DD/MM/YYYY - HH:MM:SS] [ Level ] - Message
    std::string formattedMsg = Utils::getCurrentTimestamp() + " [ " + level + " ] - " + message;
    Logger::log(formattedMsg);
}
