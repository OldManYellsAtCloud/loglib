#include <string>
#include <queue>
#include <cstdint>
#include <log/enums.h>
#include <sys/un.h>
#include <format>

#define LOG_SOCKET "/tmp/log_sock"
#define MAX_BUFFER_LENGTH 1024

#define LOG_INFO(msg)      loglib::logger().info(msg)
#define LOG_DEBUG(msg)     loglib::logger().debug(msg)
#define LOG_WARNING(msg)   loglib::logger().warning(msg)
#define LOG_ERROR(msg)     loglib::logger().error(msg)
#define LOG_FATAL(msg)     loglib::logger().fatal(msg)

#define LOG_INFO_F(...)     LOG_INFO(std::format(__VA_ARGS__))
#define LOG_DEBUG_F(...)    LOG_DEBUG(std::format(__VA_ARGS__))
#define LOG_WARNING_F(...)  LOG_WARNING(std::format(__VA_ARGS__))
#define LOG_ERROR_F(...)    LOG_ERROR(std::format(__VA_ARGS__))
#define LOG_FATAL_F(...)    LOG_FATAL(std::format(__VA_ARGS__))

class loglib{
private:
    int logsocket;
    std::string name_;
    sockaddr_un* sockaddr;
    logging::LOG_LEVEL minLogLevel = logging::LOG_LEVEL::INFO;

    loglib();

    void trySetupConnection();
    void sendMessage(std::vector<uint8_t> message);
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
    void setName(std::string name);
    void setMinLogLevel(logging::LOG_LEVEL logLevel);
    void registerLogger(logging::LOGGER_TYPE loggerType);
    loglib(const loglib&) = delete;
    void operator=(const loglib&) = delete;
    void info(std::string msg);
    void debug(std::string msg);
    void warning(std::string msg);
    void error(std::string msg);
    void fatal(std::string msg);

};
