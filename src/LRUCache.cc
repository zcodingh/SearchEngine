#include "../include/LRUCache.h"
#include <iostream>
#include <unistd.h>
using std::lock_guard;

LRUCache::LRUCache(size_t num)
: _capacity(num)
{
    _result_list.clear();
    _pending_update_list.clear();
    _hash_map.clear();
}

void LRUCache::syncToMasterCache(LRUCache& masterCache) {
    masterCache.addElement(_pending_update_list);
}

void LRUCache::syncFromMasterCache(const LRUCache& masterCache) {
    addElement(masterCache._pending_update_list);
    _pending_update_list.clear();
}

void LRUCache::addElement(const string& query, const string& value) {
    {
    std::lock_guard<mutex> lock(_mutex);
    auto it = _hash_map.find(query);
    if (it != _hash_map.end()) {
        // 更新现有元素
        it->second->second = value;
        _result_list.splice(_result_list.begin(), _result_list, it->second);
    } else {
        // 添加新元素
        _result_list.emplace_front(query, value);
        _pending_update_list.emplace_front(query, value);       // 添加待更新
        _hash_map[query] = _result_list.begin();
    }
    }
    
    // 检查缓存容量是否超限
    if (_result_list.size() > _capacity) {
        auto del = _result_list.back();
        _hash_map.erase(del.first);  // 删除hash_map中对应的元素
        _result_list.pop_back();     // 删除链表中最后的元素
    }
}


void LRUCache::addElement(const list<std::pair<string, string>>& pendingUpdateList) {
    for (const auto& item : pendingUpdateList) {
        addElement(item.first, item.second);
    }
}

string LRUCache::getElement(const string& query) {
    std::lock_guard<mutex> lock(_mutex);
    auto it = _hash_map.find(query);
    if (it != _hash_map.end()) {
        _result_list.splice(_result_list.begin(), _result_list, it->second);
        return it->second->second;
    } else {
        return "";
    }
}

void LRUCache::showResultList() const {
    // for (auto pair : _result_list) {
    //     std::cout << "LRU->docID = " << pair.first << "\n";  
    //     std::cout << "LRU->page = " << pair.second << "\n";
    // }
    std::cout << "_result_list size = " << _result_list.size() << "\n";
    std::cout << "_pending_update_list size = " << _pending_update_list.size() << "\n";
}