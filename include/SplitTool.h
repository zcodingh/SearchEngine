#ifndef __SPLITTOOL_H__
#define __SPLITTOOL_H__

#include "Configuration.h"
#include "cppjieba/Jieba.hpp"
#include <vector>
#include <string>
using std::string;
using std::vector;

class SplitTool
{
public:
    SplitTool() {}
    virtual ~SplitTool() {}
    virtual vector<string> cut(const string& sentence) = 0;
    static SplitTool* getInstance();
private:
    static void createInstance();
    static std::unique_ptr<SplitTool> _instance;
};

class SplitToolCppJieba
: public SplitTool
{
public:
    virtual ~SplitToolCppJieba() {}
    static SplitToolCppJieba& getInstance();
    
    virtual vector<string> cut(const string &sentence) override;
private:
    SplitToolCppJieba();
    cppjieba::Jieba _jieba;

    friend class SplitTool;
};

#endif
