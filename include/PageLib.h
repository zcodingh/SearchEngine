#include "WebPage.h"
#include "FileProcessor.h"
#include <vector>
#include <unordered_map>
using std::vector;
using std::unordered_map;
using std::pair;

class PageLib {     // 解析xml文件得到网页库和偏移库
public:
    PageLib(FileProcessor& processer);
    void store();
private:
    FileProcessor& _processor;
    unordered_map<int, pair<int, int>> _offsetLib;
    vector<string> _files;      // 语料路径
};