/* 
 * File:     Shaderfile.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on February 20, 2013, 7:07 PM
 */

#ifndef SHADERFILE_H
#define	SHADERFILE_H

class Shaderfile {
public:
    Shaderfile();
    virtual ~Shaderfile();
private:
public:
    /** Loading whole shader text file with include support. */
    static char* loadShaderFile(const char* filename);
};

#endif	/* SHADERFILE_H */

