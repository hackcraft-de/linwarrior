/* 
 * File:     cPlanetmap.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on 18. Juli 2010, 21:39
 */

#ifndef PLANETMAP_H
#define	PLANETMAP_H

class cPlanetmap;

#include "cObject.h"

#include "cLandscape.h"

/**
 * Procedural Landscape Rendering and Collision.
 */
class cPlanetmap : public cObject {
public:
    /// Instance counter.
    static int sInstances;
    /// Instance shared Textures.
    static std::vector<long> sTextures;
public:
    /// LRU Surface Cache-Tile.

    struct sPatch {
        // Number of accesses last frame.
        unsigned long touches;
        // Cache key - spatial index.
        unsigned long key;
        // Cached Surface Data.
        //float heightcolor[1024*1024*4];
        float heightcolor[512 * 512 * 4];
    };
    /// LRU Surface Cache.
    std::map<unsigned long, sPatch*> patches;
public:
    cPlanetmap();
    /// Calculate Height and Color of the xz position.
    inline void getHeight(float x, float z, float* const color);
    /// Retrieves Height and Color of the xz position and calculates on demand.
    inline void getCachedHeight(float x, float z, float* const color);
    /// Re-adjust particle position by making multiple downward-hemispherical checks.
    virtual float constrainParticle(float* worldpos, float radius, float* localpos, cObject* enactor);
    /// Draw Landscape surrounding the current camera position.
    virtual void drawSolid();
    /// Draw Decals surrounding the current camera position.
    virtual void drawEffect();
};

#endif	/* PLANETMAP_H */

