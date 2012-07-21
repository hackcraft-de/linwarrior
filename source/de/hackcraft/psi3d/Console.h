/* 
 * File:   Console.h
 * Author: Benjamin Pickhardt
 * Home:   hackcraft.de
 *
 * Created on 9. Juni 2012, 10:26
 */

#ifndef CONSOLE_H
#define	CONSOLE_H

struct Console;

#include "de/hackcraft/psi3d/GLF.h"

#include "de/hackcraft/opengl/GL.h"

// memset, memmove
#include <string.h>

/**
 * Simple console buffer which makes use of the
 * instant font rendering class.
 */
struct Console {
    char* buffer;
    int size;
    int width;
    int cursor;

    /**
     *
     * @param size
     * @param width
     * @return
     */
    static Console* Console_new(int size, int width) {
        char* buffer = (char*) malloc(sizeof(char) * size);
        memset(buffer, 0, sizeof(char) * size);
        Console* con = (Console*) malloc(sizeof(Console));
        Console_init(con, buffer, size, width);
        return con;
    }

    /**
     *
     * @param itself
     */
    static void Console_free(Console* itself) {
        free(itself->buffer);
        free(itself);
    }

    /**
     *
     * @param itself
     * @param buffer
     * @param buffersize
     * @param bufferwidth
     */
    static void Console_init(Console* itself, char* buffer, int buffersize, int bufferwidth) {
        itself->buffer = buffer;
        itself->size = buffersize;
        itself->width = bufferwidth;
        itself->cursor = 0;
    }

    /**
     *
     * @param itself
     */
    static void Console_draw(Console* itself) {
        glDrawConsole(itself->buffer, itself->size, itself->width, itself->cursor);
    }

    /**
     * Prints the given formated string into a console like string buffer.
     * Console like means that the string buffer is treated as having
     * (buffersize modulo bufferwidth) rows - each of bufferwidth characters.
     * bufferpos holds the posisition where printing into the buffer shall start
     * and where it shall start the next time when calling.
     * The function maintains newlines ('\n') and tabs ('\t') and
     * maintains buffer scrolling when the buffer is full.
     *
     * @param bufferstr
     * @param buffersize
     * @param bufferwidth
     * @param bufferpos
     * @param buffer
     */
    static void bnprint(char* bufferstr, int buffersize, int bufferwidth, int* bufferpos, const char* buffer) {
        int len = strlen(buffer);
        int p = *bufferpos;
        int i;
        for (i = 0; i < len; i++) {
            if (buffer[i] == '\n') {
                p += ((bufferwidth) - (p % (bufferwidth)));
            } else if (buffer[i] == '\t') {
                p += (5 - ((p % (bufferwidth)) % 5));
            } else {
                bufferstr[p] = buffer[i];
                p++;
            }
            if (p >= buffersize) {
                memmove(bufferstr, &bufferstr[bufferwidth], buffersize - (bufferwidth));
                memset(&bufferstr[buffersize - (bufferwidth)], 0, bufferwidth);
                p -= bufferwidth;
            }
        }
        *bufferpos = p;
    }
    
    /**
     * Draws the characters in bufferstr rowwise, with bufferwidth characters for each row.
     * buffersize is the total size of the buffer (multiples of bufferwidth would be meaningful).
     * bufferpos is the byte position of the cursor inside the console buffer, set negative for no cursor.
     *
     * @param buffer
     * @param buffersize
     * @param bufferwidth
     * @param bufferpos
     */
    static void glDrawConsole(char* buffer, int buffersize, int bufferwidth, int bufferpos) {
        unsigned char* bufferstr = (unsigned char*) (buffer);
        unsigned int* asciibinds = GLF::getFont();
        if (asciibinds == NULL) return;
        GL::glPushAttrib(GL_ENABLE_BIT);
        {
            GL::glDisable(GL_CULL_FACE);
            GL::glEnable(GL_TEXTURE_2D);
            GL::glPushMatrix();
            {
                int i, col = 0;
                for (i = 0; i < (buffersize); i++) {
                    if ((i % (bufferwidth)) == 0) {
                        GL::glTranslatef(-col, -1, 0);
                        col = 0;
                    }
                    if (bufferstr[i] != 0) GLF::glTexturedSquare(asciibinds[bufferstr[i]]);
                    if (bufferpos == i) GLF::glTexturedSquare(asciibinds[0]);
                    GL::glTranslatef(1, 0, 0);
                    col++;
                }
            }
            GL::glPopMatrix();
        }
        GL::glPopAttrib();
    }

    /**
     *
     * @param itself
     * @param buffer
     */
    static void Console_print(Console* itself, const char* buffer) {
        bnprint(itself->buffer, itself->size, itself->width, &itself->cursor, buffer);
    }
};

#endif	/* CONSOLE_H */

