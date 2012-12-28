/* 
 * File:   Console.h
 * Author: Benjamin Pickhardt
 * Home:   hackcraft.de
 *
 * Created on 9. Juni 2012, 10:26
 */

#ifndef CONSOLE_H
#define	CONSOLE_H

class Console;

#include "de/hackcraft/psi3d/GLF.h"

#include "de/hackcraft/opengl/GL.h"

// memset, memmove
#include <string.h>

/**
 * Simple console buffer which makes use of the
 * instant font rendering class.
 */
class Console {
    char* buffer;
    int size;
    int width;
    int cursor;
    
public:
    /**
     *
     * @param size
     * @param width
     */
    Console(int size, int width) {
        char* buffer = (char*) malloc(sizeof(char) * size);
        memset(buffer, 0, sizeof(char) * size);
        init(buffer, size, width);
    }

    ~Console() {
        free(this->buffer);
    }

    /**
     *
     * @param itself
     * @param buffer
     * @param buffersize
     * @param bufferwidth
     */
    void init(char* buffer, int buffersize, int bufferwidth) {
        this->buffer = buffer;
        this->size = buffersize;
        this->width = bufferwidth;
        this->cursor = 0;
    }

    /**
     *
     * @param itself
     */
    void draw() {
        glDrawConsole(buffer, size, width, cursor);
    }

    /**
     *
     * @param buffer
     */
    void print(const char* buffer) {
        bnprint(this->buffer, size, width, &cursor, buffer);
    }
    
    int getSize() {
        return size;
    }
    
    int getHeight() {
        int height = size / width;
        return height;
    }
    
    int getWidth() {
        return width;
    }
    
public:    

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
};

#endif	/* CONSOLE_H */

