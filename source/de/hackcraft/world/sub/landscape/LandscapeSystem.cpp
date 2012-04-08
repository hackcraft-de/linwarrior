#include "LandscapeSystem.h"

LandscapeSystem::LandscapeSystem() {
    planetmap = NULL;
}

LandscapeSystem::LandscapeSystem(const LandscapeSystem& orig) {
}

LandscapeSystem::~LandscapeSystem() {
}

float LandscapeSystem::constrainParticle(Entity* ex, float* worldpos, float radius) {
    if (planetmap) {
        return planetmap->constrain(worldpos, radius, NULL, NULL);
    }
    return 0.0f;
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
