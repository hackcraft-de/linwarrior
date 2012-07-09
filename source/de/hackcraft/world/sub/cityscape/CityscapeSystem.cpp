#include "CityscapeSystem.h"

#include "de/hackcraft/world/sub/cityscape/rBuilding.h"
#include "de/hackcraft/world/sub/cityscape/rTile.h"
#include "de/hackcraft/world/sub/cityscape/rPadmap.h"


CityscapeSystem* CityscapeSystem::instance = NULL;


CityscapeSystem::CityscapeSystem() {
    instance = this;
}

CityscapeSystem::~CityscapeSystem() {
}

void CityscapeSystem::add(rBuilding* building) {
    buildings[building->id] = building;
}


void CityscapeSystem::add(rTile* tile) {
    tiles[tile->id] = tile;
}


void CityscapeSystem::add(rPadmap* padmap) {
    padmaps[padmap->id] = padmap;
}


float CityscapeSystem::constrainParticle(Entity* ex, float* worldpos, float radius) {
    float depth = 0.0f;
    
    for (std::pair<OID,rBuilding*> building : buildings) {
        depth += building.second->constrain(worldpos, radius, NULL, ex);
    }
    for (std::pair<OID,rTile*> tile : tiles) {
        depth += tile.second->constrain(worldpos, radius, NULL, ex);
    }
    for (std::pair<OID,rPadmap*> padmap : padmaps) {
        depth += padmap.second->constrain(worldpos, radius, NULL, ex);
    }
    
    return depth;
};


void CityscapeSystem::drawSolid() {
    for (std::pair<OID,rBuilding*> building : buildings) {
        building.second->drawSolid();
    }
    for (std::pair<OID,rTile*> tile : tiles) {
        tile.second->drawSolid();
    }
    for (std::pair<OID,rPadmap*> padmap : padmaps) {
        padmap.second->drawSolid();
    }
}


void CityscapeSystem::drawEffect() {
    for (std::pair<OID,rBuilding*> building : buildings) {
        building.second->drawEffect();
    }
    for (std::pair<OID,rTile*> tile : tiles) {
        tile.second->drawEffect();
    }
    for (std::pair<OID,rPadmap*> padmap : padmaps) {
        padmap.second->drawEffect();
    }
}


CityscapeSystem* CityscapeSystem::getInstance() {
    return instance;
}

