#ifndef __CACHEMANAGER_H__
#define __CACHEMANAGER_H__

#include "LRUCache.h"
#include <unordered_map>
#include <vector>
#include <memory>
using std::unique_ptr;
using std::unordered_map;
using std::vector;

class CacheManager {
public:
    static CacheManager& getInstance();
    LRUCache* getThreaadCache(pid_t threadId);
    void syncCaches();

private:
    CacheManager() = default;

private:
    LRUCache _master_cache;
    unordered_map<pid_t, LRUCache*> _pid_cache_map;
    static std::unique_ptr<CacheManager> _instance;
};

#endif