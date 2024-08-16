#include <ctype.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <string>
using namespace std;

class Dictionary
{
public:
    Dictionary(){};

    void read(const string &filename) {
        ifstream file(filename);
        string line, word;

        if (!file) {
            cerr << "open file failed!" << endl;
        }

        while(getline(file, line)) {
            istringstream ss(line);
            while(ss >> word) {
                word = clean(word);
                updateDict(word);
            }
        }
        file.close();
    }

    void store(const string &filename) {
        ofstream file(filename);
        if (!file) {
            cerr << "open file failed!" << endl;
        }

        for (auto& it : _dict) {
            file << it.first << " " << it.second << endl;
        }
    }

private:
    void updateDict(const string& word) {
        if (word.empty())   return;
        ++_dict[word];
    }

    string clean(const string & str) {
        string result;
        for (char c : str) {
            if (isalpha(c)) {
                result.append(1, tolower(c));
            }
        }
        return result;
    }

private:
    unordered_map<string, int> _dict;
};

int main(int argc, char* argv[]) {
    Dictionary dic;
    dic.read("The_Holy_Bible.txt");
    dic.store("unordered_map_dict.txt");

    return 0;
}
