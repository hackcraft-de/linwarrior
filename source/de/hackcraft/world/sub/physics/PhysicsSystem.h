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
private:
    std::map<OID,rCollider*> colliders;
    std::map<OID,rTraceable*> traceables;
};

#endif	/* PHYSICSSYSTEM_H */

