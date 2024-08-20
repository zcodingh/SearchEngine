#include "../include/SearchEngineServer.h"
#include "../include/Configuration.h"
#include "../include/Dictionary.h"
#include <iostream>
using std::cout;

int main(int argc, char* argv[]) {
    SearchEngineServer server;
    server.start();
    return 0;
}