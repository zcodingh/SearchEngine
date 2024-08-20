#ifndef __PAGELIBPREPROCESSOR_H__
#define __PAGELIBPREPROCESSOR_H__

#include "simhash/Simhasher.hpp"
#include <cstdint>
#include <unordered_map>
#include <vector>
using namespace simhash;
using std::uint64_t;
using std::vector;
using std::pair;
using std::unordered_map;

class PageLibPreprocessor {
public:
    PageLibPreprocessor();
    void getFiltered();
    void cutRedundantPage();
private:
    bool checkSimhash(uint64_t fp);
private:
    Simhasher _simhasher;
    unordered_map<int, pair<int, int>> _offsetLib;
    unordered_map<int, pair<int, int>> _filted_offsetLib;
    vector<uint64_t> _simhash;
};

#endif