#include "../include/CreateInvert.h"
#include "../include/Configuration.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
using std::vector;

int main() {
    CreateInvertIndex index;
    index.buildIndex(Configuration::getInstance().getValue("SavePath", "NEWPAGE_PATH"));
    index.Normalization();
    index.saveIndex(Configuration::getInstance().getValue("SavePath", "INVERTINDEX_PATH"));
}


CreateInvertIndex::CreateInvertIndex()
{
    readStopWords(_stop_words, Configuration::getInstance().getValue("yuliao", "stop_words_cn"));
    readStopWords(_stop_words, Configuration::getInstance().getValue("yuliao", "stop_words_en"));
}

void CreateInvertIndex::buildIndex(const string& webpagePath) {
    std::ifstream infile(webpagePath);
    string line, content;
    int totalDocs = 0;

    // 第一遍：计算每个词语的文档频率 (DF)
    while (getPage(infile, line)){
        vector<string> words;
        getWords(line, words);
        set<string> uniqueWords(words.begin(), words.end());
        for (const auto &word : uniqueWords)
        {
            _dfTable[word]++;
        }
        totalDocs++;
    }

    infile.clear();  // 清除 EOF 标志
    infile.seekg(0);

    // 第二遍：计算每个词语的 TF-IDF 并构建倒排索引
    int docid = 1;
    while (getPage(infile, line)) {
        vector<string> words;
        getWords(line, words);
        unordered_map<string, double> tfTable;

        for (const auto& word : words) {
            tfTable[word] = calculateTF(word, words);
        }

        for (const auto& pair : tfTable) {
            const string& word = pair.first;
            double tf = pair.second;
            double idf = calculateIDF(word, totalDocs);  // 在这里使用已计算的 DF 值
            double tfidf = tf * idf;
            _invertIndexTable[word].insert({docid, tfidf});
        }

        docid++;
    }
}

bool CreateInvertIndex::getPage(std::ifstream& infile, string& page) {
    page.clear();
    string line;
    bool empty = true;
    while (getline(infile, line)) {
        empty = false;
        page += line;
        if (line.find("</content>") != string::npos) {
            break;
        }
    }
    return !empty;
}

void CreateInvertIndex::getContent(string& content) {
    auto start = content.find("<content>");
    auto end = content.find("</content>");
    content = content.substr(start, end - start);
}

double CreateInvertIndex::calculateTF(const string& word, const vector<string>& words) {
    return static_cast<double>(std::count(words.begin(), words.end(), word));
}

double CreateInvertIndex::calculateIDF(const string& word, int totalDocs) {
    return std::log2(static_cast<double>(totalDocs) / (_dfTable[word] + 1) + 1);
}

void CreateInvertIndex::getWords(string& page, vector<string>& words) {
    string::size_type start = page.find("<content>") + 9;
    if (start == string::npos)    std::cerr << "start = npos\n";            // TODO rm
    string::size_type end = page.find("</content>");
    if (end == string::npos)    std::cerr << "end = npos\n";            // TODO rm
    string pageContent = page.substr(start, end - start);
    std::vector<std::string> tmp = SplitTool::getInstance()->cut(pageContent);
    for (auto &word : tmp) {
        if (!_stop_words.count(word) && word != " " && word != "\n" && word != "\r" && word != "\t") {
            words.emplace_back(word);
        }
    }
    return;
}

void CreateInvertIndex::Normalization() {
    double totalWeight = 0.0;
    for (auto& entry : _invertIndexTable) {
        for (auto& weightEntry : entry.second) {
            totalWeight += weightEntry.second * weightEntry.second;
        }
    }
    totalWeight = sqrt(totalWeight);
    unordered_map<string, unordered_set<std::pair<int, double>, PairHash>> tmp;
    for (auto& entry : _invertIndexTable) {
        for (auto& weightEntry : entry.second) {
            double weight = weightEntry.second / totalWeight;
            tmp[entry.first].insert({weightEntry.first, weight});
        }
    }
    _invertIndexTable = tmp;
}

void CreateInvertIndex::saveIndex(const string& outputPath) {
    std::ofstream ofs(outputPath);
    for (auto& entry : _invertIndexTable) {
        ofs << entry.first;
        for (auto& weightEntry : entry.second) {
            ofs  << " " << weightEntry.first << " " << weightEntry.second;
        }
        ofs << "\n";
    }
    ofs.close();
}

void CreateInvertIndex::readStopWords(set<string>& stopWords, const string& path) {
    std::ifstream stop_words_file(path);
    if (!stop_words_file) {
        std::cerr << "open file" << path << " failed\n";
        return;
    }
    string stop_line, stop_word;
    while (getline(stop_words_file, stop_line)) {
        std::istringstream iss(stop_line);
        iss >> stop_word;                           // TODO 单词末尾有空格吗？
        stopWords.insert(stop_word);
    }
    stop_words_file.close();
}
