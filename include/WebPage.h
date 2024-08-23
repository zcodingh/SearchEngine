#ifndef __WEBPAGE_H__
#define __WEBPAGE_H__
#include <string>
using std::string;

class WebPage {
public:
    WebPage(string url, string title, string content);
    WebPage();
    size_t _docid;
    string _title;
    string _url;
    string _content;
};
#endif
