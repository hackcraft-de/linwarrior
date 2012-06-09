/* 
 * File:   Road.h
 * Author: Benjamin Pickhardt
 * Home:   hackcraft.de
 *
 * Created on June 3, 2011, 9:38 PM
 */

#ifndef ROAD_H
#define	ROAD_H

/**
 * Provides methods for generating road textures.
 */
struct Road {

    static void getBasicRoad(float x, float y, float z, float* color, unsigned char seed = 131);

    static void getStoplineMask(float x, float y, float z, float* color, unsigned char seed = 131);

    static void getZebraMask(float x, float y, float z, float* color, unsigned char seed = 131);

    static void getBasicCrossRoad(float x, float y, float z, float* color, unsigned char seed = 131);

    static void getBasicTSRoad(float x, float y, float z, float* color, unsigned char seed = 131);

    static void getBasicTNRoad(float x, float y, float z, float* color, unsigned char seed = 131);

    static void getBasicTERoad(float x, float y, float z, float* color, unsigned char seed = 131);

    static void getBasicTWRoad(float x, float y, float z, float* color, unsigned char seed = 131);

    static void getBasicNERoad(float x, float y, float z, float* color, unsigned char seed = 131);

    static void getBasicNWRoad(float x, float y, float z, float* color, unsigned char seed = 131);

    static void getBasicSERoad(float x, float y, float z, float* color, unsigned char seed = 131);

    static void getBasicSWRoad(float x, float y, float z, float* color, unsigned char seed = 131);

};

#endif	/* ROAD_H */

