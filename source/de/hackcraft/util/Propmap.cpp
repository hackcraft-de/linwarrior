#include "Propmap.h"

#include "de/hackcraft/io/Filesystem.h"

#include "de/hackcraft/log/Logger.h"

#include <string>
using std::string;

#include <sstream>
using std::stringstream;


Logger* Propmap::logger = Logger::getLogger("de.hackcraft.util.Propmap");

void Propmap::load(const char* filename) {
    logger->debug() << "Loading Property-File: " << filename << "\n";
    
    char* contents = Filesystem::loadTextFile(filename);
    if (contents == NULL) throw new string("Could not load property file.");
    stringstream lines(contents);
    delete[] contents;
    
    int ln = 0;
    string line;
    
    while (std::getline(lines, line)) {
        
        // Remove trailing carriage return characters if any.
        if (line.size() > 0 && line[line.size()-1] == '\r') {
            line.resize(line.size()-1);
        }
        
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
            
            logger->debug() << ln << "|" << key << "=" << value << "\n";
            this->put(&key, &value);
        }
    }
    
    logger->debug() << "Done loading Property-File.\n";
}
