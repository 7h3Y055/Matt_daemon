#ifndef TINTIN_REPORTER_HPP
#define TINTIN_REPORTER_HPP

#include <string>
#include <fstream>

class Tintin_reporter {
public:
    static bool init(const std::string& filepath = "/var/log/matt_daemon/matt_daemon.log");
    static void info(const std::string& message);
    static void error(const std::string& message);
    static void log(const std::string& message, const std::string& level);
    static void close();

private:
    static std::ofstream _fileStream;
    static std::string _filepath;
    static bool _initialized;

    Tintin_reporter();
    Tintin_reporter(const Tintin_reporter& other);
    Tintin_reporter& operator=(const Tintin_reporter& other);
    ~Tintin_reporter();
};

#endif // TINTIN_REPORTER_HPP
