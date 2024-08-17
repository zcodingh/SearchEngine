#include "../include/SplitTool.h"

SplitToolCppJieba::SplitToolCppJieba(Configuration& conf)
: _conf(conf)
, _jieba(
    _conf.getValue("CppJieba", "DICT_PATH"),
    _conf.getValue("CppJieba", "HMM_PATH"),
    _conf.getValue("CppJieba", "USER_DICT_PATH"),
    _conf.getValue("CppJieba", "IDF_PATH"),
    _conf.getValue("CppJieba", "STOP_WORD_PATH")
)
{}

vector<string> SplitToolCppJieba::cut(const string &sentence) {
    vector<string> words;
    _jieba.Cut(sentence, words, true);
    return words;
}