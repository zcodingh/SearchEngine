#include "../include/Configuration.h"
#include "../include/nlohmann/json.hpp"
#include <sys/types.h>        
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
using nlohmann::json;

int readn(int fd, char* buf, int len);
int readTrain(int fd, char* buf, int len);
int writen(int fd, const char* buf, int len);
int writeTrain(int fd, const char* buf, int len);

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "error create socket!!!\n"; 
        return -1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    Configuration& conf = Configuration::getInstance();   
    serverAddr.sin_port = htons(std::stoi(conf.getValue("server", "server_port")));
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "fail connect!!!\n";
        close(sockfd);
        return -1;
    }

    string req = "0:武汉";
    writeTrain(sockfd, req.c_str(), req.size());
    char buf[65536] = {0};
    if (readTrain(sockfd, buf, sizeof(buf)) < 0) {
        std::cerr << "readTrain failed !\n";
    }
    json obj = nlohmann::json::parse(buf);
    std::cout << obj.dump() << "\n";

    req = "1:中国科技";
    writeTrain(sockfd, req.c_str(), req.size());
    memset(buf, 0, sizeof(buf));
    if (readTrain(sockfd, buf, sizeof(buf)) < 0) {
        std::cerr << "readTrain failed !\n";
    }
    obj = nlohmann::json::parse(buf);
    std::cout << obj.dump() << "\n";
}

int readTrain(int fd, char* buf, int len) {
    int train_len = 0;
    if (readn(fd, reinterpret_cast<char*>(&train_len), sizeof(train_len)) <= 0) {
        return -1;
    }
    std::cout << "train len = " << train_len << "\n";   //TODO remove
    if (train_len > len) {
        std::cerr << "Buffer too small to hold data\n";
        return -1;
    }
    return readn(fd, buf, train_len);
}

int readn(int fd, char* buf, int len) {
    int left = len;
    int ret = 0;
    char* pbuf = buf;
    while (left > 0) {
        ret = read(fd, pbuf, left);
        if (ret == -1 && errno == EINTR) {
            continue;
        } else if (ret == -1) {
            perror("read error -1");
            return -1;
        } else if (ret == 0) {
            break;
        } else {
            left -= ret;
            pbuf += ret;
        }
    }
    return len - left;
}


int writen(int fd, const char* buf, int len) {
    int left = len;
    int ret = 0;
    const char* pbuf = buf;
    while (left > 0) {
        ret = write(fd, pbuf, left);
        if (ret == -1 && errno == EINTR) {
            continue;
        } else if (ret == -1) {
            perror("write error -1");
            return -1;
        } else if (ret == 0) {
            break;
        } else {
            left -= ret;
            pbuf += ret;
        }
    }
    return len - left;
}

int writeTrain(int fd, const char* buf, int len) {
    std::cout << "write train len = " << len << "\n";           // TODO rm
    if (writen(fd, reinterpret_cast<char*>(&len), sizeof(len)) <= 0) {
        return -1;
    }
    int ret = writen(fd, buf, len);
    std::cout << "write ret " << ret << "bytes\n";              // TODO rm
    return ret;
}
