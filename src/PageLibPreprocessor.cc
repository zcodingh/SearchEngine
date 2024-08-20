#include "../include/PageLibPreprocessor.h"
#include "../include/PageLib.h"
#include "../include/Configuration.h"
#include "../include/FileProcessor.h"
#include <tinyxml2.h>
#include <string>
#include <fstream>
#include <sstream>
using std::string;
using std::stoi;
using namespace tinyxml2;

PageLibPreprocessor::PageLibPreprocessor() 
: _simhasher("../include/simhash/dict/jieba.dict.utf8", "../include/simhash/dict/hmm_model.utf8",
             "../include/simhash/dict/idf.utf8", "../include/simhash/dict/stop_words.utf8")
{
    string line, id, pos, len;
    std::ifstream ifs(Configuration::getInstance().getValue("SavePath", "OFFSET_PATH"));
    while (getline(ifs, line)){
        std::istringstream iss(line);
        iss >> id >> pos >> len;
        _offsetLib.insert({stoi(id), {stoi(pos), stoi(len)}});
    }
    ifs.close();
}

// 创建对象时将旧偏移库读入。读网页文件时，每读一个，与所有已添加的网页指纹进行比较，若不等，加入指纹，并将这个网页写入去重后的文件
void PageLibPreprocessor::getFiltered() {
    std::ifstream ifs(Configuration::getInstance().getValue("SavePath", "WEBPAGE_PATH"));
    std::ofstream ofsPAGE(Configuration::getInstance().getValue("SavePath", "NEWPAGE_PATH"));
    std::ofstream ofsOS(Configuration::getInstance().getValue("SavePath", "NEWOFFSET_PATH"));
    cout << "entered-----\n";                                                               // TODO rm
    int docid = 1;
    for (auto pair : _offsetLib) {
        cout << pair.first << "\n";                 // TODO rm
        ifs.seekg(pair.second.first);
        char buf[pair.second.second + 1];
        ifs.read(buf, pair.second.second);
        string sbuf(buf);
        string::size_type start = sbuf.find("<content>") + 9;
        string::size_type end = sbuf.find("</content>");
        string pageContent = sbuf.substr(start, end - start);
        uint64_t fp;
        _simhasher.make(pageContent, 5, fp);    // topN = 5
        if (checkSimhash(fp)) {
            cout << "entered\n";                    // TODO rm
            start = sbuf.find("<title>") + 7;
            end = sbuf.find("</title>");
            string pageTitle = sbuf.substr(start, end - start);
            start = sbuf.find("<url>") + 5;
            end = sbuf.find("</url>");
            string pageURL = sbuf.substr(start, end - start);
            string doc = "<doc><docid>" + std::to_string(docid) + 
                     "</docid><url>" + pageURL + 
                     "</url><title>" + pageTitle +
                     "</title><content>" + pageContent +
                     "</content></doc>";
            ofsOS << docid << " " << ofsPAGE.tellp() << " " << doc.size() << "\n";
            ofsPAGE << doc << "\n";
            ++docid;
        }
    }
    ifs.close();
    ofsOS.close();
    ofsPAGE.close();
    cout << "exit-----\n";                                                               // TODO rm
}


bool PageLibPreprocessor::checkSimhash(uint64_t fp) {
    for (auto hash : _simhash) {
        if (Simhasher::isEqual(fp, hash)) {
            return false;
        }
    }
    _simhash.push_back(fp);
    return true;
}
int main() {
    FileProcessor processor;
    PageLib lib(processor);
    lib.store();
    PageLibPreprocessor deduplicator;
    deduplicator.getFiltered();
}