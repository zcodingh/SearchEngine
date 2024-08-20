#include "../include/SplitTool.h"

std::unique_ptr<SplitTool> SplitTool::_instance = std::unique_ptr<SplitToolCppJieba>(new SplitToolCppJieba());
SplitTool* SplitTool::getInstance() {
    return _instance.get();
}

SplitToolCppJieba::SplitToolCppJieba()
:  _jieba(
    Configuration::getInstance().getValue("CppJieba", "DICT_PATH"),
    Configuration::getInstance().getValue("CppJieba", "HMM_PATH"),
    Configuration::getInstance().getValue("CppJieba", "USER_DICT_PATH"),
    Configuration::getInstance().getValue("CppJieba", "IDF_PATH"),
    Configuration::getInstance().getValue("CppJieba", "STOP_WORD_PATH")
)
{}

vector<string> SplitToolCppJieba::cut(const string &sentence) {
    vector<string> words;
    _jieba.Cut(sentence, words, true);
    return words;
}