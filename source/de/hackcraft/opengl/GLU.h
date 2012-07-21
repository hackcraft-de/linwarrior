/* 
 * File:    GLU.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on July 21, 2012, 4:05 PM
 */


#ifndef GLU_H
#define	GLU_H

#include "de/hackcraft/opengl/GL.h"

class GLU {
public:
    static inline void gluBuild2DMipmaps (GL::GLenum target, GL::GLint internalFormat, GL::GLsizei width, GL::GLsizei height, GL::GLenum format, GL::GLenum type, const void *data) {
        GL::gluBuild2DMipmaps(target, internalFormat, width, height, format, type, data);
    }
    
    static inline void gluPerspective (GL::GLdouble fovy, GL::GLdouble aspect, GL::GLdouble zNear, GL::GLdouble zFar) {
        GL::gluPerspective (fovy, aspect, zNear, zFar);
    }
};

#endif	/* GLU_H */

