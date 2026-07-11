#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <fstream>

class Logger {
public:
    static bool init(const std::string& filepath = "/var/log/matt_daemon/matt_daemon.log");
    static void log(const std::string& message);
    static void close();

private:
    static std::ofstream _fileStream;
    static std::string _filepath;
    static bool _initialized;

    Logger();
    Logger(const Logger& other);
    Logger& operator=(const Logger& other);
    ~Logger();
};

#endif // LOGGER_HPP
