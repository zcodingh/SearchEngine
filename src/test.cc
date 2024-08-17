#include <iostream>
#include "../include/Configuration.h"
#include "../include/DictProducer.h"
#include "../include/SplitTool.h"
using std::cout;
using std::cin;
using std::endl;

int main(int argc, char* argv[]) {
    Configuration conf("../conf/myconf.conf");
    // DictProducer dic(conf);
    // dic.buildEnDict();
    // dic.store();

    SplitTool* split = new SplitToolCppJieba(conf);
    
    string sentence = "文学、艺术理论的建设和发展，应该密切关注文学、艺术的实际存在状态及其已然或可能发生的变化。这样一个浅近的道理大约谁都不会怀疑。然而，当下正在我们身边发生的艺术世界总体格局的巨大变化却又常常为我们视而不见。这不能不带来诸多理论上的遗憾。";
    vector<string> ret = split->cut(sentence);
    for(auto word:ret){
        cout << "word = " << word << "\n";
    }

    return 0;
}

