#include "../include/Configuration.h"
#include "../include/DictProducer.h"
#include "../include/SplitTool.h"
using std::cout;
using std::cin;
using std::endl;

int main(int argc, char* argv[]) {
    DictProducer dic;
    dic.buildENDict();
    dic.buildCNDict();
    dic.createIndex();
    dic.store();

    return 0;
}


