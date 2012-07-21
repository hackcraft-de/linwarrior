/* 
 * File:    GL.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on July 21, 2012, 11:36 AM
 */


#ifndef GL_H
#define	GL_H

// Prevent inclusion into namespace by glew.
#include <stddef.h>
#include <stdint.h>

#if 1

namespace GL {
#include <GL/glew.h>
}

#else

class GL;

namespace GLEW {
#include <GL/glew.h>
}

/**
 * Interface to OpenGL.
 * 
 * Versions of OpenGL higher than 2.1 and embedded versions
 * are tossing glDisable, glEnable glVertex, glColor, the matrix stack,
 * the attribute stack and other functions in favor of
 * a client defined TnL and buffer based implementation.
 * 
 * For enhanced and lasting compatibility, and for rising quality
 * and performance, a transition has to be made.
 * 
 * Therefore:
 * 1. Collect scattered OpenGL function calls and redirect to this class.
 * 2. Mark methods which are removed in later (OpenGL) versions as deprecated.
 * 3. Either remove calls to these methods (vertex/color) or build workarounds (matrix stack).
 */
class GL {
public:
    
    typedef GLEW::GLbitfield GLbitfield;
    typedef GLEW::GLboolean GLboolean;
    typedef GLEW::GLcharARB GLcharARB;
    typedef GLEW::GLclampf GLclampf;
    typedef GLEW::GLclampd GLclampd;
    typedef GLEW::GLdouble GLdouble;
    typedef GLEW::GLenum GLenum;
    typedef GLEW::GLfloat GLfloat;
    typedef GLEW::GLint GLint;
    typedef GLEW::GLsizei GLsizei;
    typedef GLEW::GLuint GLuint;
    typedef GLEW::GLubyte GLubyte;
    
    
    // Misc
    
    static void inline glFlush() {
        GLEW::glFlush();
    }
    
    // View
    
    static inline void glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar) {
        GLEW::glOrtho(left, right, bottom, top, zNear, zFar);
    }
    
    static inline void glViewport(GLint x, GLint y, GLsizei width, GLsizei height) {
        GLEW::glViewport(x, y, width, height);
    }
    
    static inline void glClear(GLbitfield mask) {
        GLEW::glClear(mask);
    }
    
    static inline void glClearAccum(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) {
        GLEW::glClearAccum(red, green, blue, alpha);
    }
    
    static inline void glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) {
        GLEW::glClearColor(red, green, blue, alpha);
    }
    
    static inline void glClearDepth(GLclampd depth) {
        GLEW::glClearDepth(depth);
    }

    // Query
    
    static inline void glGetFloatv(GLenum pname, GLfloat *params) {
        GLEW::glGetFloatv(pname, params);
    }
    
    static inline void glGetIntegerv(GLenum pname, GLint *params) {
        GLEW::glGetIntegerv(pname, params);
    }
    
    static inline const GLubyte * glGetString(GLenum name) {
        return GLEW::glGetString(name);
    }
    
    // Render modes:
    
    static inline void glAccum(GLenum op, GLfloat value) {
        GLEW::glAccum(op, value);
    }
    
    static inline void glAlphaFunc(GLenum func, GLclampf ref) {
        GLEW::glAlphaFunc(func, ref);
    }
    
    static inline void glBlendFunc(GLenum sfactor, GLenum dfactor) {
        GLEW::glBlendFunc(sfactor, dfactor);
    }
    
    static inline void glDepthFunc(GLenum func) {
        GLEW::glDepthFunc(func);
    }
    
    static inline void glDepthMask(GLboolean flag) {
        GLEW::glDepthMask(flag);
    }

    // Attribute:
    
    /** Deprecated: Remove - use shaders and attributes. */
    static inline void glPushAttrib(GLbitfield mask) {
        GLEW::glPushAttrib(mask);
    }
    
    /** Deprecated: Remove - use shaders and attributes. */
    static inline void glPopAttrib() {
        GLEW::glPopAttrib();
    }

    /** Deprecated: Remove - use shaders and attributes. */
    static inline void glEnable(GLenum cap) {
        GLEW::glEnable(cap);
    }
    
    /** Deprecated: Remove - use shaders and attributes. */
    static inline void glDisable(GLenum cap) {
        GLEW::glDisable(cap);
    }
    
    // Matrix:
    
    /** Deprecated: Implement matrix stack. */
    static inline void glMatrixMode(GLEW::GLenum mode) {
        GLEW::glMatrixMode(mode);
    }
    
    /** Deprecated: Implement matrix stack. */
    static inline void glLoadIdentity() {
        GLEW::glLoadIdentity();
    }
    
    /** Deprecated: Implement matrix stack. */
    static inline void glPushMatrix() {
        GLEW::glPushMatrix();
    }
    
    /** Deprecated: Implement matrix stack. */
    static inline void glPopMatrix() {
        GLEW::glPopMatrix();
    }

    /** Deprecated: Implement matrix stack. */
    static inline void glMultMatrixf (const GLEW::GLfloat *m) {
        GLEW::glMultMatrixf(m);
    }
    
    /** Deprecated: Implement matrix stack. */
    static inline void glTranslatef(GLEW::GLfloat x, GLEW::GLfloat y, GLEW::GLfloat z) {
        GLEW::glTranslatef(x, y, z);
    }
    
    /** Deprecated: Implement matrix stack. */
    static inline void glRotatef (GLEW::GLfloat angle, GLEW::GLfloat x, GLEW::GLfloat y, GLEW::GLfloat z) {
        GLEW::glRotatef(angle, x, y, z);
    }
    
    /** Deprecated: Implement matrix stack. */
    static inline void glScalef (GLEW::GLfloat x, GLEW::GLfloat y, GLEW::GLfloat z) {
        GLEW::glScalef(x, y, z);
    }
    
    // Render:

    /** Deprecated: Use shaders and buffers. */
    static inline void glBegin(GLEW::GLenum mode) {
        GLEW::glBegin(mode);
    }

    /** Deprecated: Use shaders and buffers. */
    static inline void glEnd() {
        GLEW::glEnd();
    }
    
    /** Deprecated: Use shaders and buffers. */
    static inline void glVertex3f(GLEW::GLfloat x, GLEW::GLfloat y, GLEW::GLfloat z) {
        GLEW::glVertex3f(x, y, z);
    }
    
    /** Deprecated: Use shaders and buffers. */
    static inline void glVertex3fv(const GLEW::GLfloat *v) {
        GLEW::glVertex3fv(v);
    }
    
    /** Deprecated: Use shaders and buffers. */
    static inline void glNormal3f(GLEW::GLfloat x, GLEW::GLfloat y, GLEW::GLfloat z) {
        GLEW::glNormal3f(x, y, z);
    }
    
    /** Deprecated: Use shaders and buffers. */
    static inline void glNormal3fv(const GLEW::GLfloat *v) {
        GLEW::glNormal3fv(v);
    }
    
    /** Deprecated: Use shaders and buffers. */
    static inline void glColor3f(GLEW::GLfloat r, GLEW::GLfloat g, GLEW::GLfloat b) {
        GLEW::glColor3f(r, g, b);
    }
    
    /** Deprecated: Use shaders and buffers. */
    static inline void glColor4f(GLEW::GLfloat r, GLEW::GLfloat g, GLEW::GLfloat b, GLEW::GLfloat a) {
        GLEW::glColor4f(r, g, b, a);
    }
    
    /** Deprecated: Use shaders and buffers. */
    static inline void glColor4fv(GLEW::GLfloat *v) {
        GLEW::glColor4fv(v);
    }
    
    /** Deprecated: Use shaders and buffers. */
    static inline void glTexCoord2f(GLEW::GLfloat s, GLEW::GLfloat t) {
        GLEW::glTexCoord2f(s, t);
    }
    
    /** Deprecated: Use shaders and buffers. */
    static inline void glTexCoord2i(GLEW::GLint s, GLEW::GLint t) {
        GLEW::glTexCoord2i(s, t);
    }
    
    /** Deprecated: Use shaders and buffers. */
    static inline void glTexCoord3f(GLEW::GLfloat s, GLEW::GLfloat t, GLEW::GLfloat u) {
        GLEW::glTexCoord3f(s, t, u);
    }
    
    /** Deprecated: Use shaders and buffers. */
    static inline void glTexCoord3fv(const GLEW::GLfloat *v) {
        GLEW::glTexCoord3fv(v);
    }
    
    
    // Texture-Management
    
    static inline void glGenTextures(GLEW::GLsizei n, GLEW::GLuint *textures) {
        GLEW::glGenTextures(n, textures);
    }
    
    static inline void glBindTexture(GLEW::GLenum target, GLEW::GLuint texture) {
        GLEW::glBindTexture(target, texture);
    }
    
    static inline void glPixelStorei(GLEW::GLenum pname, GLEW::GLint param) {
        GLEW::glPixelStorei(pname, param);
    }
    
    static inline void glTexEnvf(GLEW::GLenum target, GLEW::GLenum pname, GLEW::GLfloat param) {
        glTexEnvf(target, pname, param);
    }
    
    /** use glTexParameteri instead? */
    static inline void glTexParameterf (GLEW::GLenum target, GLEW::GLenum pname, GLEW::GLfloat param) {
        GLEW::glTexParameterf(target, pname, param);
    }
    
    static inline void glTexParameteri (GLEW::GLenum target, GLEW::GLenum pname, GLEW::GLint param) {
        GLEW::glTexParameteri(target, pname, param);
    }
    
    static inline void glTexImage2D(GLEW::GLenum target, GLEW::GLint level, GLEW::GLint internalformat, GLEW::GLsizei width, GLEW::GLsizei height, GLEW::GLint border, GLEW::GLenum format, GLEW::GLenum type, const GLEW::GLvoid *pixels) {
        GLEW::glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
    }
    
    static inline void glTexImage3D(GLEW::GLenum target, GLEW::GLint level, GLEW::GLint internalFormat, GLEW::GLsizei width, GLEW::GLsizei height, GLEW::GLsizei depth, GLEW::GLint border, GLEW::GLenum format, GLEW::GLenum type, const GLEW::GLvoid *pixels) {
        GLEW_GET_FUN(GLEW::__glewTexImage3D)(target, level, internalFormat, width, height, depth, border, format, type, pixels);
    }
    
    static inline void glCopyTexImage2D(GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border) {
        GLEW::glCopyTexImage2D(target, level, internalFormat, x, y, width, height, border);
    }
    
    static inline void glCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height) {
        GLEW::glCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
    }
    
    
};

#endif

#endif	/* GL_H */

