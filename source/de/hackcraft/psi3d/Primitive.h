/* 
 * File:   Primitive.h
 * Author: Benjamin Pickhardt
 * Home:   hackcraft.de
 *
 * Created on June 2, 2011, 12:16 AM
 */

#ifndef PRIMITIVE_H
#define	PRIMITIVE_H

#include "de/hackcraft/psi3d/macros.h"

#include <GL/glew.h>

/** 
 * Provides extended GL rendering primitives.
 */
struct Primitive {

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
                    float v0[] = { (float) a*inv0, (float) b0*inv0, (float) dir*inv0 };
                    float v1[] = { (float) a*inv1, (float) b1*inv1, (float) dir*inv1 };


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
                    float v0[] = { (float) a*inv0, (float) dir*inv0, (float) b0*inv0 };
                    float v1[] = { (float) a*inv1, (float) dir*inv1, (float) b1*inv1 };

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
                    float v0[] = { (float) dir*inv0, (float) a*inv0, (float) b0*inv0 };
                    float v1[] = { (float) dir*inv1, (float) a*inv1, (float) b1*inv1 };

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

    static inline void glCenterUnitCylinder(float segments = 9, float vrepeat = 1) {
        float step = 1.0f / (float) segments;
        float anglestep = 2.0f * M_PI *step;
        float angle = 0.0f;
        glBegin(GL_TRIANGLE_STRIP);
        for (int i=0; i<=segments; i++) {
            glTexCoord3f(0.5f+0.499f*sin(angle), 0.999f * vrepeat, 0.5f+0.499f*cos(angle));
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

    static inline void glStar() {
        glBegin(GL_QUADS);
        {
            // _
            glTexCoord2i(1, 0);
            glVertex3f(+0.5f, 0.0f, -0.0f);
            glTexCoord2i(0, 0);
            glVertex3f(-0.5f, 0.0f, -0.0f);
            glTexCoord2i(0, 1);
            glVertex3f(-0.5f, +1.0f, -0.0f);
            glTexCoord2i(1, 1);
            glVertex3f(+0.5f, +1.0f, -0.0f);
            // /
            glTexCoord2i(1, 0);
            glVertex3f(+0.25f, 0.0f, +0.43f);
            glTexCoord2i(0, 0);
            glVertex3f(-0.25f, 0.0f, -0.43f);
            glTexCoord2i(0, 1);
            glVertex3f(-0.25f, +1.0f, -0.43f);
            glTexCoord2i(1, 1);
            glVertex3f(+0.25f, +1.0f, +0.43f);
            // / inverted
            glTexCoord2i(1, 0);
            glVertex3f(+0.25f, 0.0f, -0.43f);
            glTexCoord2i(0, 0);
            glVertex3f(-0.25f, 0.0f, +0.43f);
            glTexCoord2i(0, 1);
            glVertex3f(-0.25f, +1.0f, +0.43f);
            glTexCoord2i(1, 1);
            glVertex3f(+0.25f, +1.0f, -0.43f);
        }
        glEnd();
    }

    static inline void glBrush() {
        const float f = 0.20f;
        const float s0 = +0.0000f * f;
        const float c0 = +1.0000f * f;
        const float s1 = +0.8660f * f;
        const float c1 = -0.5000f * f;
        const float s2 = -0.8660f * f;
        const float c2 = -0.5000f * f;
        glBegin(GL_QUADS);
        {
            // _
            glTexCoord2i(1, 0);
            glVertex3f(+0.5f + s0, 0.0f, -0.0f + c0);
            glTexCoord2i(0, 0);
            glVertex3f(-0.5f + s0, 0.0f, -0.0f + c0);
            glTexCoord2i(0, 1);
            glVertex3f(-0.5f - s0, +1.0f, -0.0f - c0);
            glTexCoord2i(1, 1);
            glVertex3f(+0.5f - s0, +1.0f, -0.0f - c0);
            // /
            glTexCoord2i(1, 0);
            glVertex3f(+0.25f + s1, 0.0f, +0.43f + c1);
            glTexCoord2i(0, 0);
            glVertex3f(-0.25f + s1, 0.0f, -0.43f + c1);
            glTexCoord2i(0, 1);
            glVertex3f(-0.25f - s1, +1.0f, -0.43f - c1);
            glTexCoord2i(1, 1);
            glVertex3f(+0.25f - s1, +1.0f, +0.43f - c1);
            // / inverted
            glTexCoord2i(1, 0);
            glVertex3f(+0.25f + s2, 0.0f, -0.43f + c2);
            glTexCoord2i(0, 0);
            glVertex3f(-0.25f + s2, 0.0f, +0.43f + c2);
            glTexCoord2i(0, 1);
            glVertex3f(-0.25f - s2, +1.0f, +0.43f - c2);
            glTexCoord2i(1, 1);
            glVertex3f(+0.25f - s2, +1.0f, -0.43f - c2);
        }
        glEnd();
    }

    static inline void glSharp() {
        const float f = 0.10f;
        const float s0 = +0.0000f * f;
        const float c0 = +1.0000f * f;
        const float s1 = +0.8660f * f;
        const float c1 = -0.5000f * f;
        const float s2 = -0.8660f * f;
        const float c2 = -0.5000f * f;
        glBegin(GL_QUADS);
        {
            // _
            glTexCoord2i(1, 0);
            glVertex3f(+0.5f + s0, 0.0f, -0.0f + c0);
            glTexCoord2i(0, 0);
            glVertex3f(-0.5f + s0, 0.0f, -0.0f + c0);
            glTexCoord2i(0, 1);
            glVertex3f(-0.5f + s0, +1.0f, -0.0f + c0);
            glTexCoord2i(1, 1);
            glVertex3f(+0.5f + s0, +1.0f, -0.0f + c0);
            // /
            glTexCoord2i(1, 0);
            glVertex3f(+0.25f + s1, 0.0f, +0.43f + c1);
            glTexCoord2i(0, 0);
            glVertex3f(-0.25f + s1, 0.0f, -0.43f + c1);
            glTexCoord2i(0, 1);
            glVertex3f(-0.25f + s1, +1.0f, -0.43f + c1);
            glTexCoord2i(1, 1);
            glVertex3f(+0.25f + s1, +1.0f, +0.43f + c1);
            // / inverted
            glTexCoord2i(1, 0);
            glVertex3f(+0.25f + s2, 0.0f, -0.43f + c2);
            glTexCoord2i(0, 0);
            glVertex3f(-0.25f + s2, 0.0f, +0.43f + c2);
            glTexCoord2i(0, 1);
            glVertex3f(-0.25f + s2, +1.0f, +0.43f + c2);
            glTexCoord2i(1, 1);
            glVertex3f(+0.25f + s2, +1.0f, -0.43f + c2);
        }
        glEnd();
    }
};

#endif	/* PRIMITIVE_H */

