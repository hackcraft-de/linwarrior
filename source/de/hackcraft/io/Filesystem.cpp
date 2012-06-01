#include "Filesystem.h"

#include <cstdio>

char* Filesystem::loadTextFile(const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (f == NULL) {
        char* cstr = new char[1];
        cstr[0] = 0;
        return cstr;
    }
    fseek(f, 0, SEEK_END);
    unsigned long s = ftell(f);
    char* cstr = new char[s + 1];
    rewind(f);
    if (fread(cstr, s, 1, f) != 1) cstr[0] = 0;
    cstr[s] = 0;
    fclose(f);
    return cstr;
}
