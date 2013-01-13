/* 
 * File:     Filesystem.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on December 31, 2011, 4:05 PM
 */

#ifndef FILESYSTEM_H
#define	FILESYSTEM_H

#include <stdio.h>

/**
 * Class that shall provide means to load files from the filesystem.
 */
class Filesystem {
public:
    /** Loading whole text file. */
    static char* loadTextFile(const char* filename);

    /** Try to open text file from any data directory. */
    static FILE* openReadOnlyTextFile(const char* filename);
    /** Try to open binary file from any data directory. */
    static FILE* openReadOnlyBinaryFile(const char* filename);
    /** Try to open binary file from any data directory. */
    static FILE* openWriteOnlyBinaryFile(const char* filename);
    
private:
    /** Try to open file from any data directory. */
    static FILE* openFile(const char* filename, const char* opentype);
};

#endif	/* FILESYSTEM_H */

