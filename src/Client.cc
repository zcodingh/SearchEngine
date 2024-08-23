#include "../include/Configuration.h"
#include "../include/nlohmann/json.hpp"
#include <sys/types.h>        
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
using nlohmann::json;
using std::cout;

int readn(int fd, char* buf, int len);
int readTrain(int fd, char* buf, int len);
int writen(int fd, const char* buf, int len);
int writeTrain(int fd, const char* buf, int len);
void printJson(const json& obj);

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

    fd_set rdset;
    FD_ZERO(&rdset);
    FD_SET(STDIN_FILENO, &rdset);
    FD_SET(sockfd, &rdset);
    char buf[65536] = { 0 };
    json obj;
    while (1) {
    fd_set tmp = rdset;  // 在每次循环开始时重置 fd_set
    int nready = select(sockfd + 1, &tmp, nullptr, nullptr, nullptr);
    if (nready < 0) {
        perror("select error");
        break;
    }
    if (FD_ISSET(STDIN_FILENO, &tmp)) {
        // 读取标准输入中的数据
        memset(buf, 0, sizeof(buf));
        int ret = read(STDIN_FILENO, buf, sizeof(buf));
        if (ret <= 0) {
            std::cerr << "read stdin error or EOF\n";
            break;
        }
        writeTrain(sockfd, buf, strlen(buf));
    } else if (FD_ISSET(sockfd, &tmp)) {
        // 从套接字读取数据
        memset(buf, 0, sizeof(buf));
        int ret = readTrain(sockfd, buf, sizeof(buf));
        if (ret > 0) {
            try {
                obj = nlohmann::json::parse(buf);
                printJson(obj);
            } catch (nlohmann::json::parse_error& e) {
                std::cerr << "JSON parse error: " << e.what() << "\n";
            }
        } else {
            std::cerr << "readTrain error or connection closed\n";
            break;
        }
    }
    }
}

void printJson(const json& obj) {
    for (const auto& element : obj) {
        if (element.is_object()) {
            cout << element.dump(4) << "\n"; // 4 是缩进量
        } else {
            cout << element << "\t";
        }
    }
    cout << "\n";
}

int readTrain(int fd, char* buf, int len) {
    int train_len = 0;
    if (readn(fd, reinterpret_cast<char*>(&train_len), sizeof(train_len)) <= 0) {
        return -1;
    }
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
    if (writen(fd, reinterpret_cast<char*>(&len), sizeof(len)) <= 0) {
        return -1;
    }
    int ret = writen(fd, buf, len);
    return ret;
}
