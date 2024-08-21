#include "../include/SplitTool.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <set>
#include <vector>
#include <cmath>
#include <algorithm>

class InvertedIndex {
public:
    void buildIndex(const std::string& webpagePath);
    void displayIndex() const;

    std::unordered_map<std::string, std::set<std::pair<int, double>>> index;
    std::unordered_map<std::string, int> dfTable; // 存储每个词语的文档频率

    std::vector<std::string> tokenize(const std::string& content);
    double calculateTF(const std::string& word, const std::vector<std::string>& words);
    double calculateIDF(const std::string& word, int totalDocs);
};

void InvertedIndex::buildIndex(const std::string& webpagePath) {
    std::ifstream infile(webpagePath);
    std::string line;
    int docid = 1;
    int totalDocs = 0;

    // 第一遍：计算每个词语的文档频率 (DF)
    while (std::getline(infile, line)) {
        std::vector<std::string> words = tokenize(line);
        std::set<std::string> uniqueWords(words.begin(), words.end());

        for (const auto& word : uniqueWords) {
            dfTable[word]++;
        }

        totalDocs++;
    }

    if (dfTable.empty()) {
        std::cout << "dfTable empty\n";
    }

    infile.clear();  // 清除 EOF 标志
    infile.seekg(0); // 重置文件指针，开始第二遍处理

    // 第二遍：计算每个词语的 TF-IDF 并构建倒排索引
    while (std::getline(infile, line)) {
        std::vector<std::string> words = tokenize(line);
        std::unordered_map<std::string, double> tfTable;

        // 计算每个词的 TF 值
        for (const auto& word : words) {
            tfTable[word] = calculateTF(word, words);
        }

        // 构建倒排索引
        for (const auto& pair : tfTable) {
            const std::string& word = pair.first;
            double tf = pair.second;
            double idf = calculateIDF(word, totalDocs);  // 在这里使用已计算的 DF 值
            double tfidf = tf * idf;
            index[word].insert({docid, tfidf});
        }

        docid++;
    }
}

std::vector<std::string> InvertedIndex::tokenize(const std::string &content)
{

    std::vector<std::string> tmp = SplitTool::getInstance()->cut(content);
    std::vector<std::string> words;
    for (auto &word : tmp)
    {
        if (word != " " && word != "\n" && word != "\r" && word != "\t")
        {
            words.emplace_back(word);
        }
    }
    return words;
}

double InvertedIndex::calculateTF(const std::string& word, const std::vector<std::string>& words) {
    return static_cast<double>(std::count(words.begin(), words.end(), word));
}

double InvertedIndex::calculateIDF(const std::string& word, int totalDocs) {
    // 返回 word 的 IDF 值
    // 这里用 dfTable[word] 来获取该词的文档频率
    return std::log2(static_cast<double>(totalDocs) / (dfTable[word] + 1));
}

void InvertedIndex::displayIndex() const {
    // 显示倒排索引的内容
    for (const auto& pair : index) {
        const std::string& word = pair.first;
        std::cout << "Word: " << word << std::endl;
        for (const auto& doc : pair.second) {
            std::cout << "\tDocID: " << doc.first << ", TF-IDF: " << doc.second << std::endl;
        }
    }
}

int main() {
    InvertedIndex index;
    std::cout << "-------------\n";
    index.buildIndex("../data/filtered_page.dat");
    std::cout << "-------------\n";
    index.displayIndex();
    std::cout << "-------------\n";
    if (index.index.empty()) {
        std::cout << "1\n";
    }
    return 0;
}
