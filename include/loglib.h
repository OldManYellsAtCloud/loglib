#include <string>
#include <queue>
#include <cstdint>
#include <log/enums.h>
#include <sys/un.h>

#define LOG_SOCKET "/tmp/log_sock"
#define MAX_BUFFER_LENGTH 1024

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
