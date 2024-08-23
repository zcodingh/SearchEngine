#ifndef __REDIS_H__
#define __REDIS_H__

#include <sw/redis++/redis++.h>
#include <string>
#include <memory>
using std::unique_ptr;
using std::string;

class Redis {
public:
    static Redis& getInstance();
    ~Redis() {}
    void hset(int docID, const string& field, const string& value);
    std::optional<string> hget(int docID, const string& field);
    void hdel(int docID, const string& field);

    Redis(const Redis&) = delete;
    Redis& operator=(const Redis&) = delete;
    static unique_ptr<Redis> _instance;
private:
    Redis();

private:
    unique_ptr<sw::redis::Redis> _redis;
};

#endif