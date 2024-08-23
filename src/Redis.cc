#include "../include/Redis.h"
#include "../include/Configuration.h"
#include <iostream>
using std::cout;

std::unique_ptr<Redis> Redis::_instance(new Redis());
Redis& Redis::getInstance() {
    if (!_instance) {
        _instance.reset(new Redis());
    }
    return *_instance;
}

Redis::Redis(){
    sw::redis::ConnectionOptions options;
    options.host = Configuration::getInstance().getValue("Redis", "HOST");      
    options.port = std::stoi(Configuration::getInstance().getValue( "Redis", "PORT"));        
    _redis = std::make_unique<sw::redis::Redis>(options);    
}

void Redis::hset(int docID, const string& field, const string& value) {
    std::cout << "insert into " << docID << " " << field << " = " << value << "\n";     // TODO rm
    string hashKey = "doc" + std::to_string(docID);
    _redis->hset(hashKey, field, value);
}

std::optional<string> Redis::hget(int docID, const string& field) {
    string hashKey = "doc" + std::to_string(docID);
    auto val = _redis->hget(hashKey, field);
    if (val) {
        return *val;
    } else {
        return std::nullopt;
    }
}

void Redis::hdel(int docID, const string& field) {
    string hashKey = "doc" + std::to_string(docID);
    _redis->hdel(hashKey, field);
}
