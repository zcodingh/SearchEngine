#include <iostream>
#include "../include/Configuration.h"
#include "../include/DictProducer.h"
#include "../include/SplitTool.h"
using std::cout;
using std::cin;
using std::endl;

int main(int argc, char* argv[]) {
    Configuration conf("../conf/myconf.conf");
    SplitTool* split = new SplitToolCppJieba(conf);
    DictProducer dic(conf, split);
    dic.buildENDict();
    dic.buildCNDict();
    dic.store();

    return 0;
}

