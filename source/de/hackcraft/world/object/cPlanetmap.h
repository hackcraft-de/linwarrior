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

#include "de/hackcraft/proc/Landscape.h"

#include "cTree.h"

#define USE_UNORDERED_MAP
#if defined(USE_UNORDERED_MAP)
#include <tr1/unordered_map>
#define maptype std::tr1::unordered_map
#else
#include <map>
#define maptype std::map
#endif

// Amount of cached patches.
#define PLANETMAP_CACHESIZE 512

// Size of cache patch (8 => [256 * 256])
#define PLANETMAP_TILESIZE 7

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

    struct Growth {
        enum Rendertype {
            BILLBOARD,
            CROSS,
            STAR,
            TRIANGLE
        };
        long texture;
        float size;
        Rendertype rendertype;
        Growth(long texture, float size, Rendertype rendertype) {
            this->texture = texture;
            this->size = size;
            this->rendertype = rendertype;
        }
    };
    static std::vector<cPlanetmap::Growth*> sGrowth;

public:
    /// Surface Modifiction

    struct sMod {
        float pos[3];
        float range;
        float height;
        enum Modtype {
            MODTYPE_FLATSQUARE,
            MODTYPE_SMOOTHSQUARE,
            MODTYPE_SMOOTHROUND,
            MODTYPE_CRATER
        };
        Modtype type;
        sMod() { type = MODTYPE_SMOOTHROUND; pos[0] = pos[1] = pos[2] = 0.0f; range = 10.0f; height = 0.0f; }
        float getModifiedHeight(float x, float y, float h);
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
        long normal[(1UL << PLANETMAP_TILESIZE)*(1UL << PLANETMAP_TILESIZE)];
    };
    /// LRU Surface Cache.
    maptype<unsigned long, sPatch*> patches;
    /// Tree template for drawing of all trees.
    cTree* tree;
public:
    cPlanetmap();
    /// Remove cached data - enforce recalculation (enforce cache miss).
    void invalidateCache();
    /// Calculate Height and Color of the xz position.
    void getHeight(float x, float z, float* const color);
    /// Retrieves Height and Color of the xz position and calculates on demand.
    void getCachedHeight(float x, float z, float* const color);
    /// Re-adjust particle position by making multiple downward-hemispherical checks.
    virtual float constrain(float* worldpos, float radius, float* localpos, cObject* enactor);
    /// Animate foliage.
    virtual void animate(float spf);
    /// Draw Landscape surrounding the current camera position.
    virtual void drawSolid();
    /// Draw Decals surrounding the current camera position.
    virtual void drawEffect();
protected:
    void drawBillboardPlant(float x, float h, float z, float scale, float* unrotateMatrix);
    void drawStarPlant(float x, float h, float z, float scale);
    void drawTrianglePlant(float x, float h, float z, float scale);
    void drawCrossPlant(float x, float h, float z, float scale);
};

#endif	/* PLANETMAP_H */

