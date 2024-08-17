#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <string>
#include <map>
using std::string;
using std::map;

class Configuration
{
public:
    Configuration(const string& confPath);
    string getValue(const string& section, const string& key) const;
    ~Configuration() {}

private:
    void parseConfigFile(const string& filePath);
    map<string, map<string, string>> _conf;
};

#endif