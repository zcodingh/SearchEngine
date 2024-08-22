#ifndef __WEBPAGE_H__
#define __WEBPAGE_H__
#include <string>
using std::string;

class WebPage {
public:
    WebPage(string url, string title, string content);
    int getDocId() { return _docid; };
    string getTitle() { return _title; };
    string getURL() { return _url; };
    size_t _docid;
    string _title;
    string _url;
    string _content;
};
#endif
