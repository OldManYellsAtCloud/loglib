#include <string>
#include <queue>
#include <cstdint>
#include <log/enums.h>
#include <sys/un.h>
#include <format>

#define LOG_SOCKET "/tmp/log_sock"
#define MAX_BUFFER_LENGTH 1024

#define LOG_INFO(msg)      loglib::logger().info(msg)
#define LOG_DEBUG(msg)   loglib::logger().debug(msg)
#define LOG_WARNING(msg) loglib::logger().warning(msg)
#define LOG_ERROR(msg)   loglib::logger().error(msg)
#define LOG_FATAL(msg)   loglib::logger().fatal(msg)

#define LOG_INFO_F(...)    LOG_INFO(std::format(__VA_ARGS__))
#define LOG_DEBUG_F(...)    LOG_DEBUG(std::format(__VA_ARGS__))
#define LOG_WARNING_F(...)  LOG_WARNING(std::format(__VA_ARGS__))
#define LOG_ERROR_F(...)  LOG_ERROR(std::format(__VA_ARGS__))
#define LOG_FATAL_F(...)  LOG_FATAL(std::format(__VA_ARGS__))

#define INFO(msg)    LOG_INFO(msg)
#define DEBUG(msg)   LOG_DEBUG(msg)
#define DBG(msg)     LOG_DEBUG(msg)
#define WARNING(msg) LOG_WARNING(msg)
#define ERROR(msg)   LOG_ERROR(msg)
#define FATAL(msg)   LOG_FATAL(msg)

#define INFO_F(...)    LOG_INFO_F(__VA_ARGS__)
#define DEBUG_F(...)   LOG_DEBUG_F(__VA_ARGS__)
#define DBG_F(...)     LOG_DEBUG_F(__VA_ARGS__)
#define WARNING_F(...) LOG_WARNING_F(__VA_ARGS__)
#define ERROR_F(...)   LOG_ERROR_F(__VA_ARGS__)
#define FATAL_F(...)   LOG_FATAL_F(__VA_ARGS__)

using namespace std;

class loglib{
private:
    int logsocket;
    std::string name_;
    sockaddr_un* sockaddr;

    loglib();

    void trySetupConnection();
    void sendMessage(std::vector<uint8_t> message);
    void sendLog(std::string msg, LOG_LEVEL logLevel);
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
    void registerLogger(LOGGER_TYPE loggerType);
    loglib(const loglib&) = delete;
    void operator=(const loglib&) = delete;
    void info(std::string msg);
    void debug(std::string msg);
    void warning(std::string msg);
    void error(std::string msg);
    void fatal(std::string msg);

};
