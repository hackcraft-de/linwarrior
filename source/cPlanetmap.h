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

// Amount of cached patches.
#define PLANETMAP_CACHESIZE 64

// Size of cache patch (8 => [256 * 256])
#define PLANETMAP_TILESIZE 8

// Submetre detail (2 => 4th of a metre).
#define PLANETMAP_DIVISIONS 2

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
    /// Surface Modifiction
    struct sMod {
        float pos[3];
        float range;
        float height;
    };
    /// All effective surface modifictions.
    std::vector<sMod*> mods;

    /// LRU Surface Cache-Tile.
    struct sPatch {
        // Number of accesses last frame.
        unsigned long touches;
        // Cache key - spatial index.
        unsigned long key;
        // Cached Surface Data.
        OID heightcolor[(1UL << PLANETMAP_TILESIZE)*(1UL << PLANETMAP_TILESIZE)];
    };
    /// LRU Surface Cache.
    std::map<unsigned long, sPatch*> patches;
public:
    cPlanetmap();
    /// Remove cached data - enforce recalculation (enforce cache miss).
    void invalidateCache();
    /// Calculate Height and Color of the xz position.
    void getHeight(float x, float z, float* const color);
    /// Retrieves Height and Color of the xz position and calculates on demand.
    void getCachedHeight(float x, float z, float* const color);
    /// Re-adjust particle position by making multiple downward-hemispherical checks.
    virtual float constrainParticle(float* worldpos, float radius, float* localpos, cObject* enactor);
    /// Draw Landscape surrounding the current camera position.
    virtual void drawSolid();
    /// Draw Decals surrounding the current camera position.
    virtual void drawEffect();
};

#endif	/* PLANETMAP_H */

