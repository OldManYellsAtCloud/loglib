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

void loglib::sendLog(string msg, LOG_LEVEL logLevel)
{
    int nameLength = name_.length() + 1;
    int msgLength = msg.length() + 1;
    int messageSize = sizeof(int) * 4 + nameLength + msgLength;
    std::vector<uint8_t> message(messageSize);
    REQUEST_TYPE rt = REQUEST_TYPE::LOG_MESSAGE;
    int current_offset = 0;
    memcpy(message.data(), &rt, sizeof(rt));
    current_offset += sizeof(rt);
    memcpy(message.data() + current_offset, &nameLength, sizeof(nameLength));
    current_offset += sizeof(nameLength);
    memcpy(message.data() + current_offset, name_.data(), nameLength);
    current_offset += nameLength;
    memcpy(message.data() + current_offset, &msgLength, sizeof(msgLength));
    current_offset += sizeof(msgLength);
    memcpy(message.data() + current_offset, msg.data(), msgLength);
    current_offset += msgLength;
    memcpy(message.data() + current_offset, &logLevel, sizeof(logLevel));
    sendMessage(message);
}

bool loglib::fileExists(string path)
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

void loglib::setName(string name)
{
    name_ = name;
}

void loglib::registerLogger(LOGGER_TYPE loggerType)
{
    size_t messageSize = sizeof(int) * 3 + name_.length() + 1;
    std::vector<uint8_t> message(messageSize);

    REQUEST_TYPE rt = REQUEST_TYPE::NEW_LOGGER;
    int nameLength = name_.length() + 1;
    int offset = 0;
    memcpy(message.data(), &rt, sizeof(rt));
    offset += sizeof(rt);
    memcpy(message.data() + offset, &nameLength, sizeof(int));
    offset += sizeof(int);
    memcpy(message.data() + offset, name_.data(), nameLength);
    offset += nameLength;
    memcpy(message.data() + offset, &loggerType, sizeof(loggerType));

    sendMessage(message);
}

void loglib::info(string msg)
{
    sendLog(msg, LOG_LEVEL::INFO);
}

void loglib::debug(string msg)
{
    sendLog(msg, LOG_LEVEL::DEBUG);
}

void loglib::warning(string msg)
{
    sendLog(msg, LOG_LEVEL::WARNING);
}

void loglib::error(string msg)
{
    sendLog(msg, LOG_LEVEL::ERROR);
}

void loglib::fatal(string msg)
{
    sendLog(msg, LOG_LEVEL::FATAL);
}
