#include "../include/CacheManager.h"
#include <iostream>     // DEBUG
#include <mutex>
using std::cout;

std::unique_ptr<CacheManager> CacheManager::_instance(new CacheManager());

CacheManager& CacheManager::getInstance() {
    return *_instance;
}

LRUCache* CacheManager::getThreaadCache(pid_t threadId) {
    auto it = _pid_cache_map.find(threadId);
    if (it != _pid_cache_map.end()) {
        return it->second;
    } else {
        _pid_cache_map[threadId] = new LRUCache();
        return _pid_cache_map[threadId];
    }
}

void CacheManager::syncCaches() {
    if (_pid_cache_map.empty()) {
        return;
    }
    cout << "syncCaches\n";         // TODO rm
    
    for (auto it = _pid_cache_map.begin(); it != _pid_cache_map.end(); ++it) {
        it->second->syncToMasterCache(_master_cache);
    }
    std::cout << "pass for 1\n";    // TODO rm  
    for (auto it = _pid_cache_map.begin(); it != _pid_cache_map.end(); ++it) {
        it->second->syncFromMasterCache(_master_cache);
    }

    cout << "syncCaches finished!\n";      // TODO rm
    for (auto it = _pid_cache_map.begin(); it != _pid_cache_map.end(); ++it) {
        it->second->showResultList();
    }
}