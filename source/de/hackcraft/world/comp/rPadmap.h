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

#include "de/hackcraft/world/Component.h"

#include "de/hackcraft/psi3d/math3d.h"

#include <vector>
#include <map>

class rPadmap : public Component {
public:
    /// Possible models of tile.

    enum Kinds {
        KIND_CONCRETE,
        KIND_ROAD_EASTWEST,
        KIND_ROAD_NORTHSOUTH,
        KIND_ROAD_JUNKTION,

        MAX_KINDS
    };
protected:
    /// Counts number of instances.
    static int sInstances;
    /// Contains shared texture(s) of all instances.
    static std::map<int, long> sTextures;
    static std::vector<int> sSpiral[2];
protected:
    int dims[2];
    float heights[16 * 16];
public: // INPUT
    vec3 mapscale;
    vec3 pos0;
    quat ori0;
public:
    rPadmap(cObject* obj);
    /// Calculate Height of the xz position.
    inline float getHeight(float x, float z);
    virtual void animate(float spf);
    virtual float constrain(float* worldpos, float radius = 0.0f, float* localpos = NULL, cObject* enactor = NULL);
    virtual void drawSolid();
};


#endif	/* CPADMAP_H */

