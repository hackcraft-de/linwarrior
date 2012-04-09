/**
 * File:     rBuilding.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on March 28, 2008, 10:23 PM
 */

#ifndef BUILDING_H
#define BUILDING_H

#include "de/hackcraft/psi3d/math3d.h"

#include "de/hackcraft/world/Component.h"
#include "de/hackcraft/world/OID.h"

#include <map>

/**
 * Models a building of varying size.
 */
class rBuilding : public Component {
protected:
    static int sInstances;
    static std::map<int, long> sTextures;
public:
    /// Unique id of the instance.
    OID id;
    /// Position of the building.
    vec3 pos0;
    /// Orientation of the building.
    quat ori0;
    /// Dimensions of the building.
    float buildingRooms[3];
    /// Derived from building dimensions.
    float radius;
public:
    rBuilding(int x, int y, int z, int rooms_x, int rooms_y, int rooms_z);
    virtual float constrain(float* worldpos, float radius = 0.0f, float* localpos = NULL, Entity* enactor = NULL);
    virtual void drawSolid();
};


#endif
