#include "../include/SocketIO.h"
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <iostream> //TODO remove

SocketIO::SocketIO(int fd)
: _fd(fd)
{}

SocketIO::~SocketIO()
{}

// int SocketIO::readLine(char* buf, int len) {
//     int left = len - 1;
//     char* pbuf = buf;
//     int ret = 0, total = 0;

//     while (left > 0) {
//         ret = recv(_fd, pbuf, left, MSG_PEEK);
//         if (ret == -1 && errno == EINTR) {
//             continue;
//         } else if (ret == -1) {
//             perror("readLine error -1");
//             return -1;
//         } else if (ret == 0) {
//             break;
//         } else {
//             for (int i = 0; i < ret; ++i) {
//                 if (pbuf[i] == '\n') {
//                     int sz = i + 1;
//                     readn(pbuf, sz);
//                     pbuf += sz;
//                     *pbuf = '\0';
//                     return total + sz;
//                 }
//             }

//             readn(pbuf, ret);
//             total += ret;
//             pbuf += ret;
//             left -= ret;
//         }
//     }
//     *pbuf = '\0';
//     return total - left;
// }

int SocketIO::readTrain(char* buf, int len) {
    int train_len = 0;
    if (readn(reinterpret_cast<char*>(&train_len), sizeof(train_len)) <= 0) {
        return -1;
    } 
    if (train_len > len) {
        std::cerr << "Buffer too small to hold data\n";
        return -1;
    }             
    return readn(buf, train_len);
}

int SocketIO::readn(char* buf, int len) {
    int left = len;
    int ret = 0;
    char* pbuf = buf;
    while (left > 0) {
        ret = read(_fd, pbuf, left);
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


int SocketIO::writen(const char* buf, int len) {
    int left = len;
    int ret = 0;
    const char* pbuf = buf;
    while (left > 0) {
        ret = write(_fd, pbuf, left);
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

int SocketIO::writeTrain(const char* buf, int len) {
    if (writen(reinterpret_cast<char*>(&len), sizeof(len)) <= 0) {
        return -1;
    }
    return writen(buf, len);
}