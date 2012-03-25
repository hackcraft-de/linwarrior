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

#include "de/hackcraft/world/Subsystem.h"

#include <vector>

class PhysicsSystem : public Subsystem {
public:
    PhysicsSystem();
    PhysicsSystem(const PhysicsSystem& orig);
    virtual ~PhysicsSystem();
private:
    std::vector<rCollider> colliders;
    std::vector<rTraceable> traceables;
};

#endif	/* PHYSICSSYSTEM_H */

