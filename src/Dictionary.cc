#include "../include/Dictionary.h"
#include "../include/Configuration.h"
#include <iostream>     // TODO rm
#include <fstream>
#include <sstream>
std::unique_ptr<Dictionary> Dictionary::_instance;
Dictionary& Dictionary::getInstance() {
    if (!_instance) {
        _instance.reset(new Dictionary());
    }
    return *_instance;
}

Dictionary::Dictionary() {
    std::ifstream ifsDictEN(Configuration::getInstance().getValue("SavePath", "DICT_EN_PATH"));
    std::ifstream ifsDictCN(Configuration::getInstance().getValue("SavePath", "DICT_CN_PATH"));
    std::ifstream ifsIndex(Configuration::getInstance().getValue("SavePath", "DICT_INDEX_PATH"));
    if (!ifsDictCN || !ifsDictEN || !ifsIndex) {
        std::cerr << "open dictionary file failed !!!\n";
        return;
    }
    string line;
    while (getline(ifsDictEN, line)) {
        string key;
        int val;
        std::istringstream iss(line);
        iss >> key >> val;
        _dict_en.push_back({key, val});
    }
    while (getline(ifsDictCN, line)) {
        string key;
        int val;
        std::istringstream iss(line);
        iss >> key >> val;
        _dict_cn.push_back({key, val});
    }
    while (getline(ifsIndex, line)) {
        string key;
        int val;
        std::istringstream iss(line);
        iss >> key;
        while (iss >> val) {
            _index[key].insert(val);
        }
    }  
}
