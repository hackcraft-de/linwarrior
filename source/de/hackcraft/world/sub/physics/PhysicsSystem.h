/* 
 * File:    PhysicsSystem.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on March 22, 2012, 10:46 PM
 */

#ifndef PHYSICSSYSTEM_H
#define	PHYSICSSYSTEM_H

class PhysicsSystem;

#include "de/hackcraft/lang/OID.h"

#include "de/hackcraft/psi3d/math3d.h"

#include "de/hackcraft/world/Subsystem.h"

#include <map>

class rCollider;
class rTraceable;

class PhysicsSystem : public Subsystem {
public:
    PhysicsSystem();
    virtual ~PhysicsSystem();

    float* getGravity();
    
    float getGndfriction();
    
    float getAirdensity();
    
    static PhysicsSystem* getInstance();
private:
    std::map<OID,rCollider*> colliders;
    std::map<OID,rTraceable*> traceables;
    
    /** Allows searching the world in a structured manner. */
    //std::unordered_map<OID, std::list<rCollider*> > geoToColliders;

    /** Gravity acceleration vector [m/s²]. space = (0,0,0), earth = (0,-9.8,0). */
    vec3 gravity;

    /** Air density in kg per cubic meter (default 1.204 kg/m3 Air at 20°C). */
    float airdensity;

    /** Velocity "damping" when on ground (0.14 ok). */
    float gndfriction;
    
    static PhysicsSystem* instance;
};

#endif	/* PHYSICSSYSTEM_H */

