#ifndef __CREATEINVERT_H__
#define __CREATEINVERT_H__

#include "SplitTool.h"
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <map>
#include <string>
using std::map;
using std::set;
using std::string;
using std::unordered_map;
using std::unordered_set;

struct PairHash {
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1, T2>& p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        // Combine the two hash values using XOR and bit shifting
        return h1 ^ (h2 << 1);
    }
};

class CreateInvertIndex {
public:
    CreateInvertIndex();
    void buildIndex(const string& webpagePath);
    void Normalization();
    void saveIndex(const string& outputPath);
private:
    void getWords(const string& page, vector<string>& words);
    double calculateTF(const string& word, const vector<string>& words);
    double calculateIDF(const string& word, int totalDocs);
    void readStopWords(set<string>& stopWords, const string& path);

    unordered_map<string, unordered_set<std::pair<int, double>, PairHash>> _invertIndexTable; // 倒排索引表
    unordered_map<string, int> _dfTable; // DF表
    set<string> _stop_words;
};



// class CreateInvert {
// public:
//     CreateInvert();
//     void buildInvertIndex();
//     void calculateTFIDF();
// private:
//     void cut(const string& sentence);
//     void readStopWords(set<string>& stopWords, const string& path);
//     void computeTF();
//     void computeTFIDF();
// private:
//     unordered_map<string, set<std::pair<int, double>>> _invert_index_table;
//     int _total_docs;                   

//     unordered_map<string, set<int>> _word_docs;
//     unordered_map<string, double> _idfs;
//     unordered_map<string, set<std::pair<int, double>>> _tfs;

//     vector<string> _words;       // 分词后的单词
//     set<string> _stop_words;
// };

#endif