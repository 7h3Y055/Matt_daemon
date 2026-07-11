#ifndef LOG_HPP
#define LOG_HPP

#include <string>

class Log {
public:
    static bool init();
    static void info(const std::string& message);
    static void error(const std::string& message);
    static void log(const std::string& message, const std::string& level);
    static void close();

private:
    Log();
    Log(const Log& other);
    Log& operator=(const Log& other);
    ~Log();
};

#endif // LOG_HPP
