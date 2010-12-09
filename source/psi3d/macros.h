/**
 * File:   macros.h
 * Author: Benjamin Pickhardt
 * Home:   hackcraft.de
 *
 * Created on March 11, 2008, 3:04 PM
 */

#ifndef _MACROS_H
#define	_MACROS_H

#include "ctrl.h"
#include "math3d.h"
//#include "snippetsgl.h"
//#include "noise.h"
//#include "instfont.h"

// Not a number: NaN
// gcc supplies a NAN which is float but that may not be available everywhere.
// Division by zero may produce segfaults on non x86 architectures.
#include <math.h>
#ifdef NAN
#define float_NAN  ((float)NAN)
#else
#define float_NAN  ({ const float nan = 0.0f/0.0f; nan; })
#endif

// Commandline parsing: Argument traversal, parsing and error detection

// Compares char string b to a given argument name b.
// eg. argument name "--help" matches given string "-h" and "--help".
#define argcmp(p, b)  ( (strcmp(p,b)==0) || (b[0]=='-' && b[1]==p[2] && b[2]=='\0') )

// Checks wether args[i] matches a given argument name.
#define arg_i(p)      argcmp(p, args[i])

// Increments i and checks wether there are still enough arguments.
#define advance_i()   { i++; if (i > argc) { return 1; } }


// Misc.

#define NATO_Alphabet(letter) \
({ \
    char* name[] = { \
        "Alpha", "Bravo", "Charlie", "Delta", "Echo", "Foxtrot", \
        "Golf", "Hotel", "India", "Juliett", "Kilo", "Lima", "Mike", \
        "November", "Oscar", "Papa", "Quibec", "Romeo", "Sierra", \
        "Tango", "Uniform", "Victor", "Whiskey", "X-Ray", "Yankee", "Zulu" \
    }; \
    name[letter]; \
})


#define getSystemHour(hour1f) \
{ \
  time_t t = time(NULL); \
  tm *l = localtime(&t); \
  hour1f = (float) (l->tm_hour + l->tm_min / 60.0 + l->tm_sec / 3600.0); \
}

/*
#define generateTexture3D(WIDTH, HEIGHT, DEPTH, texture_ptr,  d_func_xyz, r_func_dxyz, g_func_dxyz, b_func_dxyz, sx, sy, sz) \
{ \
    unsigned char* t = texture_ptr; \
    loopi(WIDTH) { loopj(HEIGHT) { loopk(DEPTH) { \
        unsigned char delta = (unsigned char) d_func_xyz(sx*i, sy*j, sz*k); \
        (*t++) = r_func_dxyz(delta, sx*i, sy*j, sz*k); \
        (*t++) = g_func_dxyz(delta, sx*i, sy*j, sz*k); \
        (*t++) = b_func_dxyz(delta, sx*i, sy*j, sz*k); \
    }}}; \
}

#define deltaFunc_NoisyPlasma(i,j,k)   ( 160 + (rand()%30) + 60*(0.5 + 0.5 * sin((9*i+3*j+20)*0.017453) * sin((9*j+3*k+60)*0.017453) * sin((9*k+3*i+80)*0.017453) ) )
#define ColorFunc_Delta(delta, x,y,z)  (delta)
*/

#endif	/* _MACROS_H */

