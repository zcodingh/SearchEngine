#include "../include/FileProcessor.h"
#include <tinyxml2.h>
#include <regex>
#include <iostream>
#include <fstream>

using namespace tinyxml2;
using std::cerr;
using std::ofstream;

FileProcessor::FileProcessor()
: _docid(1)
{}

void FileProcessor::parseRss(string file) {
    XMLDocument doc; // 创建XMLDocument对象，用于加载和解析XML文件
    if (doc.LoadFile(file.c_str()) != XML_SUCCESS) {
        cerr << "Failed to load file " << file << "\n";
        return;
    }

    // 获取XML的根元素，如果不存在则输出错误信息并返回
    XMLElement* root = doc.RootElement();
    if (!root) {
        cerr << "Invalid XML format in " << file << "\n";
        return;
    }

    for (XMLElement *item = root->FirstChildElement("channel")->FirstChildElement("item"); item; item = item->NextSiblingElement("item")){
        const char *title = item->FirstChildElement("title") ? item->FirstChildElement("title")->GetText() : "";
        const char *url = item->FirstChildElement("link") ? item->FirstChildElement("link")->GetText() : "";
        const char *content = item->FirstChildElement("description") ? item->FirstChildElement("description")->GetText() : "";

        WebPage page(
            title ? title : "",
            url ? url : "",
            content ? removeHTMLTags(content) : "");
        _rss.push_back(page);
    }
}

string FileProcessor::removeHTMLTags(const string & input) {
    std::regex htmlTag("<[^>]*>"); 
    // 使用regex_replace函数将HTML标签替换为空字符串
    return regex_replace(input, htmlTag, "");
}

void FileProcessor::dump(const string & filename, unordered_map<int, pair<int, int>>& offsetLib){
    ofstream outFile(filename, std::ios::app); 
    if (!outFile) {
        cerr << "Failed to open output file " << filename << "\n";
        return;
    }
    // 遍历_rss向量，写入每个WebPage的内容
    for (const auto & item : _rss) {
        int pos = outFile.tellp();
        string doc = "<doc><docid>" + std::to_string(_docid) + 
                     "</docid><url>" + item._url + 
                     "</url><title>" + item._title +
                     "</title><content>" + item._content +
                     "</content></doc>";
        int len = doc.size();
        outFile << doc << "\n";
        offsetLib.insert({_docid, {pos, len}});
        _docid++;
    }
    outFile.close(); 
}
