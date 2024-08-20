#include "../include/nlohmann/json.hpp"
#include <iostream>
using std::cout;
using nlohmann::json;

int main(int argc, char* argv[]) {
    json obj1;
    obj1[0] = "hello";
    obj1[1] = "world"; 
    cout << obj1.dump() << "\n";
    json obj2({"test", "obj"});
    cout << obj2.dump() << "\n";
    json obj3;
    obj3.push_back({"test", "ojb3"});
    obj3.push_back({"test", "ojb3"});
    cout << obj3.dump() << "\n";
    return 0;
}

