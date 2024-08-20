#include "../include/Configuration.h"
#include <iostream>
#include <fstream>
#include <sstream>
using std::ifstream;
using std::istringstream;

std::unique_ptr<Configuration> Configuration::_instance;
Configuration& Configuration::getInstance() {
    if (!_instance) {
        _instance.reset(new Configuration("../conf/myconf.conf"));
    }
    return *_instance;
}

Configuration::Configuration(const string& filePath) {
    parseConfigFile(filePath);
    // To Debug
    for (auto sec : _conf) {
        std::cout << sec.first << "\n";
        for (auto pair : sec.second) {
            std::cout << "\t" << pair.first << "\t" << pair.second << "\n";
        }
    }
}

void Configuration::parseConfigFile(const string& filePath) {
    ifstream file(filePath);
    if (!file) {
        std::cerr << "Failed to open configuration file: " << filePath << "\n";
        return;
    }

    std::string line, currentSection;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == ';' || line[0] == '#') continue;

        if (line[0] == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.size() - 2);
        } else {
            std::istringstream iss(line);
            std::string key, value;
            if (std::getline(iss, key, '=') && std::getline(iss, value)) {
                _conf[currentSection][key] = value;
            }
        }
    }
}

string Configuration::getValue(const string& section, const string& key) const {
    auto secIt = _conf.find(section);
    if (secIt != _conf.end()) {
        auto keyIt = secIt->second.find(key);
        if (keyIt != secIt->second.end()) {
            std::cout << keyIt->second << "\n";  //TODO remove
            return keyIt->second;
        }
    }
    return "";
}
