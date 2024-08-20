#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <string>
#include <map>
#include <memory>
using std::string;
using std::map;

class Configuration
{
public:
    ~Configuration() {}
    static Configuration& getInstance();
    Configuration(const Configuration&) = delete;
    Configuration& operator=(const Configuration&) = delete;
    string getValue(const string& section, const string& key) const;

private:
    Configuration(const string& confPath);
    void parseConfigFile(const string& filePath);
    map<string, map<string, string>> _conf;
    static std::unique_ptr<Configuration> _instance;
};

#endif
