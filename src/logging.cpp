#include "logging.hpp"

const Logger& logger() {
    static const Logger logger("CustomTypes");
    return logger;
}