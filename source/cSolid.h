/* 
 * File:     cSolid.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on 28. November 2010, 13:48
 */

#ifndef CSOLID_H
#define	CSOLID_H

/**
 * ~[0,1] normalized solid materials.
 */
struct cSolid {
    enum {
        RED, GREEN, BLUE, ALPHA, BUMPMAP, SHININESS
    };

    static void stone_lava(float x, float y, float z, float* color, unsigned char seed = 131);
    static void concrete_cracked(float x, float y, float z, float* color, unsigned char seed = 131);

    static void camo_urban(float x, float y, float z, float* color, unsigned char seed = 131);
    static void camo_wood(float x, float y, float z, float* color, unsigned char seed = 131);
    static void camo_desert(float x, float y, float z, float* color, unsigned char seed = 131);
    static void camo_snow(float x, float y, float z, float* color, unsigned char seed = 131);
    static void camo_rust(float x, float y, float z, float* color, unsigned char seed = 131);

    static void skin_gecko(float x, float y, float z, float* color, unsigned char seed = 131);
    static void flesh_organix(float x, float y, float z, float* color, unsigned char seed = 131);

    static void metal_damast(float x, float y, float z, float* color, unsigned char seed = 131);
    static void metal_rust(float x, float y, float z, float* color, unsigned char seed = 131);
    static void metal_sheets(float x, float y, float z, float* color, unsigned char seed = 131);

    static void star_nebula(float x, float y, float z, float* color, unsigned char seed = 131);

    static void planet_cloud(float x, float y, float z, float* color, unsigned char seed = 131);
    static void planet_ground(float x, float y, float z, float* color);

};

#endif	/* CSOLID_H */

