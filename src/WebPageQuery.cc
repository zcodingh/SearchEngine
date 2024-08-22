#include "../include/WebPageQuery.h"
#include "../include/Configuration.h"
#include "../include/SplitTool.h"
#include "../include/nlohmann/json.hpp"
#include "../include/TcpConnection.h"
#include <iostream>
#include <fstream>
#include <sstream>

using nlohmann::json;

unique_ptr<WebPageQuery> WebPageQuery::_instance;
WebPageQuery& WebPageQuery::getInstance() {
    if (!_instance) {
        _instance.reset(new WebPageQuery());
    }
    return *_instance;
}

WebPageQuery::WebPageQuery() {
    loadInvertIndexTable();
    loadOffsetLib();
    loadStopWords(Configuration::getInstance().getValue("yuliao", "stop_words_cn"));
    loadStopWords(Configuration::getInstance().getValue("yuliao", "stop_words_en"));
}

void WebPageQuery::doQuery(TcpConnectionPtr con) {
    auto queryWeights = calculateWeight(_query_words);
    unordered_map<int, unordered_map<string, double>> docWeights;

    set<int> commonDocs;
    bool first = true;
    for (auto& query : queryWeights) {          // query = [queryWord : queryWeight]
        auto it = _invert_index_table.find(query.first);
        if (it == _invert_index_table.end()) {
            std::cerr << "_inver_index_table find fail\n";
            continue;
        }
        std::cout << "it->first = " << it->first << "\n";       // TODO rm

        set<int> termDocs;
        for (auto& pair : it->second) {         // pair = [docID : docWordWeight]
            termDocs.insert(pair.first);
            docWeights[pair.first][query.first] = pair.second;
        }
        for (int ele : termDocs) {          // TODO rm
            std::cout << ele << " ";
        }
        std::cout << "\n";
        if (first) {
            commonDocs = std::move(termDocs);
            first = false;
        } else {
            set<int> tmp;
            std::set_intersection(commonDocs.begin(), commonDocs.end(),
                                  termDocs.begin(), termDocs.end(),
                                  std::inserter(tmp, tmp.begin()));
            commonDocs = std::move(tmp);
        }
    }
    if (commonDocs.empty()) {
        std::cerr << "didn't find the file\n";
    }

    auto comp = [](const std::pair<int, double>& a, const std::pair<int, double>& b) {
        return a.second > b.second; 
    };
    std::priority_queue<std::pair<int, double>, std::vector<std::pair<int, double>>, decltype(comp)> maxHeap(comp);
    for (int docId : commonDocs) {
        double similarity = cosSimilarity(docWeights[docId], queryWeights);
        maxHeap.emplace(docId, similarity);
    }
    json msg = json::array();
    int count = 5;      //默认发出前5个网页
    std::ifstream ifs(Configuration::getInstance().getValue("SavePath", "NEWPAGE_PATH"));
    if (!ifs) {
        std::cerr << "open page failed\n";
    }
    std::cout << "maxHeap size = " << maxHeap.size() << "\n";           // TODO rm
    while (!maxHeap.empty() && count > 0) {
        WebPage page = readWebPage(ifs, maxHeap.top().first);
        json doc = {
            {"title", page._title},
            {"url", page._url},
            {"content", page._content}};
        msg.push_back(doc);
        maxHeap.pop();
        count--;   
    }
    ifs.close();
    std::cout << "\nmsg = " << msg.dump() << "\n";        // TODO rm
    std::cout << "msgsize = " << msg.dump().size() << "\n";
    con->sendInLoop(msg.dump());
}

//1. 求取一个字符占据的字节数
size_t getBytes(const char ch) {
    if (ch & (1 << 7)) {
        int nBytes = 1;
        for (int idx = 0; idx != 6; ++idx) {
            if (ch & (1 << (6 - idx))) {
                ++nBytes;
            }
            else
                break;
        }
        return nBytes;
    }
    return 1;
}

double WebPageQuery::cosSimilarity(unordered_map<string, double>& queryWeights, unordered_map<string, double>& docWeights) {
    // 点积、范数
    double dotProduct = 0.0, docNorm = 0.0, queryNorm = 0.0;
    for (auto& pair : queryWeights) {
        auto it = docWeights.find(pair.first);
        if (it != docWeights.end()) {
            dotProduct += it->second * pair.second;
        }
    }

    for (auto& pair : docWeights) {
        docNorm += pair.second * pair.second;
    }
    for (auto& pair : queryWeights) {
        queryNorm += pair.second * pair.second;
    }
    if (docNorm == 0 || queryNorm == 0) return 0.0;
    return dotProduct / (std::sqrt(docNorm) * std::sqrt(queryNorm));
}

WebPage WebPageQuery::readWebPage(std::ifstream& ifs, int docID) {
    std::cout << "readWebPage docID = " << docID << "\n";
    int pos = _offset_lib[docID - 1].first;
    int len = _offset_lib[docID - 1].second;
    ifs.clear();
    ifs.seekg(pos);
    vector<char> buf(len + 1, 0);
    ifs.read(buf.data(), len);
    string page(buf.begin(), buf.begin() + len);

    return filterMessage(page);
}

WebPage WebPageQuery::filterMessage(string& page) {
    string::size_type start = page.find("<content>") + 9;
    string::size_type end = page.find("</content>");
    string pageContent = page.substr(start, end - start);
    start = page.find("<title>") + 7;
    end = page.find("/title");
    string pageTitle = page.substr(start, end - start);
    start = page.find("<url>") + 5;
    end = page.find("/url");
    string pageURL = page.substr(start, end - start);
    size_t len = 0;
    size_t idx;
    std::cout << "pageContent size = " << pageContent.size() << "\n";   // TODO rm
    for(idx = 0; idx < pageContent.size() && len < 100; ++idx) {        // 取前100个字符
        int nBytes = getBytes(pageContent[idx]);
        if (idx + nBytes >= pageContent.size()) break;
        idx += (nBytes - 1);
        ++len;
    }
    pageContent = pageContent.substr(0, idx);
    return {pageURL, pageTitle, pageContent};
}

unordered_map<string, double> WebPageQuery::calculateWeight(const vector<string>& queryWords) {
    unordered_map<string, double> queryWeights;
    unordered_map<string, int> TF;
    for (auto& word : queryWords) {
        ++TF[word];
    }

    for (auto& pair : TF) {
        double tf = static_cast<double>(pair.second);
        double idf = std::log2(1 / (tf + 1) + 1);
        queryWeights[pair.first] = tf * idf;
    }
    return queryWeights;
}

void WebPageQuery::cut(const string& query) {
    _query_words.clear();
    vector<string> tmp = SplitTool::getInstance()->cut(query);
    for (auto& word : tmp) {
        if (!_stop_words.count(word) && word != " " && word != "\n" && word != "\r" && word != "\t") {
            _query_words.emplace_back(word);
            std::cout << "cut word = " << word << "\n";             // TODO rm
        }
    }
}

void WebPageQuery::loadOffsetLib() {
    std::ifstream ifsOffset(Configuration::getInstance().getValue("SavePath", "NEWOFFSET_PATH"));
    string line;
    while (getline(ifsOffset, line)) {
        int len, pos;
        std::istringstream iss(line);
        iss  >> pos >> len;
        _offset_lib.emplace_back(len, pos);
    }
}
void WebPageQuery::loadInvertIndexTable() {
    std::ifstream ifsInvert(Configuration::getInstance().getValue("SavePath", "INVERTINDEX_PATH"));
    string line;
    while (getline(ifsInvert, line)) {
        string word;
        std::pair<int, double> docInfo;
        set<std::pair<int, double>> doc_weight;
        std::istringstream iss(line);
        iss >> word;
        while (iss >> docInfo.first >> docInfo.second) {
            doc_weight.emplace(docInfo);
        }
        _invert_index_table.emplace(word, doc_weight);
    }
}
void WebPageQuery::loadStopWords(const string& path) {
    std::ifstream stop_words_file(path);
    if (!stop_words_file) {
        std::cerr << "open file" << path << " failed\n";
        return;
    }
    string stop_line, stop_word;
    while (getline(stop_words_file, stop_line)) {
        std::istringstream iss(stop_line);
        iss >> stop_word;                           // TODO 单词末尾有空格吗？
        _stop_words.emplace(stop_word);
    }
    stop_words_file.close();
}