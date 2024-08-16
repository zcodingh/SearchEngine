#include "../include/DictProducer.h"
#include <sys/types.h>
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <sstream>

using std::cerr;
using std::ifstream;
using std::istringstream;

DictProducer::DictProducer(const string& dir) {
    DIR* dstream = opendir(dir.c_str());
    if (dstream == nullptr) {
        std::cerr << "Failed to open dir\n";
    }
    struct dirent* entry;
    while ((entry = readdir(dstream)) != nullptr) {
        string fileName = entry->d_name;
        if (fileName == "." || fileName == "..") {
            continue;
        }

        string filepath = dir + "/" + fileName;
        _files.push_back(filepath);
    }
}

void DictProducer::cleanUpdateFrequency(const string& word, map<string, int>& freq) {
    string result;
    for (char c : word) {
        if (isalpha(c)) {
            result.append(1, tolower(c));
        }
    }
    if (!result.empty()) {
        ++freq[result];
    }
}

void DictProducer::buildEnDict() {
    map<string, int> freq;
    for (auto fileName : _files) {
        ifstream file(fileName);
        string line, word;
        if (!file) {
            std::cerr << "open file" << fileName << "failed\n";
        }

        while (getline(file, line)) {
            istringstream iss(line);
            while (iss >> word) {
                cleanUpdateFrequency(word, freq);
            }
        }
        file.close();
    }
    for (auto kv : freq) {
        _dict.push_back({kv.first, kv.second});
    }
}

void DictProducer::createIndex() {
    int size = _dict.size();
    for (int i = 0; i < size; ++i) {
        for (char c : _dict[i].first) {
            _index[string(1, c)].insert(i);
        }
    }
}
