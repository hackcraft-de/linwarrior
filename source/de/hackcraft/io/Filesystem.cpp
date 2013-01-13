#include "Filesystem.h"

#include <iostream>
#include <sstream>
#include <cstdio>


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
    
    // For now only just open the filename as it is
    // later try 1st data directory then second and so on.
    
    std::stringstream fname;
    
    fname << filename;
    
    //std::list<std::string> prefixes;
    
    FILE* f = fopen(fname.str().c_str(), opentype);
    
    return f;
}
