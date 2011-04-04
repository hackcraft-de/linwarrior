/**
 * File:     cBuilding.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on March 28, 2008, 10:23 PM
 */

#ifndef BUILDING_H
#define BUILDING_H

class cBuilding;
struct rDamageable;

#include "cObject.h"
#include "rWeapon.h"

/**
 * Models a building of varring size.
 */
class cBuilding : public cObject {
public:
    rDamageable* damageable;
protected:
    static int sInstances;
    static std::map<int,long> sTextures;
protected:
    /// Need to re-setup base matrix for explosion (true/false)?
    bool dirtyBase;
    /// Base matrix for explosion ie. at center.
    float baseMatrix[16];
    /// Explosion to be triggered on destruction of the building.
    rWeaponExplosion explosionObject;
    /// Dimensions of the building.
    float buildingRooms[3];
public:
    cBuilding(int x, int y, int z, int rooms_x, int rooms_y, int rooms_z);
    
    virtual void damageByParticle(float* localpos, float damage, cObject* enactor = NULL);
    virtual float constrainParticle(float* worldpos, float radius = 0.0f, float* localpos = NULL, cObject* enactor = NULL);
    
    virtual void onSpawn();
    
    virtual void animate(float spf);
    virtual void transform();
    virtual void drawSolid();
    virtual void drawEffect();
};

struct cParticle;

/**
 * Models a set/cluster/field of random distributed small
 * environmental decal objects such as stones, flowers,
 * grass, dirt and debris.
 */
class cScatterfield : public cObject {
protected:
    static int sInstances;
    static std::map<int,long> sTextures;
public:
    enum CollisionType { CT_NONE, CT_CYLINDER, CT_SPHERE, CT_BOX };
    enum BillboardType { BT_NONE, BT_AROUND, BT_AXIS };
protected:
    std::list<cParticle*> decalParticles;
public:
    cScatterfield(float x, float y, float z, float radius, float density);
    virtual void drawEffect();
};


/**
 * Models an urban street-tile of different types.
 */
class cTile : public cObject {
public:
    /// Possible models of tile.
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
    /// Counts number of instances.
    static int sInstances;
    /// Contains shared texture(s) of all instances.
    static std::map<int,long> sTextures;
    
protected:
    /// This tile has the named appeareance (as in Kinds-enum).
    int tileKind;
public:

    /// Initialises a tile of a certain kind at the given tile-grid position.
    cTile(int x, int y, int z, int kind);
    virtual void drawSolid();
};


#endif
