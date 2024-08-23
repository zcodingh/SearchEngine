#include "../include/PageLib.h"
#include "../include/Configuration.h"
#include <dirent.h>
#include <iostream>
#include <fstream>

PageLib::PageLib(FileProcessor& processor)
:_processor(processor)
{
    string dir = Configuration::getInstance().getValue("yuliao", "dir_webpages");
    DIR* dstream = opendir(dir.c_str());
    if (dstream == nullptr) {
        std::cerr << "Failed to open dir\n";
    }
    struct dirent* entry;
    while ((entry = readdir(dstream)) != nullptr) {
        string fileName = entry->d_name;
        if (fileName == "." || fileName == "..") {
            continue;
        }

        string filepath = dir + "/" + fileName;
        _files.push_back(filepath);
    }
    closedir(dstream); 
}

void PageLib::store() {
    string WebPagePath = Configuration::getInstance().getValue("SavePath", "WEBPAGE_PATH");
    for (auto file : _files) {
        _processor.parseRss(file);
        _processor.dump(WebPagePath, _offsetLib);
    }
    std::ofstream ofs(Configuration::getInstance().getValue("SavePath", "OFFSET_PATH"));
    for (auto offset : _offsetLib) {
        ofs << offset.first << " " << offset.second.first << " " << offset.second.second << "\n";
    }
}
