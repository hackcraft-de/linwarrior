#include "Filesystem.h"

#include <iostream>
#include <sstream>
#include <cstdio>
#include <list>


Filesystem* Filesystem::instance = NULL;


Filesystem::Filesystem() {
    
    instance = this;
    
    prefixes.push_back("mod");
    prefixes.push_back("ext");
    prefixes.push_back("data");
    prefixes.push_back("");
}


Filesystem* Filesystem::getInstance() {
    if (instance == NULL) {
        new Filesystem();
    }
    return instance;
}


std::list<std::string>* Filesystem::getDirectoryPrefixes() {
    return & (getInstance()->prefixes);
}


char* Filesystem::loadTextFile(const char* filename) {
    
    // Open file from any registered location.
    FILE* f = openReadOnlyBinaryFile(filename);
    
    // Return empty string (0 terminated) if there is no such file.
    if (f == NULL) {
        char* cstr = new char[1];
        cstr[0] = 0;
        return cstr;
    }
    
    // Determine file length and allocate string array.
    fseek(f, 0, SEEK_END);
    unsigned long s = ftell(f);
    char* cstr = new char[s + 1];
    rewind(f);
    
    // Read file contents into array.
    if (fread(cstr, s, 1, f) != 1) cstr[0] = 0;
    cstr[s] = 0;
    
    fclose(f);
    
    return cstr;
}


char* Filesystem::loadBinaryFile(const char* filename, long* filesize) {
    
    // In case of an error nothing is read.
    *filesize = 0;
    
    // Open file from any registered location.
    FILE* f = openReadOnlyBinaryFile(filename);
    
    // Return empty string (0 terminated) if there is no such file.
    if (f == NULL) {
        return NULL;
    }
    
    // Determine file length and allocate string array.
    fseek(f, 0, SEEK_END);
    unsigned long s = ftell(f);
    char* cstr = new char[s];
    rewind(f);
    
    // Read file contents into array.
    if (fread(cstr, s, 1, f) != 1) {
        delete[] cstr;
        s = 0;
    }
    
    fclose(f);
    
    *filesize = s;
    
    return cstr;
}


FILE* Filesystem::openReadOnlyTextFile(const char* filename) {
    
    return openFile(filename, "rt");
}


FILE* Filesystem::openReadOnlyBinaryFile(const char* filename) {
    
    return openFile(filename, "rb");
}


FILE* Filesystem::openWriteOnlyBinaryFile(const char* filename) {
    
    return openFile(filename, "wb");
}


FILE* Filesystem::openFile(const char* filename, const char* opentype) {

    // Try to open file from given directories.
    
    for (std::string prefix : getInstance()->prefixes) {
    
        std::stringstream fname;

        fname << prefix << filename;
        std::cout << fname.str() << "\n";
        FILE* f = fopen(fname.str().c_str(), opentype);

        if (f != NULL) {
            return f;
        }
    }
    
    return NULL;
}
