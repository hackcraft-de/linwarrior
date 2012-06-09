/* 
 * File:    rTile.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on April 9, 2012, 8:04 PM
 */

#ifndef RTILE_H
#define	RTILE_H

#include "de/hackcraft/psi3d/math3d.h"

#include "de/hackcraft/world/Component.h"
#include "de/hackcraft/world/OID.h"

#include <map>

/**
 * Models an urban street-tile of different types.
 */
class rTile : public Component {
public:
    /** Possible models of tile. */

    enum Kinds {
        KIND_CONCRETE,
        KIND_ROAD_EW,
        KIND_ROAD_NS,
        KIND_ROAD_NEWS,
        KIND_ROAD_TN,
        KIND_ROAD_TE,
        KIND_ROAD_TW,
        KIND_ROAD_TS,
        KIND_ROAD_NE,
        KIND_ROAD_NW,
        KIND_ROAD_SE,
        KIND_ROAD_SW,
        KIND_ROAD_PLAZA,

        MAX_KINDS
    };
protected:
    /** Counts number of instances. */
    static int sInstances;
    /** Contains shared texture(s) of all instances. */
    static std::map<int, long> sTextures;

public:
    /** Unique id of the instance. */
    OID id;
    /** Position of the tile. */
    vec3 pos0;
    /** This tile has the named appearance (as in Kinds-enum). */
    int tileKind;
public:

    /** Initializes a tile of a certain kind at the given tile-grid position. */
    rTile(int x, int y, int z, int kind);
    virtual void drawSolid();
};

#endif	/* RTILE_H */

