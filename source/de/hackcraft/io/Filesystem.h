/* 
 * File:     Filesystem.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on December 31, 2011, 4:05 PM
 */

#ifndef FILESYSTEM_H
#define	FILESYSTEM_H

#include <list>
#include <stdio.h>
#include <string>

/**
 * Class that shall provide means to load files from the filesystem.
 */
class Filesystem {
public:
    Filesystem();
    
    static Filesystem* getInstance();
    
    static std::list<std::string>* getDirectoryPrefixes();
    
    /** Loading whole text file with zero termination. */
    static char* loadTextFile(const char* filename);
    
    /** Loading whole binary file and deliver loaded file size. */
    static char* loadBinaryFile(const char* filename, long* filesize);

    /** Try to open text file from any data directory. */
    static FILE* openReadOnlyTextFile(const char* filename);
    /** Try to open binary file from any data directory. */
    static FILE* openReadOnlyBinaryFile(const char* filename);
    /** Try to open binary file from any data directory. */
    static FILE* openWriteOnlyBinaryFile(const char* filename);
    
private:
    /** Try to open file from any data directory. */
    static FILE* openFile(const char* filename, const char* opentype);

private:
    static Filesystem* instance;
    
private:
    std::list<std::string> prefixes;
};

#endif	/* FILESYSTEM_H */

