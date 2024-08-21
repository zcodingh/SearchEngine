#ifndef __FILEPROCESSOR_H__
#define __FILEPROCESSOR_H__

#include "WebPage.h"
#include <string>
#include <vector>
#include <unordered_map>
using std::unordered_map;
using std::pair;
using std::string;
using std::vector;

class FileProcessor
{
public:
    FileProcessor();
    void parseRss(string file);
    void dump(const string & filename, unordered_map<int, pair<int, int>>& offsetLib);
    size_t _docid;
private:
    vector<WebPage> _rss;   //解析完一个文件后存储在此
    string removeHTMLTags(const string & input);
};

#endif