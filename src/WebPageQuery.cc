#include "../include/WebPageQuery.h"
#include "../include/Configuration.h"
#include "../include/SplitTool.h"
#include "../include/nlohmann/json.hpp"
#include "../include/TcpConnection.h"
#include "../include/CacheManager.h"
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

WebPageQuery::WebPageQuery()
: _max_heap(comp)
{
    loadInvertIndexTable();
    loadOffsetLib();
    loadStopWords(Configuration::getInstance().getValue("yuliao", "stop_words_cn"));
    loadStopWords(Configuration::getInstance().getValue("yuliao", "stop_words_en"));
}

void WebPageQuery::doQuery(const string& query, TcpConnectionPtr con) {
    string cacheMSG;
    LRUCache* threadCache = CacheManager::getInstance().getThreaadCache(pthread_self());
    std::cout << "thread id = " << pthread_self() << "\n";      // DEBUG
    if ((cacheMSG = threadCache->getElement(query)) != "") {
        con->sendInLoop(cacheMSG);
        std::cout << "cache hit\n";         // DEBUG
        return;
    }
    std::cout << "cache not hit\n";         // DEBUG

    auto queryWeights = calculateWeight(_query_words);
    unordered_map<int, unordered_map<string, double>> docWeights;
 
    set<int> commonDocs;
    bool first = true;
    // 获取各个词的交集
    for (auto& query : queryWeights) {          // query = [queryWord : queryWeight]
        auto it = _invert_index_table.find(query.first);
        if (it == _invert_index_table.end()) {
            std::cerr << "_inver_index_table find fail\n";
            continue;
        }

        set<int> termDocs;
        for (auto& pair : it->second) {         // pair = [docID : docWordWeight]
            termDocs.insert(pair.first);
            docWeights[pair.first][query.first] = pair.second;
        }
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
    for (int docId : commonDocs) {
        double similarity = cosSimilarity(docWeights[docId], queryWeights);
        _max_heap.emplace(docId, similarity);
    }
    sendMessage(con, query);
}


void WebPageQuery::sendMessage(TcpConnectionPtr con, const string& query) {
    json msg = json::array();
    int count = 5;      //默认发出前5个网页
    std::ifstream ifs(Configuration::getInstance().getValue("SavePath", "NEWPAGE_PATH"));
    if (!ifs) {
        std::cerr << "open page failed\n";
    }
    while (!_max_heap.empty() && count > 0) {
        WebPage page;
        if (!readWebPage(ifs, _max_heap.top().first, page)) {
            _max_heap.pop();
            continue;
        }
        json doc = {
            {"title", page._title},
            {"url", page._url},
            {"content", page._content}};
        msg.push_back(doc);
        _max_heap.pop();
        count--;   
    }
    ifs.close();
    LRUCache* threadCache = CacheManager::getInstance().getThreaadCache(pthread_self());
    threadCache->addElement(query, msg.dump());
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

bool WebPageQuery::readWebPage(std::ifstream& ifs, int docID, WebPage& result) {
    int pos = _offset_lib[docID - 1].first;
    int len = _offset_lib[docID - 1].second;
    ifs.clear();
    ifs.seekg(pos);
    vector<char> buf(len + 1, 0);
    ifs.read(buf.data(), len);
    string page(buf.begin(), buf.begin() + len);

    return filterMessage(page, docID, result);
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

bool WebPageQuery::filterMessage(string& page, int docID, WebPage& result) {
    string pageContent, pageTitle, pageURL;
    if (!parsePage(page, "content", pageContent) || !parsePage(page, "title", pageTitle) || !parsePage(page, "url", pageURL)) {
        return false;
    }

    size_t len = 0;
    size_t idx;
    for(idx = 0; idx < pageContent.size() && len < 100; ++idx) {        // 取前100个字符
        int nBytes = getBytes(pageContent[idx]);
        if (idx + nBytes >= pageContent.size()) break;
        idx += (nBytes - 1);
        ++len;
    }
    pageContent = pageContent.substr(0, idx);


    // Redis::getInstance().hset(docID, "url", pageURL);
    // Redis::getInstance().hset(docID, "title", pageTitle);
    // Redis::getInstance().hset(docID, "content", pageContent);
    result._title = pageTitle;
    result._url = pageURL;
    result._content = pageContent;
    return true;
}


bool WebPageQuery::parsePage(const string& page, const string& label, string& result) {
    string::size_type start = page.find("<" + label + ">") + label.size() + 2;
    string::size_type end = page.find("</" + label + ">");
    if (start != string::npos && end != string::npos && start < end) {  
        result = page.substr(start, end - start); 
        return true;
    }
    result = "";
    return false;
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
