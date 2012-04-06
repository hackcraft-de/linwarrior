#include "PhysicsSystem.h"

// Earth : -9.80665f
// Mars  : -3.69f
vec3 PhysicsSystem::mGravity = { 0.0f, -9.80665f, 0.0f };

// Helium    : 0.1785f
// Earth-Air : 1.204f
// Water     : 1000.0f
float PhysicsSystem::mAirdensity = 1.204f;

// ??? : 0.13f
float PhysicsSystem::mGndfriction = 0.13f;


PhysicsSystem::PhysicsSystem() {
}

PhysicsSystem::PhysicsSystem(const PhysicsSystem& orig) {
}

PhysicsSystem::~PhysicsSystem() {
}


float* PhysicsSystem::getGravity() {
    return mGravity;
}

float PhysicsSystem::getGndfriction() {
    return mGndfriction;
}

float PhysicsSystem::getAirdensity() {
    return mAirdensity;
}
