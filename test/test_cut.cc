#include "../include/SplitTool.h"
#include <iostream>
#include <string>
#include <vector>
using namespace std;


int main() {
    cout << "start\n";
    string sen = "  helo   asd  123123  a1 23b 中 asd\n\n你好";      
    vector words = SplitTool::getInstance()->cut(sen);
    for (int i = 0; i < words.size(); ++i) {
        if (words[i] != " ") {
            cout << "word " << i << " = " << words[i] << " end\n"; 
        }
    }
}
