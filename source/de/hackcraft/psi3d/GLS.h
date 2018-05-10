/* 
 * File:   GLS.h
 * Author: Benjamin Pickhardt
 * Home:   hackcraft.de
 *
 * Created on September 15, 2008, 12:54 PM
 */

#ifndef _SNIPPETSGL_H
#define	_SNIPPETSGL_H

#include "de/hackcraft/opengl/GL.h"
#include "de/hackcraft/opengl/GLU.h"

// memcpy
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ostream>

/** Extended OpenGL commands/snippets. */
struct GLS {

    static void glPrintEnum(const char* name, int getenum)
    {
        int i;
        GL::glGetIntegerv(getenum, &i);
        printf("%s = %i\n", name, i);
    }

    // Texture Bindings.

    static unsigned int glBindTexture3D(unsigned int texnamevar, bool min_linear, bool mag_linear, bool repeat_x, bool repeat_y, bool repeat_z, int WIDTH, int HEIGHT, int DEPTH, void* texture_ptr)
    {
        if (texnamevar == 0) {
            GL::glGenTextures(1, &texnamevar);
        }
        GL::glBindTexture(GL_TEXTURE_3D, texnamevar);
        GL::glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, min_linear ? GL_LINEAR : GL_NEAREST);
        GL::glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, mag_linear ? GL_LINEAR : GL_NEAREST);
        GL::glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, repeat_x ? GL_REPEAT : GL_CLAMP);
        GL::glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, repeat_y ? GL_REPEAT : GL_CLAMP);
        GL::glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, repeat_z ? GL_REPEAT : GL_CLAMP);
        if (!true && min_linear) GL::glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        GL::glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB8, WIDTH, HEIGHT, DEPTH, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_ptr);
        return texnamevar;
    }

    static unsigned int glBindTexture2D(unsigned int texnamevar, bool min_linear, bool mag_linear, bool repeat_x, bool repeat_y, int WIDTH, int HEIGHT, int DEPTH, void* texture_ptr)
    {
        if (texnamevar == 0) {
            GL::glGenTextures(1, &texnamevar);
        }
        GL::glBindTexture(GL_TEXTURE_2D, texnamevar);
        GL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_linear ? GL_LINEAR : GL_NEAREST);
        GL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_linear ? GL_LINEAR : GL_NEAREST);
        GL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeat_x ? GL_REPEAT : GL_CLAMP);
        GL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeat_y ? GL_REPEAT : GL_CLAMP);
        if (DEPTH == 3) {
            GL::glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, WIDTH, HEIGHT, 0, GL_BGR, GL_UNSIGNED_BYTE, texture_ptr);
        } else {
            GL::glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WIDTH, HEIGHT, 0, GL_BGRA, GL_UNSIGNED_BYTE, texture_ptr);
        }
        return texnamevar;
    }

    static unsigned int glBindTextureMipmap2D(unsigned int texnamevar, bool min_linear, bool mag_linear, bool repeat_x, bool repeat_y, int WIDTH, int HEIGHT, void* texture_ptr, GL::GLenum internalformat = GL_RGB8, GL::GLenum format = GL_BGR, GL::GLenum dataformat = GL_UNSIGNED_BYTE)
    {
        if (texnamevar == 0) {
            GL::glGenTextures(1, &texnamevar);
        }
        GL::glBindTexture(GL_TEXTURE_2D, texnamevar);
        GL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_linear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST);
        GL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_linear ? GL_LINEAR : GL_NEAREST);
        GL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeat_x ? GL_REPEAT : GL_CLAMP);
        GL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeat_y ? GL_REPEAT : GL_CLAMP);
        if (min_linear) GL::glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        GL::glTexImage2D(GL_TEXTURE_2D, 0, internalformat, WIDTH, HEIGHT, 0, format, dataformat, texture_ptr);
        return texnamevar;
    }

    /* Pushing and Poping different Projection-Matrices:
     * Note that the current Matrix-Mode is saved and
     * stays the same after calling the projection-macros (most often MODELVIEW).
     */

    static void glPushPerspectiveProjection(float fovdegrees = 90, float znear = 0.07f, float zfar = 1500.0f)
    {
        GL::glPushAttrib(GL_DEPTH_BUFFER_BIT); /* push glDepthFunc */
        GL::glPushAttrib(GL_TRANSFORM_BIT);
            GL::glMatrixMode(GL_PROJECTION);
            GL::glPushMatrix();
            GL::glLoadIdentity();
            GL::GLint view[4];
            GL::glGetIntegerv(GL_VIEWPORT, view);
            GLU::gluPerspective(fovdegrees, GL::GLdouble(view[2]) / GL::GLdouble(view[3]), znear, zfar);
            GL::glDepthFunc(GL_LEQUAL);
        GL::glPopAttrib();
    }

    static void glPushOrthoProjection(float left = 0, float right = 1, float bottom = 0, float top = 1, float znear = -500, float zfar = 500)
    {
        GL::glPushAttrib(GL_DEPTH_BUFFER_BIT); /* push glDepthFunc */
        GL::glPushAttrib(GL_TRANSFORM_BIT);
            GL::glMatrixMode(GL_PROJECTION);
            GL::glPushMatrix();
            GL::glLoadIdentity();
            GL::glOrtho(left, right, bottom, top, znear, zfar);
            GL::glDepthFunc(GL_GEQUAL);
        GL::glPopAttrib();
    }

    static void glPushOrthoProjectionViewport(float znear = -500, float zfar = 500)
    {
        GL::glPushAttrib(GL_DEPTH_BUFFER_BIT); /* push glDepthFunc */
        GL::glPushAttrib(GL_TRANSFORM_BIT);
            GL::glMatrixMode(GL_PROJECTION);
            GL::glPushMatrix();
            GL::glLoadIdentity();
            GL::GLint view[4];
            GL::glGetIntegerv(GL_VIEWPORT, view);
            GL::glOrtho(view[0], view[2], view[1], view[3], znear, zfar);
            GL::glDepthFunc(GL_GEQUAL);
        GL::glPopAttrib();
    }

    static void glPopProjection()
    {
        GL::glPushAttrib(GL_TRANSFORM_BIT);
            GL::glMatrixMode(GL_PROJECTION);
            GL::glPopMatrix();
        GL::glPopAttrib();
        GL::glPopAttrib(); /* pop glDepthFunc */
    }

    // Special Screen Effects

    static void glAccumBlur(float alpha)
    {
        GL::glAccum(GL_MULT, alpha); /* accum = accum * factor */
        GL::glAccum(GL_ACCUM, 1.0f-alpha); /* accum = accum + color * factor */
        GL::glAccum(GL_RETURN, 1.0f); /* color = accum * factor */
    }


    static void glAccumBlurInverse(float alpha)
    {
        GL::glAccum(GL_MULT, alpha); /* accum = accum * factor */
        GL::glAccum(GL_ACCUM, 1.0f-alpha); /* accum = accum + color * factor */
        GL::glAccum(GL_MULT, -1.0f); GL::glAccum(GL_ADD, 1.0f); /* inversion */
        GL::glAccum(GL_RETURN, 1.0f); /* color = accum * factor */
        GL::glAccum(GL_ADD, -1.0f); GL::glAccum(GL_MULT, -1.0f); /* inversion */
    }

    // Shader related.

    /** Background plain colored */
    static void glUseProgram_bkplaincolor() {
        GL::glEnable(GL_CULL_FACE);
        GL::glDisable(GL_DEPTH_TEST);
        GL::glDisable(GL_LIGHTING);
        GL::glDisable(GL_FOG);
        GL::glDisable(GL_ALPHA_TEST);
        GL::glAlphaFunc(GL_GREATER, 0.1f);
        GL::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    /** Background plain textured */
    static void glUseProgram_bkplaintexture() {
        GL::glEnable(GL_TEXTURE_2D);
        GL::glEnable(GL_CULL_FACE);
        GL::glDisable(GL_DEPTH_TEST);
        GL::glDisable(GL_LIGHTING);
        GL::glDisable(GL_FOG);
        GL::glDisable(GL_ALPHA_TEST);
        GL::glAlphaFunc(GL_GREATER, 0.1f);
        GL::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    /** Background additive textured */
    static void glUseProgram_bkaddtexture() {
        GL::glEnable(GL_CULL_FACE);
        GL::glEnable(GL_TEXTURE_2D);
        GL::glDisable(GL_DEPTH_TEST);
        GL::glDisable(GL_FOG);
        GL::glDisable(GL_LIGHTING);
        GL::glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    }

    /** Foreground plain colored */
    static void glUseProgram_fgplaincolor() {
        GL::glEnable(GL_CULL_FACE);
        GL::glEnable(GL_DEPTH_TEST);
        GL::glDisable(GL_FOG);
        GL::glDisable(GL_LIGHTING);
        GL::glDisable(GL_TEXTURE_2D);
    }

    /** Foreground lit colored */
    static void glUseProgram_fglitcolor() {
        GL::glEnable(GL_CULL_FACE);
        GL::glEnable(GL_DEPTH_TEST);
        GL::glEnable(GL_FOG);
        GL::glEnable(GL_LIGHTING);
        GL::glEnable(GL_NORMALIZE);
        GL::glDisable(GL_TEXTURE_2D);
        GL::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    /** Foreground plain textured */
    static void glUseProgram_fgplaintexture() {
        GL::glEnable(GL_CULL_FACE);
        GL::glEnable(GL_DEPTH_TEST);
        GL::glEnable(GL_TEXTURE_2D);
        GL::glDisable(GL_FOG);
        GL::glDisable(GL_LIGHTING);
        GL::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    /** Foreground lit textured */
    static void glUseProgram_fglittexture() {
        GL::glEnable(GL_CULL_FACE);
        GL::glEnable(GL_DEPTH_TEST);
        GL::glEnable(GL_TEXTURE_2D);
        GL::glEnable(GL_FOG);
        GL::glEnable(GL_LIGHTING);
        GL::glDisable(GL_ALPHA_TEST);
        GL::glAlphaFunc(GL_GREATER, 0.4f);
        GL::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    /** Foreground lit textured 3d */
    static void glUseProgram_fglittexture3d() {
        GL::glEnable(GL_CULL_FACE);
        GL::glEnable(GL_DEPTH_TEST);
        GL::glEnable(GL_TEXTURE_3D);
        GL::glEnable(GL_FOG);
        GL::glEnable(GL_LIGHTING);
        GL::glDisable(GL_ALPHA_TEST);
        GL::glAlphaFunc(GL_GREATER, 0.4f);
        GL::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    /** Foreground additive textured */
    static void glUseProgram_fgaddtexture() {
        GL::glEnable(GL_TEXTURE_2D);
        GL::glEnable(GL_DEPTH_TEST);
        GL::glDisable(GL_CULL_FACE);
        GL::glDisable(GL_FOG);
        GL::glDisable(GL_LIGHTING);
        GL::glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        GL::glDepthMask(GL_FALSE);
    }

    /** Foreground additive colored */
    static void glUseProgram_fgaddcolor() {
        GL::glEnable(GL_DEPTH_TEST);
        GL::glDisable(GL_CULL_FACE);
        GL::glDisable(GL_TEXTURE_2D);
        GL::glDisable(GL_FOG);
        GL::glDisable(GL_LIGHTING);
        GL::glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        GL::glDepthMask(GL_FALSE);
    }

    static GL::GLenum glCompileProgram(char* vertexshader, char* fragmentshader, std::ostream& str) {
        GL::GLenum program = GL::glCreateProgramObjectARB();

        GL::GLenum vs = GL::glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
        const GL::GLcharARB* vs_source = vertexshader;
        GL::glShaderSourceARB(vs, 1, &vs_source, NULL);
        GL::glCompileShaderARB(vs);
        GL::glAttachObjectARB(program, vs);

        {
            int length;
            GL::glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &length);
            char* log = new char[length];
            GL::glGetShaderInfoLog(vs, length, &length, log);
            str << "Vertex Shader Log:\n" << log << "\n";
            delete[] log;
        }

        GL::GLenum fs = GL::glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
        const GL::GLcharARB* fs_source = fragmentshader;
        GL::glShaderSourceARB(fs, 1, &fs_source, NULL);
        GL::glCompileShaderARB(fs);
        GL::glAttachObjectARB(program, fs);

        {
            int length;
            GL::glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &length);
            char* log = new char[length];
            GL::glGetShaderInfoLog(fs, length, &length, log);
            str << "Fragment Shader Log:\n" << log << "\n";
            delete[] log;
        }

        GL::glLinkProgramARB(program);
        // Use unique progam.
        //GL::glUseProgramObjectARB(postprocess);
        // Back to normal.
        //GL::glUseProgramObjectARB(0);
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
        GL::glGetFloatv(GL_MODELVIEW_MATRIX, _m_);
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
        GL::glGetFloatv(GL_MODELVIEW_MATRIX, _m_);
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
        GL::glMultMatrixf(m);
    }

};


#endif	/* _SNIPPETSGL_H */

