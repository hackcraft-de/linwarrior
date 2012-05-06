#include "LandscapeSystem.h"
#include "de/hackcraft/world/World.h"

LandscapeSystem::LandscapeSystem() {
    planetmap = NULL;
}

LandscapeSystem::LandscapeSystem(const LandscapeSystem& orig) {
}

LandscapeSystem::~LandscapeSystem() {
}


void LandscapeSystem::add(rPlanetmap* map) {
    planetmap = map;
};


float LandscapeSystem::constrainParticle(Entity* ex, float* worldpos, float radius) {
    if (planetmap) {
        return planetmap->constrain(worldpos, radius, NULL, NULL);
    }
    return 0.0f;
};


void LandscapeSystem::animateObjects() {
    if (planetmap) {
        planetmap->animate(World::getInstance()->getTiming()->getSPF());
    }
};


void LandscapeSystem::drawSolid() {
    if (planetmap) {
        planetmap->drawSolid();
    }
};


void LandscapeSystem::drawEffect() {
    if (planetmap) {
        planetmap->drawEffect();
    }
};
