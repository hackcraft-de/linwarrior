/* 
 * File:    PhysicsSystem.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on March 22, 2012, 10:46 PM
 */


#ifndef PHYSICSSYSTEM_H
#define	PHYSICSSYSTEM_H

#include "rCollider.h"
#include "rTraceable.h"

#include "de/hackcraft/world/OID.h"
#include "de/hackcraft/world/Subsystem.h"

#include <map>

class PhysicsSystem : public Subsystem {
public:
    PhysicsSystem();
    PhysicsSystem(const PhysicsSystem& orig);
    virtual ~PhysicsSystem();

    static float* getGravity();
    
    static float getGndfriction();
    
    static float getAirdensity();
private:
    std::map<OID,rCollider*> colliders;
    std::map<OID,rTraceable*> traceables;

    /// Gravity acceleration vector [m/s²]. space = (0,0,0), earth = (0,-9.8,0).
    static vec3 mGravity;

    /// Air density in kg per cubic meter (default 1.204 kg/m3 Air at 20°C).
    static float mAirdensity;

    /// Velocity "damping" when on ground (0.14 ok).
    static float mGndfriction;
};

#endif	/* PHYSICSSYSTEM_H */

