#include "Utils.hpp"
#include <ctime>

namespace Utils {
    std::string getCurrentTimestamp() {
        std::time_t rawtime;
        std::time(&rawtime);
        struct std::tm* timeinfo = std::localtime(&rawtime);
        char buffer[80];
        // Format: [DD/MM/YYYY - HH:MM:SS]
        std::strftime(buffer, sizeof(buffer), "[%d/%m/%Y - %H:%M:%S]", timeinfo);
        return std::string(buffer);
    }
}
