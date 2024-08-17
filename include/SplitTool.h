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
};

class SplitToolCppJieba
: public SplitTool
{
public:
    SplitToolCppJieba(Configuration& conf);
    virtual ~SplitToolCppJieba() {}
    
    virtual vector<string> cut(const string &sentence) override;
private:
    Configuration& _conf;
    cppjieba::Jieba _jieba;
};

#endif
