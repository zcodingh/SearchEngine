#include "../include/PageLib.h"
#include "../include/Configuration.h"
#include "../include/FileProcessor.h"

int main() {
    FileProcessor processor;
    PageLib lib(processor);
    lib.store();
}