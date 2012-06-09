/* 
 * File:   Facade.h
 * Author: Benjamin Pickhardt
 * Home:   hackcraft.de
 *
 * Created on June 3, 2011, 9:59 PM
 */

#ifndef FACADE_H
#define	FACADE_H

/**
 * Class that provides methods for generating facade textures.
 */
struct Facade {

    static void getVStrings(float x, float y, float* color4f, unsigned char seed = 131);

    static void getHStrings(float x, float y, float* color4f, unsigned char seed = 131);

    static void getInterrior(float x, float y, float* color4fv, unsigned char seed = 131);

    static void getExterrior(float x, float y, float* color4fv, unsigned char seed = 131);

    static void getConcrete(float x, float y, float z, float* color4fv, unsigned char seed = 131);

    static void getFacade(float x, float y, int gx, int gy, float age, float* c3f, unsigned char seed = 131);
    
private:
    
    static unsigned char generateFrame(float* dims4x4, float (*sums5)[4], unsigned char seed);
};

#endif	/* FACADE_H */

