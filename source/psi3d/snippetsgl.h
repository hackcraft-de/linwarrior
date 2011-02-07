/* 
 * File:   snippetsgl.h
 * Author: Benjamin Pickhardt
 * Home:   hackcraft.de
 *
 * Created on September 15, 2008, 12:54 PM
 */

#ifndef _SNIPPETSGL_H
#define	_SNIPPETSGL_H

// memcpy
#include "string.h"

/// Primitives
struct cPrimitives {

    static inline void glVertex3fv2f(float* pnt, float u, float v) {
        glTexCoord2f(u,v);
        glVertex3fv(pnt);
    }

    static inline void glVertex3fv2fv(float* pnt, float* tex) {
        glTexCoord2f(tex[0], tex[1]);
        glVertex3fv(pnt);
    }

    static inline void glVertex3fv3fv(float* pnt, float* tex) {
        glTexCoord3fv(tex);
        glVertex3fv(pnt);
    }

    static void glBalloid() {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);

        int steps = 2;
        double delta = 2.0f / (float) steps;

        float c[3];

        // xy
        loopk (2) {
            float dir = (k & 1) ? -1 : +1;
            glColor4f(0.9f,0.5f,0.5f,1.0f);
            //glColor4f(t==0?1:0.5,t==1?1:0.5,t==2?1:0.5,1);

            double b = -1;
            loopj(steps) {
                double b0 = b * -dir;
                double b1 = (b + delta) * -dir;
                double a = -1;
                glBegin(GL_TRIANGLE_STRIP);
                loopi(steps+1) {

                    float inv0 = 1.0f / (float) sqrtf(a*a+b0*b0+1.0f);
                    float inv1 = 1.0f / (float) sqrtf(a*a+b1*b1+1.0f);
                    float v0[] = { a*inv0, b0*inv0, dir*inv0 };
                    float v1[] = { a*inv1, b1*inv1, dir*inv1 };


                    glNormal3fv(v0);
                    //transformV3(v0,c);
                    glColor3fv(c);
                    glVertex3fv(v0);

                    glNormal3fv(v1);
                    //transformV3(v1,c);
                    glColor3fv(c);
                    glVertex3fv(v1);

                    a += delta;
                }
                glEnd();
                b += delta;
            }
        }

        // xz
        loopk (2) {
            float dir = (k & 1) ? -1 : +1;
            glColor4f(0.5f,0.9f,0.5f,1.0f);
            //glColor4f(t==0?1:0.5,t==1?1:0.5,t==2?1:0.5,1);

            double b = -1;
            loopj(steps) {
                double b0 = b * dir;
                double b1 = (b + delta) * dir;
                double a = -1;
                glBegin(GL_TRIANGLE_STRIP);
                loopi(steps+1) {

                    float inv0 = 1.0f / (float) sqrtf(a*a+b0*b0+1.0f);
                    float inv1 = 1.0f / (float) sqrtf(a*a+b1*b1+1.0f);
                    float v0[] = { a*inv0, dir*inv0, b0*inv0 };
                    float v1[] = { a*inv1, dir*inv1, b1*inv1 };

                    glNormal3fv(v0);
                    //transformV3(v0,c);
                    glColor3fv(c);
                    glVertex3fv(v0);

                    glNormal3fv(v1);
                    //transformV3(v1,c);
                    glColor3fv(c);
                    glVertex3fv(v1);

                    a += delta;
                }
                glEnd();
                b += delta;
            }
        }

        // yz
        loopk (2) {
            float dir = (k & 1) ? -1 : +1;
            glColor4f(0.5f,0.5f,0.9f,1.0f);
            //glColor4f(t==0?1:0.5,t==1?1:0.5,t==2?1:0.5,1);

            double b = -1;
            loopj(steps) {
                double b0 = b * -dir;
                double b1 = (b + delta) * -dir;
                double a = -1;
                glBegin(GL_TRIANGLE_STRIP);
                loopi(steps+1) {

                    //a = fma(a,1,0);
                    float inv0 = 1.0f / (float) sqrtf(a*a+b0*b0+1.0f);
                    float inv1 = 1.0f / (float) sqrtf(a*a+b1*b1+1.0f);
                    float v0[] = { dir*inv0, a*inv0, b0*inv0 };
                    float v1[] = { dir*inv1, a*inv1, b1*inv1 };

                    glNormal3fv(v0);
                    //transformV3(v0,c);
                    glColor3fv(c);
                    glVertex3fv(v0);

                    glNormal3fv(v1);
                    //transformV3(v1,c);
                    glColor3fv(c);
                    glVertex3fv(v1);

                    a += delta;
                }
                glEnd();
                b += delta;
            }
        }
    }

    static inline void glAxis(float l = 1.0f) {
        glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT);
        {
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_LIGHTING);
            glBegin(GL_LINES);
            {
                glColor4f(1,0,0,1);
                glVertex3f(0,0,0);
                glVertex3f(l,0,0);
                glColor4f(0,1,0,1);
                glVertex3f(0,0,0);
                glVertex3f(0,l,0);
                glColor4f(0,0,1,1);
                glVertex3f(0,0,0);
                glVertex3f(0,0,l);
            }
            glEnd();
        }
        glPopAttrib();
    }

    static inline void glLineSquare(float border = 0) {
        float lo = border;
        float hi = 1 - border;
        glBegin(GL_LINE_STRIP);
        {
            glVertex3f(lo, lo, 0);
            glVertex3f(hi, lo, 0);
            glVertex3f(hi, hi, 0);
            glVertex3f(lo, hi, 0);
            glVertex3f(lo, lo, 0);
        }
        glEnd();
    }

    static inline void glDisk(int segments = 9, float radius = 1.0f) {
        int n = 2 + (segments);
        float step = 2*M_PI / (float) n;
        float a = 0.0f;
        glBegin(GL_TRIANGLE_FAN);
        glNormal3f(0,0,1);
        loopi(n) {
            glTexCoord3f(0.45+0.499*sin(a), 0.5+0.499*cos(a), 0);
            glVertex3f(sin(a)*(radius), cos(a)*(radius), 0);
            a += step;
        }
        glEnd();
    }

    static inline void glXCenteredTextureSquare() {
        glBegin(GL_QUADS);
        {
            glTexCoord2i(1, 0);
            glVertex3f(+0.5f, 0.0f, 0);
            glTexCoord2i(0, 0);
            glVertex3f(-0.5f, 0.0f, 0);
            glTexCoord2i(0, 1);
            glVertex3f(-0.5f, +1.0f, 0);
            glTexCoord2i(1, 1);
            glVertex3f(+0.5f, +1.0f, 0);
        }
        glEnd();
    }

    static inline void glXYCenteredTextureSquare(float s = 0.5f) {
        glBegin(GL_QUADS);
        {
            glTexCoord2i(1, 0);
            glVertex3f(+s, -s, 0);
            glTexCoord2i(0, 0);
            glVertex3f(-s, -s, 0);
            glTexCoord2i(0, 1);
            glVertex3f(-s, +s, 0);
            glTexCoord2i(1, 1);
            glVertex3f(+s, +s, 0);
        }
        glEnd();
    }

    static inline void glUnitCylinder(float segments = 9) {
        int n = 2 + (segments);
        float step = 1.0f / (float) n;
        float anglestep = 2.0f * M_PI * step;
        float angle = 0.0f;
        glBegin(GL_TRIANGLE_STRIP);
        for (int i=0; i<=n; i++) {
            glTexCoord3f(0.5f+0.499f*sin(angle), 0.999f, 0.5f+0.499*cos(angle));
            glVertex3f(sin(angle), +1.0f, cos(angle));
            glTexCoord3f(0.5f+0.499f*sin(angle), 0.001f, 0.5f+0.499*cos(angle));
            glVertex3f(sin(angle), +0.0f, cos(angle));
            angle += anglestep;
        }
        glEnd();
    }

    static inline void glCenterUnitCylinder(float segments = 9) {
        float step = 1.0f / (float) segments;
        float anglestep = 2.0f * M_PI *step;
        float angle = 0.0f;
        glBegin(GL_TRIANGLE_STRIP);
        for (int i=0; i<=segments; i++) {
            glTexCoord3f(0.5f+0.499f*sin(angle), 0.999f, 0.5f+0.499f*cos(angle));
            glVertex3f(sin(angle), +1.0f, cos(angle));
            glTexCoord3f(0.5f+0.499f*sin(angle), 0.001f, 0.5f+0.499f*cos(angle));
            glVertex3f(sin(angle), -1.0f, cos(angle));
            angle += anglestep;
        };
        glEnd();
    }

    static inline void glUnitBlock()
    {
        GLfloat p[][3] = {
            {0,0,0}, {1,0,0}, {1,1,0}, {0,1,0},
            {0,0,1}, {1,0,1}, {1,1,1}, {0,1,1}
        };
        const float a = 0.01f;
        const float b = 0.99f;
        GLfloat t[][3] = {
            {a,a,a}, {b,a,a}, {b,b,a}, {a,b,a},
            {a,a,b}, {b,a,b}, {b,b,b}, {a,b,b}
        };
        glBegin(GL_QUADS);
        {
            /* front */
            glNormal3f(0, 0, -1);
            glVertex3fv3fv(p[1], t[1]); glVertex3fv3fv(p[0], t[0]);
            glVertex3fv3fv(p[3], t[3]); glVertex3fv3fv(p[2], t[2]);
            /* back */
            glNormal3f(0, 0, +1);
            glVertex3fv3fv(p[4], t[4]); glVertex3fv3fv(p[5], t[5]);
            glVertex3fv3fv(p[6], t[6]); glVertex3fv3fv(p[7], t[7]);
            /* top */
            glNormal3f(0, -1, 0);
            glVertex3fv3fv(p[0], t[0]); glVertex3fv3fv(p[1], t[1]);
            glVertex3fv3fv(p[5], t[5]); glVertex3fv3fv(p[4], t[4]);
            /* bottom */
            glNormal3f(0, +1, 0);
            glVertex3fv3fv(p[2], t[2]); glVertex3fv3fv(p[3], t[3]);
            glVertex3fv3fv(p[7], t[7]); glVertex3fv3fv(p[6], t[6]);
            /* left */
            glNormal3f(+1, 0, 0);
            glVertex3fv3fv(p[0], t[0]); glVertex3fv3fv(p[4], t[4]);
            glVertex3fv3fv(p[7], t[7]); glVertex3fv3fv(p[3], t[3]);
            /* right */
            glNormal3f(-1, 0, 0);
            glVertex3fv3fv(p[5], t[5]); glVertex3fv3fv(p[1], t[1]);
            glVertex3fv3fv(p[2], t[2]); glVertex3fv3fv(p[6], t[6]);
        }
        glEnd();
    }

    static inline void glCenterUnitBlock() {
        GLfloat p[][3] = {
            {-1,-1,-1}, {+1,-1,-1}, {+1,+1,-1}, {-1,+1,-1},
            {-1,-1,+1}, {+1,-1,+1}, {+1,+1,+1}, {-1,+1,+1}
        };
        const float a = 0.01f;
        const float b = 0.99f;
        GLfloat t[][3] = {
            {a,a,a}, {b,a,a}, {b,b,a}, {a,b,a},
            {a,a,b}, {b,a,b}, {b,b,b}, {a,b,b}
        };
        glBegin(GL_QUADS);
        {
            /* front */
            glNormal3f(0, 0, -1);
            glVertex3fv3fv(p[1], t[1]); glVertex3fv3fv(p[0], t[0]);
            glVertex3fv3fv(p[3], t[3]); glVertex3fv3fv(p[2], t[2]);
            /* back */
            glNormal3f(0, 0, +1);
            glVertex3fv3fv(p[4], t[4]); glVertex3fv3fv(p[5], t[5]);
            glVertex3fv3fv(p[6], t[6]); glVertex3fv3fv(p[7], t[7]);
            /* top */
            glNormal3f(0, -1, 0);
            glVertex3fv3fv(p[0], t[0]); glVertex3fv3fv(p[1], t[1]);
            glVertex3fv3fv(p[5], t[5]); glVertex3fv3fv(p[4], t[4]);
            /* bottom */
            glNormal3f(0, +1, 0);
            glVertex3fv3fv(p[2], t[2]); glVertex3fv3fv(p[3], t[3]);
            glVertex3fv3fv(p[7], t[7]); glVertex3fv3fv(p[6], t[6]);
            /* left */
            glNormal3f(+1, 0, 0);
            glVertex3fv3fv(p[0], t[0]); glVertex3fv3fv(p[4], t[4]);
            glVertex3fv3fv(p[7], t[7]); glVertex3fv3fv(p[3], t[3]);
            /* right */
            glNormal3f(-1, 0, 0);
            glVertex3fv3fv(p[5], t[5]); glVertex3fv3fv(p[1], t[1]);
            glVertex3fv3fv(p[2], t[2]); glVertex3fv3fv(p[6], t[6]);
        }
        glEnd();
    }

    static inline void glBlockFlat(float width, float height, float depth, int left, int right, int front, int back, int top, int bottom) {
        float a = 0, b = 0, c = 0;
        float x = width;
        float y = height;
        float z = depth;
        GLfloat p[][3] = {
            {a,b,c}, {x,b,c}, {x,y,c}, {a,y,c},
            {a,b,z}, {x,b,z}, {x,y,z}, {a,y,z}
        };
        GLfloat t[][2] = {
            {a,b}, {x,b}, {x,y}, {a,y},
            {c,b}, {z,b}, {z,y}, {c,y},
            {a,c}, {x,c}, {x,z}, {a,z},
        };
        glBindTexture(GL_TEXTURE_2D, front);
        glBegin(GL_QUADS);
        {
            glNormal3f(0, 0, -1);
            glVertex3fv2fv(p[1], t[1]); glVertex3fv2fv(p[0], t[0]);
            glVertex3fv2fv(p[3], t[3]); glVertex3fv2fv(p[2], t[2]);
        }
        glEnd();
        if (back != front) glBindTexture(GL_TEXTURE_2D, back);
        glBegin(GL_QUADS);
        {
            glNormal3f(0, 0, +1);
            glVertex3fv2fv(p[4], t[1]); glVertex3fv2fv(p[5], t[0]);
            glVertex3fv2fv(p[6], t[3]); glVertex3fv2fv(p[7], t[2]);
        }
        glEnd();
        if (left != back) glBindTexture(GL_TEXTURE_2D, left);
        glBegin(GL_QUADS);
        {
            glNormal3f(+1, 0, 0);
            glVertex3fv2fv(p[0], t[5]); glVertex3fv2fv(p[4], t[4]);
            glVertex3fv2fv(p[7], t[7]); glVertex3fv2fv(p[3], t[6]);
        }
        glEnd();
        if (right != left) glBindTexture(GL_TEXTURE_2D, right);
        glBegin(GL_QUADS);
        {
            glNormal3f(-1, 0, 0);
            glVertex3fv2fv(p[5], t[5]); glVertex3fv2fv(p[1], t[4]);
            glVertex3fv2fv(p[2], t[7]); glVertex3fv2fv(p[6], t[6]);
        }
        glEnd();
        if (top != right) glBindTexture(GL_TEXTURE_2D, top);
        glBegin(GL_QUADS);
        {
            glNormal3f(0, -1, 0);
            glVertex3fv2fv(p[0], t[9]); glVertex3fv2fv(p[1], t[8]);
            glVertex3fv2fv(p[5], t[11]); glVertex3fv2fv(p[4], t[10]);
        }
        glEnd();
        if (bottom != top) glBindTexture(GL_TEXTURE_2D, bottom);
        glBegin(GL_QUADS);
        {
            glNormal3f(0, +1, 0);
            glVertex3fv2fv(p[2], t[9]); glVertex3fv2fv(p[3], t[8]);
            glVertex3fv2fv(p[7], t[11]); glVertex3fv2fv(p[6], t[10]);
        }
        glEnd();
    }

};


/// Extended OpenGL commands/snippets.
struct SGL {

    static void glPrintEnum(const char* name, int getenum)
    {
        int i;
        glGetIntegerv(getenum, &i);
        printf("%s = %i\n", name, i);
    }

    // Texture Bindings.

    static unsigned int glBindTexture3D(unsigned int texnamevar, bool min_linear, bool mag_linear, bool repeat_x, bool repeat_y, bool repeat_z, int WIDTH, int HEIGHT, int DEPTH, void* texture_ptr)
    {
        if (texnamevar == 0) {
            glGenTextures(1, &texnamevar);
        }
        glBindTexture(GL_TEXTURE_3D, texnamevar);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, min_linear ? GL_LINEAR : GL_NEAREST);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, mag_linear ? GL_LINEAR : GL_NEAREST);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, repeat_x ? GL_REPEAT : GL_CLAMP);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, repeat_y ? GL_REPEAT : GL_CLAMP);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, repeat_z ? GL_REPEAT : GL_CLAMP);
        if (!true && min_linear) glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB8, WIDTH, HEIGHT, DEPTH, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_ptr);
        return texnamevar;
    }

    static unsigned int glBindTexture2D(unsigned int texnamevar, bool min_linear, bool mag_linear, bool repeat_x, bool repeat_y, int WIDTH, int HEIGHT, int DEPTH, void* texture_ptr)
    {
        if (texnamevar == 0) {
            glGenTextures(1, &texnamevar);
        }
        glBindTexture(GL_TEXTURE_2D, texnamevar);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_linear ? GL_LINEAR : GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_linear ? GL_LINEAR : GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeat_x ? GL_REPEAT : GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeat_y ? GL_REPEAT : GL_CLAMP);
        if (DEPTH == 3) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, WIDTH, HEIGHT, 0, GL_BGR, GL_UNSIGNED_BYTE, texture_ptr);
        } else {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WIDTH, HEIGHT, 0, GL_BGRA, GL_UNSIGNED_BYTE, texture_ptr);
        }
        return texnamevar;
    }

    static unsigned int glBindTextureMipmap2D(unsigned int texnamevar, bool min_linear, bool mag_linear, bool repeat_x, bool repeat_y, int WIDTH, int HEIGHT, void* texture_ptr, GLenum internalformat = GL_RGB8, GLenum format = GL_BGR, GLenum dataformat = GL_UNSIGNED_BYTE)
    {
        if (texnamevar == 0) {
            glGenTextures(1, &texnamevar);
        }
        glBindTexture(GL_TEXTURE_2D, texnamevar);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_linear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_linear ? GL_LINEAR : GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeat_x ? GL_REPEAT : GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeat_y ? GL_REPEAT : GL_CLAMP);
        if (min_linear) glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        glTexImage2D(GL_TEXTURE_2D, 0, internalformat, WIDTH, HEIGHT, 0, format, dataformat, texture_ptr);
        return texnamevar;
    }

    /* Pushing and Poping different Projection-Matrices:
     * Note that the current Matrix-Mode is saved and
     * stays the same after calling the projection-macros (most often MODELVIEW).
     */

    static void glPushPerspectiveProjection(float fovdegrees = 90, float near = 0.07f, float far = 1500.0f)
    {
        glPushAttrib(GL_DEPTH_BUFFER_BIT); /* push glDepthFunc */
        glPushAttrib(GL_TRANSFORM_BIT);
            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
            GLint view[4];
            glGetIntegerv(GL_VIEWPORT, view);
            gluPerspective(fovdegrees, GLdouble(view[2]) / GLdouble(view[3]), near, far);
            glDepthFunc(GL_LEQUAL);
        glPopAttrib();
    }

    static void glPushOrthoProjection(float left = 0, float right = 1, float bottom = 0, float top = 1, float near = -500, float far = 500)
    {
        glPushAttrib(GL_DEPTH_BUFFER_BIT); /* push glDepthFunc */
        glPushAttrib(GL_TRANSFORM_BIT);
            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
            glOrtho(left, right, bottom, top, -500.0, 500.0);
            glDepthFunc(GL_GEQUAL);
        glPopAttrib();
    }

    static void glPushOrthoProjectionViewport(float near = -500, float far = 500)
    {
        glPushAttrib(GL_DEPTH_BUFFER_BIT); /* push glDepthFunc */
        glPushAttrib(GL_TRANSFORM_BIT);
            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
            GLint view[4];
            glGetIntegerv(GL_VIEWPORT, view);
            glOrtho(view[0], view[2], view[1], view[3], near, far);
            glDepthFunc(GL_GEQUAL);
        glPopAttrib();
    }

    static void glPopProjection()
    {
        glPushAttrib(GL_TRANSFORM_BIT);
            glMatrixMode(GL_PROJECTION);
            glPopMatrix();
        glPopAttrib();
        glPopAttrib(); /* pop glDepthFunc */
    }

    // Special Screen Effects

    static void glAccumBlur(float alpha)
    {
        glAccum(GL_MULT, alpha); /* accum = accum * factor */
        glAccum(GL_ACCUM, 1.0f-alpha); /* accum = accum + color * factor */
        glAccum(GL_RETURN, 1.0f); /* color = accum * factor */
    }


    static void glAccumBlurInverse(float alpha)
    {
        glAccum(GL_MULT, alpha); /* accum = accum * factor */
        glAccum(GL_ACCUM, 1.0f-alpha); /* accum = accum + color * factor */
        glAccum(GL_MULT, -1.0f); glAccum(GL_ADD, 1.0f); /* inversion */
        glAccum(GL_RETURN, 1.0f); /* color = accum * factor */
        glAccum(GL_ADD, -1.0f); glAccum(GL_MULT, -1.0f); /* inversion */
    }

    // Shader related.

    /// Background plain colored
    static void glUseProgram_bkplaincolor() {
        glEnable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glDisable(GL_FOG);
        glDisable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.1f);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    /// Background plain textured
    static void glUseProgram_bkplaintexture() {
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glDisable(GL_FOG);
        glDisable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.1f);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    /// Background additive textured
    static void glUseProgram_bkaddtexture() {
        glEnable(GL_CULL_FACE);
        glEnable(GL_TEXTURE_2D);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_FOG);
        glDisable(GL_LIGHTING);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    }

    /// Foreground plain colored
    static void glUseProgram_fgplaincolor() {
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_FOG);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
    }

    /// Foreground lit colored
    static void glUseProgram_fglitcolor() {
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_FOG);
        glEnable(GL_LIGHTING);
        glEnable(GL_NORMALIZE);
        glDisable(GL_TEXTURE_2D);
    }

    /// Foreground plain textured
    static void glUseProgram_fgplaintexture() {
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);
        glDisable(GL_FOG);
        glDisable(GL_LIGHTING);
    }

    /// Foreground lit textured
    static void glUseProgram_fglittexture() {
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_FOG);
        glEnable(GL_LIGHTING);
        glDisable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.4f);
    }

    /// Foreground lit textured 3d
    static void glUseProgram_fglittexture3d() {
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_3D);
        glEnable(GL_FOG);
        glEnable(GL_LIGHTING);
        glDisable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.4f);
    }

    /// Foreground additive textured
    static void glUseProgram_fgaddtexture() {
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDisable(GL_FOG);
        glDisable(GL_LIGHTING);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glDepthMask(GL_FALSE);
    }

    /// Foreground additive colored
    static void glUseProgram_fgaddcolor() {
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_FOG);
        glDisable(GL_LIGHTING);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glDepthMask(GL_FALSE);
    }

    static GLenum glCompileProgram(char* vertexshader, char* fragmentshader, std::ostream& str) {
        GLenum program = glCreateProgramObjectARB();

        GLenum vs = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
        const GLcharARB* vs_source = vertexshader;
        glShaderSourceARB(vs, 1, &vs_source, NULL);
        glCompileShaderARB(vs);
        glAttachObjectARB(program, vs);

        {
            int length;
            glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &length);
            char* log = new char[length];
            glGetShaderInfoLog(vs, length, &length, log);
            str << "Vertex Shader Log:\n" << log << "\n";
            delete log;
        }

        GLenum fs = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
        const GLcharARB* fs_source = fragmentshader;
        glShaderSourceARB(fs, 1, &fs_source, NULL);
        glCompileShaderARB(fs);
        glAttachObjectARB(program, fs);

        {
            int length;
            glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &length);
            char* log = new char[length];
            glGetShaderInfoLog(fs, length, &length, log);
            str << "Fragment Shader Log:\n" << log << "\n";
            delete log;
        }

        glLinkProgramARB(program);
        // Use unique progam.
        //glUseProgramObjectARB(postprocess);
        // Back to normal.
        //glUseProgramObjectARB(0);
        return program;
    }

    /**
     * Billboarding
     * Extracts and inverts the rotational part of the MODELVIEW_MATRIX
     * by embedding the transposed upper 3x3 into an otherwise
     * identity 4x4 matrix - assumes that no scaling was applied.
     * The result can be glMultMatrix'ed in to eliminate rotations
     * for billboarding.
     */
    static void glGetTransposeInverseRotationMatrix(float* invm16fv)
    {
        float _m_[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, _m_);
        float _n_[] = { /* = m Transposed = m Inverted */
            _m_[0], _m_[4], _m_[ 8],  0,
            _m_[1], _m_[5], _m_[ 9],  0,
            _m_[2], _m_[6], _m_[10],  0,
                 0,      0,       0,  1
        };
        memcpy(invm16fv, _n_, sizeof(float)*16);
    }

    /**
     * Extracts and inverts the translational part of the MODELVIEW_MATRIX
     * by embedding the inverted lower 1x3 translational vector
     * into an otherwise identity 4x4 matrix.
     */
    static void glGetInverseTranslationMatrix(float* invm16fv)
    {
        float _m_[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, _m_);
        float _n_[] = { /* = m Transposed = m Inverted */
            1, 0, 0,  0,
            0, 1, 0,  0,
            0, 0, 1,  0,
           -_m_[12],-_m_[13],-_m_[14],  1
        };
        memcpy(invm16fv, _n_, sizeof(float)*16);
    }

    /// Multiplies a quaternion rotation into the current matrix.
    static void glRotateq(float* quat4fv) \
    {
        float x = -(quat4fv)[0];
        float y = -(quat4fv)[1];
        float z = -(quat4fv)[2];
        float w =  (quat4fv)[3];
        float x2 = x*x;
        float y2 = y*y;
        float z2 = z*z;
        float m[] = {
            1 - 2 * y2 - 2 * z2, 2 * x * y - 2 * w*z, 2 * x * z + 2 * w*y, 0,
            2 * x * y + 2 * w*z, 1 - 2 * x2 - 2 * z2, 2 * y * z - 2 * w*x, 0,
            2 * x * z - 2 * w*y, 2 * y * z + 2 * w*x, 1 - 2 * x2 - 2 * y2, 0,
            0, 0, 0, 1
        };
        glMultMatrixf(m);
    }

};


#endif	/* _SNIPPETSGL_H */

