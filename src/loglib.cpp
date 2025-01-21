#include "loglib.h"
#include "string.h"
#include <sys/socket.h>

#include <filesystem>
#include <iostream>

loglib::loglib()
{
    trySetupConnection();
    sockaddr = new sockaddr_un {
        .sun_family = AF_UNIX,
        .sun_path = LOG_SOCKET
    };
}

void loglib::trySetupConnection()
{
    int ret;
    if (!fileExists(LOG_SOCKET)){
        std::cerr << LOG_SOCKET << " doesn't exist yet." << std::endl;
        return;
    }

    logsocket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (logsocket < 0){
        std::cerr << "Could not open socket: " << strerror(errno) << std::endl;
        return;
    }

    ret = connect(logsocket, (struct sockaddr*)sockaddr, sizeof(*sockaddr));
    if (ret < 0){
        std::cerr << "Could not connect to socket: " << strerror(errno) << std::endl;
        return;
    }
    ready = true;
}

void loglib::sendMessage(std::vector<uint8_t> message)
{
    if (!ready){
        std::cout << "Trying to setup late connection to log server..." << std::endl;
        trySetupConnection();
    }

    if (ready) {
        int ret = send(logsocket, message.data(), message.size(), 0);
        if (ret < 0) {
            std::cerr << "Could not send message (discarded): " << strerror(errno) << std::endl;
            ready = false;
        }
    } else {
        std::cerr << "Storing message for later distribution..." << std::endl;
        if (buffer.size() < MAX_BUFFER_LENGTH) buffer.push(message);
    }
}

void loglib::sendLog(std::string msg, logging::LOG_LEVEL logLevel, std::string name)
{
    if (name.empty()){
        std::cerr << "Name is empty. Make sure to call registerLogger() first." << std::endl;
        return;
    }

#ifdef STRICT_REGISTRATION
    if (!loggerList.contains(name)){
        std::cerr << name << " has no loggers registered!" << std::endl;
        return;
    }
#endif

    int nameLength = defaultName_.length() + 1;
    int msgLength = msg.length() + 1;
    int messageSize = sizeof(int) * 4 + nameLength + msgLength;
    std::vector<uint8_t> message(messageSize);
    logging::REQUEST_TYPE rt = logging::REQUEST_TYPE::LOG_MESSAGE;
    int current_offset = 0;
    memcpy(message.data(), &rt, sizeof(rt));
    current_offset += sizeof(rt);
    memcpy(message.data() + current_offset, &nameLength, sizeof(nameLength));
    current_offset += sizeof(nameLength);
    memcpy(message.data() + current_offset, name.data(), nameLength);
    current_offset += nameLength;
    memcpy(message.data() + current_offset, &msgLength, sizeof(msgLength));
    current_offset += sizeof(msgLength);
    memcpy(message.data() + current_offset, msg.data(), msgLength);
    current_offset += msgLength;
    memcpy(message.data() + current_offset, &logLevel, sizeof(logLevel));
    sendMessage(message);
}

void loglib::sendLog(std::string msg, logging::LOG_LEVEL logLevel)
{
    sendLog(msg, logLevel, defaultName_);
}

bool loglib::fileExists(std::string path)
{
    std::filesystem::directory_entry de {path};
    return de.exists();
}

void loglib::flushBuffer()
{
    if (!ready || buffer.empty()) return;

    while (!buffer.empty()){
        sendMessage(buffer.front());
        buffer.pop();
    }
}

void loglib::setDefaultName(std::string name)
{
    defaultName_ = name;
}

void loglib::setName(std::string name)
{
    std::cerr << "Note to self: Setname is deprecated, don't forget "
                 "to switch to setDefaultName" << std::endl;
    setDefaultName(name);
}

void loglib::setMinLogLevel(logging::LOG_LEVEL logLevel)
{
    minLogLevel = logLevel;
}

void loglib::registerLogger(logging::LOGGER_TYPE loggerType)
{
    registerLogger(loggerType, defaultName_);
}

void loglib::registerLogger(logging::LOGGER_TYPE loggerType, std::string name)
{
    if (name.empty()){
        std::cerr << "No logger name is specified!" << std::endl;
        return;
    }

    size_t messageSize = sizeof(int) * 3 + name.length() + 1;
    std::vector<uint8_t> message(messageSize);

    logging::REQUEST_TYPE rt = logging::REQUEST_TYPE::NEW_LOGGER;
    int nameLength = name.length() + 1;
    int offset = 0;
    memcpy(message.data(), &rt, sizeof(rt));
    offset += sizeof(rt);
    memcpy(message.data() + offset, &nameLength, sizeof(int));
    offset += sizeof(int);
    memcpy(message.data() + offset, name.data(), nameLength);
    offset += nameLength;
    memcpy(message.data() + offset, &loggerType, sizeof(loggerType));

    sendMessage(message);

#ifdef STRICT_REGISTRATION
    loggerList.insert(name);
#endif
}

void loglib::info(std::string msg)
{
    info(msg, defaultName_);
}

void loglib::debug(std::string msg)
{
    debug(msg, defaultName_);
}

void loglib::warning(std::string msg)
{
    warning(msg, defaultName_);
}

void loglib::error(std::string msg)
{
    error(msg, defaultName_);
}

void loglib::fatal(std::string msg)
{
    fatal(msg, defaultName_);
}


void loglib::info(std::string msg, std::string name)
{
    if (minLogLevel > logging::LOG_LEVEL::INFO)
        return;
    sendLog(msg, logging::LOG_LEVEL::INFO, name);
}

void loglib::debug(std::string msg, std::string name)
{
    if (minLogLevel > logging::LOG_LEVEL::DEBUG)
        return;
    sendLog(msg, logging::LOG_LEVEL::DEBUG, name);
}

void loglib::warning(std::string msg, std::string name)
{
    if (minLogLevel > logging::LOG_LEVEL::WARNING)
        return;
    sendLog(msg, logging::LOG_LEVEL::WARNING, name);
}

void loglib::error(std::string msg, std::string name)
{
    if (minLogLevel > logging::LOG_LEVEL::ERROR)
        return;
    sendLog(msg, logging::LOG_LEVEL::ERROR, name);
}

void loglib::fatal(std::string msg, std::string name)
{
    sendLog(msg, logging::LOG_LEVEL::FATAL, name);
}
