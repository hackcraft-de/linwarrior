#include "Shaderfile.h"

#include "de/hackcraft/io/Filesystem.h"

#include <sstream>
#include <iostream>
#include <string.h>


Shaderfile::Shaderfile() {
}


Shaderfile::~Shaderfile() {
}


char* Shaderfile::loadShaderFile(const char* filename) {
    
    std::stringstream preprocessedOutput;
    
    char *text = Filesystem::loadTextFile(filename);
    
    if (text == NULL) {
        throw "Could not open a shader file.";
    }
    
    std::stringstream s;
    
    s << text;
    
    char line[1024];
    while ( s.getline(line, sizeof(line)) ) {

        //std::cout << "> " << line << "\n";
        
        char inc[1024];
        char filenameWithQuotes[1024];
        char* filenameNoQuotes = NULL;

        std::stringstream l;
        l << line;
        
        if (l >> inc >> filenameWithQuotes) {
            if (strcmp("#include", inc) == 0) {
                
                //std::cout << inc << " <=> [" << filenameWithQuotes << "]\n";
                
                filenameNoQuotes = &filenameWithQuotes[1];
                filenameNoQuotes[strlen(filenameNoQuotes) - 1] = 0;
                
                std::cout << inc << " <=> [" << filenameNoQuotes << "]\n";
            }
        }
        
        bool isIncludeLine = (filenameNoQuotes != NULL);
        
        if (isIncludeLine) {
            char* included = loadShaderFile(filenameNoQuotes);
            preprocessedOutput << included << "\n";
            delete[] included;
        } else {
            preprocessedOutput << line << "\n";
        }
        
    }
    
    //std::cout << preprocessedOutput.str();
    
    long size = strlen(preprocessedOutput.str().c_str()) + 1;
    //std::cout <<  size << "\n";
    char* result = new char[size];
    strcpy(result, preprocessedOutput.str().c_str());
    return result;
}

