/* 
 * File:     cLandscape.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on 30. November 2010, 18:25
 */

#ifndef CLANDSCAPE_H
#define	CLANDSCAPE_H


/**
 * ~[0,1] normalized landscape materials.
 */
struct cLandscape {

    enum {
        RED, GRN, BLU, BUMP
    };

    static void land_rockies(float x, float y, float z, float* color, unsigned char seed = 131);

    /**
     * ~[0,1] normalized desert sand dune landscape.
     */
    static void land_dunes(float x, float y, float z, float* color, unsigned char seed = 131);

    /**
     * ~[0,1] normalized desert sand dune landscape.
     */
    static void land_dunes_red(float x, float y, float z, float* color, unsigned char seed = 131);

    /**
     * ~[0,1] normalized lavatic landscape.
     */
    static void land_lava(float x, float y, float z, float* color, unsigned char seed = 131);

    static void land_grass(float x, float y, float z, float* color, unsigned char seed = 131);

    static void land_snow(float x, float y, float z, float* color, unsigned char seed = 131);

    static void land_dirt(float x, float y, float z, float* color, unsigned char seed = 131);

    static void decoration_spikes(float x, float y, float z, float* color, unsigned char seed = 131);

    static void decoration_stones(float x, float y, float z, float* color, unsigned char seed = 131);

    static void mashup_lava(float x, float y, float z, float* color, unsigned char seed = 131);

    static void experimental_continents(float x, float y, float z, float* color, unsigned char seed = 131);

};

#endif	/* CLANDSCAPE_H */

