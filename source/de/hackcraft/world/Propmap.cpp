#include "Propmap.h"

#include "de/hackcraft/io/Filesystem.h"


#include <string>
using std::string;

#include <sstream>
using std::stringstream;

#include <iostream>
using std::cout;

void Propmap::load(const char* filename) {
    cout << "Loading Property-File: " << filename << "\n";
    
    char* contents = Filesystem::loadTextFile(filename);
    if (contents == NULL) throw new string("Could not load property file.");
    stringstream lines(contents);
    delete contents;
    
    int ln = 0;
    string line;
    
    while (std::getline(lines, line)) {
        
        ln++;
        
        //std::cout << ln << "| " << line << "\n";
        
        if (line.size() > 0 && line.at(0) != '#') {
            string key;
            string value;
            stringstream kv(line);
            std::skipws(kv);
            std::getline(kv, key, '=');
            std::skipws(kv);
            std::getline(kv, value);
            
            std::cout << ln << "|" << key << "=" << value << "\n";
            this->put(&key, &value);
        }
    }
    
    cout << "Done loading Property-File.\n";
}
