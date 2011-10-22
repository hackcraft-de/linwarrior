/* 
 * File:   Ambient.h
 * Author: Benjamin Pickhardt
 * Home:   hackcraft.de
 *
 * Created on October 22, 2011, 6:15 PM
 */

#ifndef AMBIENT_H
#define	AMBIENT_H

struct Ambient {
        static void galaxy(float x, float y, float z, float* color, float scale);
        static void clouds(float x, float y, float z, float* color);
        static void sky(float x, float y, float z, float* color, float hour = 24.00f);
};

#endif	/* AMBIENT_H */

