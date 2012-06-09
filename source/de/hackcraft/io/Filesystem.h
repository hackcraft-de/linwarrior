/* 
 * File:     Filesystem.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on December 31, 2011, 4:05 PM
 */

#ifndef FILESYSTEM_H
#define	FILESYSTEM_H

/**
 * Class that shall provide means to load files from the filesystem.
 */
class Filesystem {
    public:
    /** Loading whole text file. */
    static char* loadTextFile(const char* filename);
};

#endif	/* FILESYSTEM_H */

