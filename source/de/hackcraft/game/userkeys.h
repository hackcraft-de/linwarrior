/**
 * File:     userkeys.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * User changeable keys which are
 * compiled into the program.
 */

#ifndef _USERKEYS_H
#define _USERKEYS_H

#define _TEXTURE_KEY SDLK_z
#define _NIGHTVISION_KEY SDLK_u
#define _WIREFRAME_KEY SDLK_o
#define _PAUSE_KEY SDLK_p


static int map_zedwise[] = {// impact - default model
    0, 1, 2, 3,
    4, 5, 6, 7,
    8, 9, 10, 11,
    12, 13, 14, 15,
    16, 17, 18, 19,
    20, 21, 22, 23,
    24, 25, 26, 27,
    28, 29, 30, 31
};

static int map_clockwise[] = {// thunderstrike - problems with axes.
    3, 1, 2, 0,
    4, 6, 5, 7, // swapped: R L
    10, 11, 8, 9, // swapped: start&select for j1b&j2b
    12, 13, 14, 15,
    16, 17, 18, 19,
    20, 21, 22, 23,
    24, 25, 26, 27,
    28, 29, 30, 31
};

#endif

