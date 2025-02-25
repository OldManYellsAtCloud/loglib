#ifndef LOGLIB_INCLUDE_LOGLIB_H
#define LOGLIB_INCLUDE_LOGLIB_H

#include <string>
#include <queue>
#include <cstdint>
#include <log/enums.h>
#include <sys/un.h>
#include <format>

#ifdef STRICT_REGISTRATION
#include <set>
#endif

#define LOG_SOCKET "/tmp/log_sock"
#define MAX_BUFFER_LENGTH 1024

// Log simple text (without need for formatting) with default name
#define LOG_INFO(msg)      loglib::logger().info(msg)
#define LOG_DEBUG(msg)     loglib::logger().debug(msg)
#define LOG_WARNING(msg)   loglib::logger().warning(msg)
#define LOG_ERROR(msg)     loglib::logger().error(msg)
#define LOG_FATAL(msg)     loglib::logger().fatal(msg)

// Log simple text (without need for formatting) with specified name
// Parameter order is flipped for the sake of consistency, as variadic
// argument list must appear at the end, for later macros.
#define LOG_INFO_N(name, msg)      loglib::logger().info(msg, name)
#define LOG_DEBUG_N(name, msg)     loglib::logger().debug(msg, name)
#define LOG_WARNING_N(name, msg)   loglib::logger().warning(msg, name)
#define LOG_ERROR_N(name, msg)     loglib::logger().error(msg, name)
#define LOG_FATAL_N(name, msg)     loglib::logger().fatal(msg, name)

// Log formatted text with default name
#define LOG_INFO_F(...)     LOG_INFO(std::format(__VA_ARGS__))
#define LOG_DEBUG_F(...)    LOG_DEBUG(std::format(__VA_ARGS__))
#define LOG_WARNING_F(...)  LOG_WARNING(std::format(__VA_ARGS__))
#define LOG_ERROR_F(...)    LOG_ERROR(std::format(__VA_ARGS__))
#define LOG_FATAL_F(...)    LOG_FATAL(std::format(__VA_ARGS__))

// Log formatted text with specified name
#define LOG_INFO_NF(name, ...)     LOG_INFO_N(name, std::format(__VA_ARGS__))
#define LOG_DEBUG_NF(name, ...)    LOG_DEBUG_N(name, std::format(__VA_ARGS__))
#define LOG_WARNING_NF(name, ...)  LOG_WARNING_N(name, std::format(__VA_ARGS__))
#define LOG_ERROR_NF(name, ...)    LOG_ERROR_N(name, std::format(__VA_ARGS__))
#define LOG_FATAL_NF(name, ...)    LOG_FATAL_N(name, std::format(__VA_ARGS__))

class loglib{
private:
    int logsocket;
    std::string defaultName_;
    sockaddr_un* sockaddr;
    logging::LOG_LEVEL minLogLevel = logging::LOG_LEVEL::INFO;

#ifdef STRICT_REGISTRATION
    std::set<std::string> loggerList;
#endif

    loglib();
    void trySetupConnection();
    void sendMessage(std::vector<uint8_t> message);
    void sendLog(std::string msg, logging::LOG_LEVEL logLevel, std::string name);
    void sendLog(std::string msg, logging::LOG_LEVEL logLevel);

    bool ready = false;
    std::queue<std::vector<uint8_t>> buffer;
    bool fileExists(std::string path);
    void flushBuffer();
public:
    static loglib& logger(){
        static loglib l;
        return l;
    }

    /**
     * @brief setDefaultName
     * Set the default name that is used for subsequent logging and
     * logger-registration requests, unless specified explicitly.
     * @param name The name to use as default.
     */
    void setDefaultName(std::string name);

    /**
     * @brief setName alias for setDefaultName
     * Deprecated, use setDefaultName instead.
     * @param name The name to use as default.
     */
    void setName(std::string name);

    /**
     * @brief setMinLogLevel Set the minimum logging level.
     * Log requests below this minimum are discarded. Default
     * is INFO.
     * @param logLevel
     */
    void setMinLogLevel(logging::LOG_LEVEL logLevel);

    /**
     * @brief registerLogger Register a new logger.
     * Register a new Logger, using the default name, and the specified
     * logger type.
     * @param loggerType
     */
    void registerLogger(logging::LOGGER_TYPE loggerType);

    /**
     * @brief registerLogger Register a new logger.
     * Register a new Logger, using the specified name and the specified
     * logger type.
     * @param loggerType
     * @param name
     */
    void registerLogger(logging::LOGGER_TYPE loggerType, std::string name);

    loglib(const loglib&) = delete;
    void operator=(const loglib&) = delete;

    /**
     * @brief info Info level log message using the default name.
     * @param msg
     */
    void info(std::string msg);

    /**
     * @brief debug Debug level log message using the default name.
     * @param msg
     */
    void debug(std::string msg);

    /**
     * @brief warning Warning level log message using the default name.
     * @param msg
     */
    void warning(std::string msg);

    /**
     * @brief error Error level log message using the default name.
     * @param msg
     */
    void error(std::string msg);

    /**
     * @brief fatal Fatal level log message using the default name.
     * @param msg
     */
    void fatal(std::string msg);

    /**
     * @brief info Info level log message using the specified name.
     * @param msg
     * @param name
     */
    void info(std::string msg, std::string name);

    /**
     * @brief debug Debug level log message using the specified name.
     * @param msg
     * @param name
     */
    void debug(std::string msg, std::string name);

    /**
     * @brief warning Warning level log message using the specified name.
     * @param msg
     * @param name
     */
    void warning(std::string msg, std::string name);

    /**
     * @brief error Error level log message using the specified name.
     * @param msg
     * @param name
     */
    void error(std::string msg, std::string name);

    /**
     * @brief fatal Fatal level log message using the specified name.
     * @param msg
     * @param name
     */
    void fatal(std::string msg, std::string name);
};
#endif
