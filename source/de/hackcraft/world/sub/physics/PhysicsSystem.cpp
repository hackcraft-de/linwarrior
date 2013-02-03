#include "PhysicsSystem.h"

#include "de/hackcraft/log/Logger.h"

#include "de/hackcraft/world/World.h"

#include "de/hackcraft/world/sub/physics/rCollider.h"
#include "de/hackcraft/world/sub/physics/rSpringlink.h"
#include "de/hackcraft/world/sub/physics/rTraceable.h"


PhysicsSystem* PhysicsSystem::instance = NULL;

Logger* PhysicsSystem::logger = Logger::getLogger("de.hackcraft.world.sub.physics.PhysicsSystem");


PhysicsSystem* PhysicsSystem::getInstance() {
    return instance;
}


PhysicsSystem::PhysicsSystem() {
    // Earth : -9.80665f
    // Mars  : -3.69f
    gravity[0] = 0.0f;
    gravity[1] = -9.80665f;
    gravity[2] = 0.0f;

    // Helium    : 0.1785f
    // Earth-Air : 1.204f
    // Water     : 1000.0f
    airdensity = 1.204f;

    // ??? : 0.13f
    gndfriction = 0.13f;

    instance = this;
}


PhysicsSystem::~PhysicsSystem() {
}


void PhysicsSystem::add(rCollider* collider) {
    colliders.push_back(collider);
    collidersIndex[collider->getId()] = collider;
}


void PhysicsSystem::add(rSpringlink* springlink) {
    springlinks.push_back(springlink);
    springlinksIndex[springlink->getId()] = springlink;
}

void PhysicsSystem::add(rTraceable* traceable) {
    traceables.push_back(traceable);
    traceablesIndex[traceable->getId()] = traceable;
}


float* PhysicsSystem::getGravity() {
    return gravity;
}


float PhysicsSystem::getGndfriction() {
    return gndfriction;
}


float PhysicsSystem::getAirdensity() {
    return airdensity;
}


void PhysicsSystem::animateObjects() {
    
    float spf = World::getInstance()->getTiming()->getSPF();
    
    for (rCollider* collider : colliders) {
        collider->prebind();
        collider->animate(spf);
        collider->postbind();
    }
    
    for (rSpringlink* springlink : springlinks) {
        springlink->prebind();
        springlink->animate(spf);
        springlink->postbind();
    }
    
    for (rTraceable* traceable : traceables) {
        traceable->prebind();
        traceable->animate(spf);
        traceable->postbind();
    }
}


float PhysicsSystem::constrainParticle(Entity* ex, float* worldpos, float radius) {
    
    float maxdepth = 0.0f;
    
    // TODO: Cluster and replace all-loop with geospatial-indexed-loop.
    for (rCollider* collider : colliders) {
        
        if (collider->object != ex) {

            float depth = collider->constrain(worldpos, radius, NULL, ex);
            //maxdepth = fmax(maxdepth, depth);
            maxdepth += depth;
        }
    }
    return maxdepth;
}

