/* 
 * File:   GLS.h
 * Author: Benjamin Pickhardt
 * Home:   hackcraft.de
 *
 * Created on September 15, 2008, 12:54 PM
 */

#ifndef _SNIPPETSGL_H
#define	_SNIPPETSGL_H

#include "de/hackcraft/psi3d/macros.h"

// memcpy
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>

/** Extended OpenGL commands/snippets. */
struct GLS {

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

    static void glPushPerspectiveProjection(float fovdegrees = 90, float znear = 0.07f, float zfar = 1500.0f)
    {
        glPushAttrib(GL_DEPTH_BUFFER_BIT); /* push glDepthFunc */
        glPushAttrib(GL_TRANSFORM_BIT);
            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
            GLint view[4];
            glGetIntegerv(GL_VIEWPORT, view);
            gluPerspective(fovdegrees, GLdouble(view[2]) / GLdouble(view[3]), znear, zfar);
            glDepthFunc(GL_LEQUAL);
        glPopAttrib();
    }

    static void glPushOrthoProjection(float left = 0, float right = 1, float bottom = 0, float top = 1, float znear = -500, float zfar = 500)
    {
        glPushAttrib(GL_DEPTH_BUFFER_BIT); /* push glDepthFunc */
        glPushAttrib(GL_TRANSFORM_BIT);
            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
            glOrtho(left, right, bottom, top, znear, zfar);
            glDepthFunc(GL_GEQUAL);
        glPopAttrib();
    }

    static void glPushOrthoProjectionViewport(float znear = -500, float zfar = 500)
    {
        glPushAttrib(GL_DEPTH_BUFFER_BIT); /* push glDepthFunc */
        glPushAttrib(GL_TRANSFORM_BIT);
            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
            GLint view[4];
            glGetIntegerv(GL_VIEWPORT, view);
            glOrtho(view[0], view[2], view[1], view[3], znear, zfar);
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

    /** Background plain colored */
    static void glUseProgram_bkplaincolor() {
        glEnable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glDisable(GL_FOG);
        glDisable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.1f);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    /** Background plain textured */
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

    /** Background additive textured */
    static void glUseProgram_bkaddtexture() {
        glEnable(GL_CULL_FACE);
        glEnable(GL_TEXTURE_2D);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_FOG);
        glDisable(GL_LIGHTING);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    }

    /** Foreground plain colored */
    static void glUseProgram_fgplaincolor() {
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_FOG);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
    }

    /** Foreground lit colored */
    static void glUseProgram_fglitcolor() {
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_FOG);
        glEnable(GL_LIGHTING);
        glEnable(GL_NORMALIZE);
        glDisable(GL_TEXTURE_2D);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    /** Foreground plain textured */
    static void glUseProgram_fgplaintexture() {
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);
        glDisable(GL_FOG);
        glDisable(GL_LIGHTING);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    /** Foreground lit textured */
    static void glUseProgram_fglittexture() {
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_FOG);
        glEnable(GL_LIGHTING);
        glDisable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.4f);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    /** Foreground lit textured 3d */
    static void glUseProgram_fglittexture3d() {
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_3D);
        glEnable(GL_FOG);
        glEnable(GL_LIGHTING);
        glDisable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.4f);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    /** Foreground additive textured */
    static void glUseProgram_fgaddtexture() {
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDisable(GL_FOG);
        glDisable(GL_LIGHTING);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glDepthMask(GL_FALSE);
    }

    /** Foreground additive colored */
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
            //str << "Vertex Shader Log:\n" << log << "\n";
            delete[] log;
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
            //str << "Fragment Shader Log:\n" << log << "\n";
            delete[] log;
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

    /** Multiplies a quaternion rotation into the current matrix. */
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

