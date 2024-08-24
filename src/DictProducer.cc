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

// DictProducer::DictProducer(Configuration& conf)
// : _conf(conf)
// {
//     string dir_yuliao_en = _conf.getValue("yuliao", "dir_yuliao_en");
//     DIR* dstream = opendir(dir_yuliao_en.c_str());
//     if (dstream == nullptr) {
//         std::cerr << "Failed to open dir\n";
//     }
//     struct dirent* entry;
//     while ((entry = readdir(dstream)) != nullptr) {
//         string fileName = entry->d_name;
//         if (fileName == "." || fileName == "..") {
//             continue;
//         }

//         string filepath = dir_yuliao_en + "/" + fileName;
//         _files_en.push_back(filepath);
//     }
//     closedir(dstream);
// }

DictProducer::DictProducer() 
{
    string dir_yuliao_en = Configuration::getInstance().getValue("yuliao", "dir_yuliao_en");
    string dir_yuliao_cn = Configuration::getInstance().getValue("yuliao", "dir_yuliao_cn");
    DIR* dstream = opendir(dir_yuliao_en.c_str());
    readDir(dir_yuliao_en, _files_en);  
    readDir(dir_yuliao_cn, _files_cn);  
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

void DictProducer::readDir(const string& dir, vector<string>& files) {
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
        files.push_back(filepath);
    }
    closedir(dstream);      
}

void DictProducer::buildENDict() {
    map<string, int> freq;
    set<string> stop_words;

    ifstream stop_words_file(Configuration::getInstance().getValue("yuliao", "stop_words_en"));
    if (!stop_words_file) {
        cerr << "open file" << Configuration::getInstance().getValue("yuliao", "stop_words_en") << " failed\n";
        return;
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
            std::cerr << "open file" << fileName << " failed\n";
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

void DictProducer::buildCNDict() {
    map<string, int> freq;
    set<string> stop_words;

    ifstream stop_words_file(Configuration::getInstance().getValue("yuliao", "stop_words_cn"));
    if (!stop_words_file) {
        cerr << "open file" << Configuration::getInstance().getValue("yuliao", "stop_words_cn") << " failed\n";
        return;
    }
    string stop_line, stop_word;
    while (getline(stop_words_file, stop_line)) {
        istringstream iss(stop_line);
        iss >> stop_word;
        stop_words.insert(stop_word);
    }
    stop_words_file.close();

    for (auto fileName : _files_cn) {
        ifstream file(fileName);
        if (!file) {
            std::cerr << "open file" << fileName << " failed\n";
        }

        string line;
        while (getline(file, line)) {
            vector<string> words;
            words = SplitTool::getInstance()->cut(line);
            for (auto word : words) {
                if (!stop_words.count(word) && word != " " && word != "\r" && word != "\t") {
                    ++freq[word];
                }
            }
        }
        file.close();
    }
    for (auto kv : freq) {
        _dict_cn.emplace_back(kv.first, kv.second);
    }
}

void DictProducer::createIndex() {
    int size_en = _dict_en.size();
    for (int i = 0; i < size_en; ++i) {
        for (char c : _dict_en[i].first) {
            _index[string(1, c)].insert(i); 
        }
    }

    int size_cn = _dict_cn.size();
    for (int i = 0; i < size_cn; ++i) {
        string word = _dict_cn[i].first;
        for (size_t j = 0; j < word.size();) {
            unsigned char c = word[j];
            size_t len = 1;
            if (c >= 0xF0) {
                len = 4;  
            } else if (c >= 0xE0) {
                len = 3;  
            } else if (c >= 0xC0) {
                len = 2; 
            } 
            if (len >= 3) {
                string utf8_char = word.substr(j, len);
                _index[utf8_char].insert(i);
            }
            j += len;
        }
    }
}

void DictProducer::store() {
    ofstream ofsEN(Configuration::getInstance().getValue("SavePath", "DICT_EN_PATH"));
    ofstream ofsCN(Configuration::getInstance().getValue("SavePath", "DICT_CN_PATH"));
    ofstream ofsIndex(Configuration::getInstance().getValue("SavePath", "DICT_INDEX_PATH"));

    if (!ofsEN || !ofsCN || !ofsIndex) {
        cerr << "failed to create/open dict/index\n"; 
    }
    for (auto kv : _dict_en) {
        ofsEN << kv.first << " " << kv.second << "\n";
    }
    for (auto kv : _dict_cn) {
        ofsCN << kv.first << " " << kv.second << "\n";
    }
    for (auto kv : _index) {
        ofsIndex << kv.first << " ";
        for (int val : kv.second) {
            ofsIndex << val << " ";
        }
        ofsIndex << "\n";
    }
    
    ofsEN.close();
    ofsCN.close();
    ofsIndex.close();
}
