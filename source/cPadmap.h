/**
 * File:     cPadmap.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on 31. August 2010, 18:29
 * Contained source roots back to cBuilding.h
 */

#ifndef CPADMAP_H
#define	CPADMAP_H

#include "cObject.h"

class cPadmap : public cObject {
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
    float mapscale[3];
public:
    cPadmap(float x, float y, float z);
    /// Calculate Height of the xz position.
    inline float getHeight(float x, float z);
    virtual float constrainParticle(float* worldpos, float radius = 0.0f, float* localpos = NULL, cObject* enactor = NULL);
    virtual void drawSolid();
};


#endif	/* CPADMAP_H */

