#include "../include/DictProducer.h"
#include <sys/types.h>
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <sstream>

using std::cerr;
using std::ifstream;
using std::ofstream;
using std::istringstream;

DictProducer::DictProducer(Configuration& conf)
: _conf(conf)
{
    string dir_yuliao_en = _conf.getValue("yuliao", "dir_yuliao_en");
    DIR* dstream = opendir(dir_yuliao_en.c_str());
    if (dstream == nullptr) {
        std::cerr << "Failed to open dir\n";
    }
    struct dirent* entry;
    while ((entry = readdir(dstream)) != nullptr) {
        string fileName = entry->d_name;
        if (fileName == "." || fileName == "..") {
            continue;
        }

        string filepath = dir_yuliao_en + "/" + fileName;
        _files_en.push_back(filepath);
    }
    closedir(dstream);
}

void DictProducer::updateFrequency(const string& word, map<string, int>& freq, set<string>& stop_words) {
    if (!word.empty() && !stop_words.count(word)) {
        ++freq[word];
    }
}

void DictProducer::cleanUpdate(const string& word, map<string, int>& freq, set<string>& stop_words) {
    string result;
    for (char c : word) {
        if (isalpha(c)) {
            result.append(1, tolower(c));
        } else {
            updateFrequency(result, freq, stop_words);
            result.clear();
        }
    }
    updateFrequency(result, freq, stop_words);
}

void DictProducer::buildEnDict() {
    map<string, int> freq;
    set<string> stop_words;

    ifstream stop_words_file(_conf.getValue("yuliao", "stop_words_en"));
    if (!stop_words_file) {
        cerr << "open file" << _conf.getValue("yuliao", "stop_words_en") << "failed\n";
    }
    string stop_line, stop_word;
    while (getline(stop_words_file, stop_line)) {
        istringstream iss(stop_line);
        iss >> stop_word;
        stop_words.insert(stop_word);
    }
    stop_words_file.close();
    
    for (auto fileName : _files_en) {
        ifstream file(fileName);
        string line, word;
        if (!file) {
            std::cerr << "open file" << fileName << "failed\n";
        }

        while (getline(file, line)) {
            istringstream iss(line);
            while (iss >> word) {
                cleanUpdate(word, freq, stop_words);
            }
        }
        file.close();
    }
    for (auto kv : freq) {
        _dict_en.push_back({kv.first, kv.second});
    }
}

void DictProducer::createIndex() {
    int size = _dict_en.size();
    for (int i = 0; i < size; ++i) {
        for (char c : _dict_en[i].first) {
            _index[string(1, c)].insert(i);
        }
    }
}

void DictProducer::store() {
    ofstream ofs("../data/dict_en.dat");
    if (!ofs) {
        cerr << "failed to create/open dict_en.dat\n"; 
    }
    for (auto kv : _dict_en) {
        ofs << kv.first << " " << kv.second << "\n";
    }
    ofs.close();
}
