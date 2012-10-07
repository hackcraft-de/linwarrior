/* 
 * File:    rTile.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on April 9, 2012, 8:04 PM
 */

#ifndef RTILE_H
#define	RTILE_H

class rTile;

#include "de/hackcraft/lang/OID.h"

#include "de/hackcraft/psi3d/math3d.h"

#include "de/hackcraft/world/Component.h"

#include <map>

class Logger;

/**
 * Models an urban street-tile of different types.
 */
class rTile : public Component {
private:
    static Logger* logger;
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
    
    /** Load vertex and fragment programs and/or return loaded program. */
    unsigned int loadMaterial();
    
    /** Generate, upload (and optionally save) all textures of all road kinds. */
    void uploadTextures(int resolution, bool save, std::string basepath, unsigned char seed);

    /** Generate, upload (and optionally save) a texture of a certain kind. */
    void uploadTexture(int kind, int resolution, bool save, std::string basepath, std::string name, unsigned char seed);
    
    /** Get texel for a texture of a certain kind. */
    void getTexelOfKind(int kind, float x, float y, float* color, unsigned char seed);
};

#endif	/* RTILE_H */

