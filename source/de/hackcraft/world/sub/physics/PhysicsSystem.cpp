#include "PhysicsSystem.h"

#include "de/hackcraft/world/sub/physics/rCollider.h"
#include "de/hackcraft/world/sub/physics/rTraceable.h"

PhysicsSystem* PhysicsSystem::instance = NULL;


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


float* PhysicsSystem::getGravity() {
    return gravity;
}

float PhysicsSystem::getGndfriction() {
    return gndfriction;
}

float PhysicsSystem::getAirdensity() {
    return airdensity;
}

PhysicsSystem* PhysicsSystem::getInstance() {
    return instance;
}
