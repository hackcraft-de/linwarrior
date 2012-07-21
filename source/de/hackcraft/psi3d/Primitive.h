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

#include "de/hackcraft/opengl/GL.h"

/** 
 * Provides extended GL rendering primitives.
 */
struct Primitive {

    static inline void glVertex3fv2f(float* pnt, float u, float v) {
        GL::glTexCoord2f(u,v);
        GL::glVertex3fv(pnt);
    }

    static inline void glVertex3fv2fv(float* pnt, float* tex) {
        GL::glTexCoord2f(tex[0], tex[1]);
        GL::glVertex3fv(pnt);
    }

    static inline void glVertex3fv3fv(float* pnt, float* tex) {
        GL::glTexCoord3fv(tex);
        GL::glVertex3fv(pnt);
    }

    static void glBalloid() {
        GL::glEnable(GL_DEPTH_TEST);
        GL::glEnable(GL_CULL_FACE);
        GL::glDisable(GL_LIGHTING);
        GL::glDisable(GL_TEXTURE_2D);

        int steps = 2;
        double delta = 2.0f / (float) steps;

        float c[3];

        // xy
        loopk (2) {
            float dir = (k & 1) ? -1 : +1;
            GL::glColor4f(0.9f,0.5f,0.5f,1.0f);
            //GL::glColor4f(t==0?1:0.5,t==1?1:0.5,t==2?1:0.5,1);

            double b = -1;
            loopj(steps) {
                double b0 = b * -dir;
                double b1 = (b + delta) * -dir;
                double a = -1;
                GL::glBegin(GL_TRIANGLE_STRIP);
                loopi(steps+1) {

                    float inv0 = 1.0f / (float) sqrtf(a*a+b0*b0+1.0f);
                    float inv1 = 1.0f / (float) sqrtf(a*a+b1*b1+1.0f);
                    float v0[] = { (float) a*inv0, (float) b0*inv0, (float) dir*inv0 };
                    float v1[] = { (float) a*inv1, (float) b1*inv1, (float) dir*inv1 };


                    GL::glNormal3fv(v0);
                    //transformV3(v0,c);
                    GL::glColor3fv(c);
                    GL::glVertex3fv(v0);

                    GL::glNormal3fv(v1);
                    //transformV3(v1,c);
                    GL::glColor3fv(c);
                    GL::glVertex3fv(v1);

                    a += delta;
                }
                GL::glEnd();
                b += delta;
            }
        }

        // xz
        loopk (2) {
            float dir = (k & 1) ? -1 : +1;
            GL::glColor4f(0.5f,0.9f,0.5f,1.0f);
            //GL::glColor4f(t==0?1:0.5,t==1?1:0.5,t==2?1:0.5,1);

            double b = -1;
            loopj(steps) {
                double b0 = b * dir;
                double b1 = (b + delta) * dir;
                double a = -1;
                GL::glBegin(GL_TRIANGLE_STRIP);
                loopi(steps+1) {

                    float inv0 = 1.0f / (float) sqrtf(a*a+b0*b0+1.0f);
                    float inv1 = 1.0f / (float) sqrtf(a*a+b1*b1+1.0f);
                    float v0[] = { (float) a*inv0, (float) dir*inv0, (float) b0*inv0 };
                    float v1[] = { (float) a*inv1, (float) dir*inv1, (float) b1*inv1 };

                    GL::glNormal3fv(v0);
                    //transformV3(v0,c);
                    GL::glColor3fv(c);
                    GL::glVertex3fv(v0);

                    GL::glNormal3fv(v1);
                    //transformV3(v1,c);
                    GL::glColor3fv(c);
                    GL::glVertex3fv(v1);

                    a += delta;
                }
                GL::glEnd();
                b += delta;
            }
        }

        // yz
        loopk (2) {
            float dir = (k & 1) ? -1 : +1;
            GL::glColor4f(0.5f,0.5f,0.9f,1.0f);
            //GL::glColor4f(t==0?1:0.5,t==1?1:0.5,t==2?1:0.5,1);

            double b = -1;
            loopj(steps) {
                double b0 = b * -dir;
                double b1 = (b + delta) * -dir;
                double a = -1;
                GL::glBegin(GL_TRIANGLE_STRIP);
                loopi(steps+1) {

                    //a = fma(a,1,0);
                    float inv0 = 1.0f / (float) sqrtf(a*a+b0*b0+1.0f);
                    float inv1 = 1.0f / (float) sqrtf(a*a+b1*b1+1.0f);
                    float v0[] = { (float) dir*inv0, (float) a*inv0, (float) b0*inv0 };
                    float v1[] = { (float) dir*inv1, (float) a*inv1, (float) b1*inv1 };

                    GL::glNormal3fv(v0);
                    //transformV3(v0,c);
                    GL::glColor3fv(c);
                    GL::glVertex3fv(v0);

                    GL::glNormal3fv(v1);
                    //transformV3(v1,c);
                    GL::glColor3fv(c);
                    GL::glVertex3fv(v1);

                    a += delta;
                }
                GL::glEnd();
                b += delta;
            }
        }
    }

    static inline void glAxis(float l = 1.0f) {
        GL::glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT);
        {
            GL::glDisable(GL_TEXTURE_2D);
            GL::glDisable(GL_LIGHTING);
            GL::glBegin(GL_LINES);
            {
                GL::glColor4f(1,0,0,1);
                GL::glVertex3f(0,0,0);
                GL::glVertex3f(l,0,0);
                GL::glColor4f(0,1,0,1);
                GL::glVertex3f(0,0,0);
                GL::glVertex3f(0,l,0);
                GL::glColor4f(0,0,1,1);
                GL::glVertex3f(0,0,0);
                GL::glVertex3f(0,0,l);
            }
            GL::glEnd();
        }
        GL::glPopAttrib();
    }

    static inline void glLineSquare(float border = 0) {
        float lo = border;
        float hi = 1 - border;
        GL::glBegin(GL_LINE_STRIP);
        {
            GL::glVertex3f(lo, lo, 0);
            GL::glVertex3f(hi, lo, 0);
            GL::glVertex3f(hi, hi, 0);
            GL::glVertex3f(lo, hi, 0);
            GL::glVertex3f(lo, lo, 0);
        }
        GL::glEnd();
    }

    static inline void glDisk(int segments = 9, float radius = 1.0f) {
        int n = 2 + (segments);
        float step = 2*M_PI / (float) n;
        float a = 0.0f;
        GL::glBegin(GL_TRIANGLE_FAN);
        GL::glNormal3f(0,0,1);
        loopi(n) {
            GL::glTexCoord3f(0.45+0.499*sin(a), 0.5+0.499*cos(a), 0);
            GL::glVertex3f(sin(a)*(radius), cos(a)*(radius), 0);
            a += step;
        }
        GL::glEnd();
    }

    static inline void glXCenteredTextureSquare() {
        GL::glBegin(GL_QUADS);
        {
            GL::glTexCoord2i(1, 0);
            GL::glVertex3f(+0.5f, 0.0f, 0);
            GL::glTexCoord2i(0, 0);
            GL::glVertex3f(-0.5f, 0.0f, 0);
            GL::glTexCoord2i(0, 1);
            GL::glVertex3f(-0.5f, +1.0f, 0);
            GL::glTexCoord2i(1, 1);
            GL::glVertex3f(+0.5f, +1.0f, 0);
        }
        GL::glEnd();
    }

    static inline void glXYCenteredTextureSquare(float s = 0.5f) {
        GL::glBegin(GL_QUADS);
        {
            GL::glTexCoord2i(1, 0);
            GL::glVertex3f(+s, -s, 0);
            GL::glTexCoord2i(0, 0);
            GL::glVertex3f(-s, -s, 0);
            GL::glTexCoord2i(0, 1);
            GL::glVertex3f(-s, +s, 0);
            GL::glTexCoord2i(1, 1);
            GL::glVertex3f(+s, +s, 0);
        }
        GL::glEnd();
    }

    static inline void glUnitCylinder(float segments = 9) {
        int n = 2 + (segments);
        float step = 1.0f / (float) n;
        float anglestep = 2.0f * M_PI * step;
        float angle = 0.0f;
        GL::glBegin(GL_TRIANGLE_STRIP);
        for (int i=0; i<=n; i++) {
            GL::glTexCoord3f(0.5f+0.499f*sin(angle), 0.999f, 0.5f+0.499*cos(angle));
            GL::glVertex3f(sin(angle), +1.0f, cos(angle));
            GL::glTexCoord3f(0.5f+0.499f*sin(angle), 0.001f, 0.5f+0.499*cos(angle));
            GL::glVertex3f(sin(angle), +0.0f, cos(angle));
            angle += anglestep;
        }
        GL::glEnd();
    }

    static inline void glCenterUnitCylinder(float segments = 9, float vrepeat = 1) {
        float step = 1.0f / (float) segments;
        float anglestep = 2.0f * M_PI *step;
        float angle = 0.0f;
        GL::glBegin(GL_TRIANGLE_STRIP);
        for (int i=0; i<=segments; i++) {
            GL::glTexCoord3f(0.5f+0.499f*sin(angle), 0.999f * vrepeat, 0.5f+0.499f*cos(angle));
            GL::glVertex3f(sin(angle), +1.0f, cos(angle));
            GL::glTexCoord3f(0.5f+0.499f*sin(angle), 0.001f, 0.5f+0.499f*cos(angle));
            GL::glVertex3f(sin(angle), -1.0f, cos(angle));
            angle += anglestep;
        };
        GL::glEnd();
    }

    static inline void glUnitBlock()
    {
        GL::GLfloat p[][3] = {
            {0,0,0}, {1,0,0}, {1,1,0}, {0,1,0},
            {0,0,1}, {1,0,1}, {1,1,1}, {0,1,1}
        };
        const float a = 0.01f;
        const float b = 0.99f;
        GL::GLfloat t[][3] = {
            {a,a,a}, {b,a,a}, {b,b,a}, {a,b,a},
            {a,a,b}, {b,a,b}, {b,b,b}, {a,b,b}
        };
        GL::glBegin(GL_QUADS);
        {
            /* front */
            GL::glNormal3f(0, 0, -1);
            glVertex3fv3fv(p[1], t[1]); glVertex3fv3fv(p[0], t[0]);
            glVertex3fv3fv(p[3], t[3]); glVertex3fv3fv(p[2], t[2]);
            /* back */
            GL::glNormal3f(0, 0, +1);
            glVertex3fv3fv(p[4], t[4]); glVertex3fv3fv(p[5], t[5]);
            glVertex3fv3fv(p[6], t[6]); glVertex3fv3fv(p[7], t[7]);
            /* top */
            GL::glNormal3f(0, -1, 0);
            glVertex3fv3fv(p[0], t[0]); glVertex3fv3fv(p[1], t[1]);
            glVertex3fv3fv(p[5], t[5]); glVertex3fv3fv(p[4], t[4]);
            /* bottom */
            GL::glNormal3f(0, +1, 0);
            glVertex3fv3fv(p[2], t[2]); glVertex3fv3fv(p[3], t[3]);
            glVertex3fv3fv(p[7], t[7]); glVertex3fv3fv(p[6], t[6]);
            /* left */
            GL::glNormal3f(+1, 0, 0);
            glVertex3fv3fv(p[0], t[0]); glVertex3fv3fv(p[4], t[4]);
            glVertex3fv3fv(p[7], t[7]); glVertex3fv3fv(p[3], t[3]);
            /* right */
            GL::glNormal3f(-1, 0, 0);
            glVertex3fv3fv(p[5], t[5]); glVertex3fv3fv(p[1], t[1]);
            glVertex3fv3fv(p[2], t[2]); glVertex3fv3fv(p[6], t[6]);
        }
        GL::glEnd();
    }

    static inline void glCenterUnitBlock() {
        GL::GLfloat p[][3] = {
            {-1,-1,-1}, {+1,-1,-1}, {+1,+1,-1}, {-1,+1,-1},
            {-1,-1,+1}, {+1,-1,+1}, {+1,+1,+1}, {-1,+1,+1}
        };
        const float a = 0.01f;
        const float b = 0.99f;
        GL::GLfloat t[][3] = {
            {a,a,a}, {b,a,a}, {b,b,a}, {a,b,a},
            {a,a,b}, {b,a,b}, {b,b,b}, {a,b,b}
        };
        GL::glBegin(GL_QUADS);
        {
            /* front */
            GL::glNormal3f(0, 0, -1);
            glVertex3fv3fv(p[1], t[1]); glVertex3fv3fv(p[0], t[0]);
            glVertex3fv3fv(p[3], t[3]); glVertex3fv3fv(p[2], t[2]);
            /* back */
            GL::glNormal3f(0, 0, +1);
            glVertex3fv3fv(p[4], t[4]); glVertex3fv3fv(p[5], t[5]);
            glVertex3fv3fv(p[6], t[6]); glVertex3fv3fv(p[7], t[7]);
            /* top */
            GL::glNormal3f(0, -1, 0);
            glVertex3fv3fv(p[0], t[0]); glVertex3fv3fv(p[1], t[1]);
            glVertex3fv3fv(p[5], t[5]); glVertex3fv3fv(p[4], t[4]);
            /* bottom */
            GL::glNormal3f(0, +1, 0);
            glVertex3fv3fv(p[2], t[2]); glVertex3fv3fv(p[3], t[3]);
            glVertex3fv3fv(p[7], t[7]); glVertex3fv3fv(p[6], t[6]);
            /* left */
            GL::glNormal3f(+1, 0, 0);
            glVertex3fv3fv(p[0], t[0]); glVertex3fv3fv(p[4], t[4]);
            glVertex3fv3fv(p[7], t[7]); glVertex3fv3fv(p[3], t[3]);
            /* right */
            GL::glNormal3f(-1, 0, 0);
            glVertex3fv3fv(p[5], t[5]); glVertex3fv3fv(p[1], t[1]);
            glVertex3fv3fv(p[2], t[2]); glVertex3fv3fv(p[6], t[6]);
        }
        GL::glEnd();
    }

    static inline void glBlockFlat(float width, float height, float depth, int left, int right, int front, int back, int top, int bottom) {
        float a = 0, b = 0, c = 0;
        float x = width;
        float y = height;
        float z = depth;
        GL::GLfloat p[][3] = {
            {a,b,c}, {x,b,c}, {x,y,c}, {a,y,c},
            {a,b,z}, {x,b,z}, {x,y,z}, {a,y,z}
        };
        GL::GLfloat t[][2] = {
            {a,b}, {x,b}, {x,y}, {a,y},
            {c,b}, {z,b}, {z,y}, {c,y},
            {a,c}, {x,c}, {x,z}, {a,z},
        };
        GL::glBindTexture(GL_TEXTURE_2D, front);
        GL::glBegin(GL_QUADS);
        {
            GL::glNormal3f(0, 0, -1);
            glVertex3fv2fv(p[1], t[1]); glVertex3fv2fv(p[0], t[0]);
            glVertex3fv2fv(p[3], t[3]); glVertex3fv2fv(p[2], t[2]);
        }
        GL::glEnd();
        if (back != front) GL::glBindTexture(GL_TEXTURE_2D, back);
        GL::glBegin(GL_QUADS);
        {
            GL::glNormal3f(0, 0, +1);
            glVertex3fv2fv(p[4], t[1]); glVertex3fv2fv(p[5], t[0]);
            glVertex3fv2fv(p[6], t[3]); glVertex3fv2fv(p[7], t[2]);
        }
        GL::glEnd();
        if (left != back) GL::glBindTexture(GL_TEXTURE_2D, left);
        GL::glBegin(GL_QUADS);
        {
            GL::glNormal3f(+1, 0, 0);
            glVertex3fv2fv(p[0], t[5]); glVertex3fv2fv(p[4], t[4]);
            glVertex3fv2fv(p[7], t[7]); glVertex3fv2fv(p[3], t[6]);
        }
        GL::glEnd();
        if (right != left) GL::glBindTexture(GL_TEXTURE_2D, right);
        GL::glBegin(GL_QUADS);
        {
            GL::glNormal3f(-1, 0, 0);
            glVertex3fv2fv(p[5], t[5]); glVertex3fv2fv(p[1], t[4]);
            glVertex3fv2fv(p[2], t[7]); glVertex3fv2fv(p[6], t[6]);
        }
        GL::glEnd();
        if (top != right) GL::glBindTexture(GL_TEXTURE_2D, top);
        GL::glBegin(GL_QUADS);
        {
            GL::glNormal3f(0, -1, 0);
            glVertex3fv2fv(p[0], t[9]); glVertex3fv2fv(p[1], t[8]);
            glVertex3fv2fv(p[5], t[11]); glVertex3fv2fv(p[4], t[10]);
        }
        GL::glEnd();
        if (bottom != top) GL::glBindTexture(GL_TEXTURE_2D, bottom);
        GL::glBegin(GL_QUADS);
        {
            GL::glNormal3f(0, +1, 0);
            glVertex3fv2fv(p[2], t[9]); glVertex3fv2fv(p[3], t[8]);
            glVertex3fv2fv(p[7], t[11]); glVertex3fv2fv(p[6], t[10]);
        }
        GL::glEnd();
    }

    static inline void glStar() {
        GL::glBegin(GL_QUADS);
        {
            // _
            GL::glTexCoord2i(1, 0);
            GL::glVertex3f(+0.5f, 0.0f, -0.0f);
            GL::glTexCoord2i(0, 0);
            GL::glVertex3f(-0.5f, 0.0f, -0.0f);
            GL::glTexCoord2i(0, 1);
            GL::glVertex3f(-0.5f, +1.0f, -0.0f);
            GL::glTexCoord2i(1, 1);
            GL::glVertex3f(+0.5f, +1.0f, -0.0f);
            // /
            GL::glTexCoord2i(1, 0);
            GL::glVertex3f(+0.25f, 0.0f, +0.43f);
            GL::glTexCoord2i(0, 0);
            GL::glVertex3f(-0.25f, 0.0f, -0.43f);
            GL::glTexCoord2i(0, 1);
            GL::glVertex3f(-0.25f, +1.0f, -0.43f);
            GL::glTexCoord2i(1, 1);
            GL::glVertex3f(+0.25f, +1.0f, +0.43f);
            // / inverted
            GL::glTexCoord2i(1, 0);
            GL::glVertex3f(+0.25f, 0.0f, -0.43f);
            GL::glTexCoord2i(0, 0);
            GL::glVertex3f(-0.25f, 0.0f, +0.43f);
            GL::glTexCoord2i(0, 1);
            GL::glVertex3f(-0.25f, +1.0f, +0.43f);
            GL::glTexCoord2i(1, 1);
            GL::glVertex3f(+0.25f, +1.0f, -0.43f);
        }
        GL::glEnd();
    }

    static inline void glBrush() {
        const float f = 0.20f;
        const float s0 = +0.0000f * f;
        const float c0 = +1.0000f * f;
        const float s1 = +0.8660f * f;
        const float c1 = -0.5000f * f;
        const float s2 = -0.8660f * f;
        const float c2 = -0.5000f * f;
        GL::glBegin(GL_QUADS);
        {
            // _
            GL::glTexCoord2i(1, 0);
            GL::glVertex3f(+0.5f + s0, 0.0f, -0.0f + c0);
            GL::glTexCoord2i(0, 0);
            GL::glVertex3f(-0.5f + s0, 0.0f, -0.0f + c0);
            GL::glTexCoord2i(0, 1);
            GL::glVertex3f(-0.5f - s0, +1.0f, -0.0f - c0);
            GL::glTexCoord2i(1, 1);
            GL::glVertex3f(+0.5f - s0, +1.0f, -0.0f - c0);
            // /
            GL::glTexCoord2i(1, 0);
            GL::glVertex3f(+0.25f + s1, 0.0f, +0.43f + c1);
            GL::glTexCoord2i(0, 0);
            GL::glVertex3f(-0.25f + s1, 0.0f, -0.43f + c1);
            GL::glTexCoord2i(0, 1);
            GL::glVertex3f(-0.25f - s1, +1.0f, -0.43f - c1);
            GL::glTexCoord2i(1, 1);
            GL::glVertex3f(+0.25f - s1, +1.0f, +0.43f - c1);
            // / inverted
            GL::glTexCoord2i(1, 0);
            GL::glVertex3f(+0.25f + s2, 0.0f, -0.43f + c2);
            GL::glTexCoord2i(0, 0);
            GL::glVertex3f(-0.25f + s2, 0.0f, +0.43f + c2);
            GL::glTexCoord2i(0, 1);
            GL::glVertex3f(-0.25f - s2, +1.0f, +0.43f - c2);
            GL::glTexCoord2i(1, 1);
            GL::glVertex3f(+0.25f - s2, +1.0f, -0.43f - c2);
        }
        GL::glEnd();
    }

    static inline void glSharp() {
        const float f = 0.10f;
        const float s0 = +0.0000f * f;
        const float c0 = +1.0000f * f;
        const float s1 = +0.8660f * f;
        const float c1 = -0.5000f * f;
        const float s2 = -0.8660f * f;
        const float c2 = -0.5000f * f;
        GL::glBegin(GL_QUADS);
        {
            // _
            GL::glTexCoord2i(1, 0);
            GL::glVertex3f(+0.5f + s0, 0.0f, -0.0f + c0);
            GL::glTexCoord2i(0, 0);
            GL::glVertex3f(-0.5f + s0, 0.0f, -0.0f + c0);
            GL::glTexCoord2i(0, 1);
            GL::glVertex3f(-0.5f + s0, +1.0f, -0.0f + c0);
            GL::glTexCoord2i(1, 1);
            GL::glVertex3f(+0.5f + s0, +1.0f, -0.0f + c0);
            // /
            GL::glTexCoord2i(1, 0);
            GL::glVertex3f(+0.25f + s1, 0.0f, +0.43f + c1);
            GL::glTexCoord2i(0, 0);
            GL::glVertex3f(-0.25f + s1, 0.0f, -0.43f + c1);
            GL::glTexCoord2i(0, 1);
            GL::glVertex3f(-0.25f + s1, +1.0f, -0.43f + c1);
            GL::glTexCoord2i(1, 1);
            GL::glVertex3f(+0.25f + s1, +1.0f, +0.43f + c1);
            // / inverted
            GL::glTexCoord2i(1, 0);
            GL::glVertex3f(+0.25f + s2, 0.0f, -0.43f + c2);
            GL::glTexCoord2i(0, 0);
            GL::glVertex3f(-0.25f + s2, 0.0f, +0.43f + c2);
            GL::glTexCoord2i(0, 1);
            GL::glVertex3f(-0.25f + s2, +1.0f, +0.43f + c2);
            GL::glTexCoord2i(1, 1);
            GL::glVertex3f(+0.25f + s2, +1.0f, -0.43f + c2);
        }
        GL::glEnd();
    }
};

#endif	/* PRIMITIVE_H */

