/**
 * File:     rPadmap.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on 31. August 2010, 18:29
 * Contained source roots back to cBuilding.h
 */

#ifndef CPADMAP_H
#define	CPADMAP_H

class rPadmap;

#include "de/hackcraft/lang/OID.h"

#include "de/hackcraft/psi3d/math3d.h"

#include "de/hackcraft/world/Component.h"

#include <vector>
#include <map>

/**
 * Enhanced heightmap model with vertical walls for gaps
 * (with 4 heights at one grid point).
 * To be used for urban areas or scifi building complexes.
 */
class rPadmap : public Component {
public: // SYSTEM
    /** Identifier for this component (all uppercase letters without leading "r"). */
    static std::string cname;
    /** A unique random number (0-9999) to identify this component. */
    static unsigned int cid;
    /** Counts number of instances. */
    static int sInstances;
    /** Contains shared texture(s) of all instances. */
    static std::map<int, long> sTextures;
    /** Experimental: steps for spiral rendering order. */
    static std::vector<int> sSpiral[2];
public:
    /** Possible models of tile. */

    enum Kinds {
        KIND_CONCRETE,
        KIND_ROAD_EASTWEST,
        KIND_ROAD_NORTHSOUTH,
        KIND_ROAD_JUNKTION,

        MAX_KINDS
    };
public:
    OID id;
    int dims[2];
    float heights[16 * 16];
public: // INPUT
    vec3 mapscale;
    vec3 pos0;
    quat ori0;
public:
    rPadmap(Entity* obj);
    /** Calculate Height of the xz position. */
    inline float getHeight(float x, float z);
    virtual void animate(float spf);
    virtual float constrain(float* worldpos, float radius = 0.0f, float* localpos = NULL, Entity* enactor = NULL);
    virtual void drawSolid();
};


#endif	/* CPADMAP_H */

