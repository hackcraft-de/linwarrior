#include "CityscapeSystem.h"

#include "de/hackcraft/world/sub/cityscape/rBuilding.h"
#include "de/hackcraft/world/sub/cityscape/rTile.h"
#include "de/hackcraft/world/sub/cityscape/rPadmap.h"


CityscapeSystem* CityscapeSystem::instance = NULL;


CityscapeSystem* CityscapeSystem::getInstance() {
    return instance;
}


CityscapeSystem::CityscapeSystem() {
    instance = this;
}


CityscapeSystem::~CityscapeSystem() {
}


void CityscapeSystem::add(rBuilding* building) {
    buildings.push_back(building);
    buildingsIndex[building->getId()] = building;
}


void CityscapeSystem::add(rTile* tile) {
    tiles.push_back(tile);
    tilesIndex[tile->getId()] = tile;
}


void CityscapeSystem::add(rPadmap* padmap) {
    padmaps.push_back(padmap);
    padmapsIndex[padmap->getId()] = padmap;
}


float CityscapeSystem::constrainParticle(Entity* ex, float* worldpos, float radius) {
    
    float depth = 0.0f;
    
    for (rBuilding* building : buildings) {
        depth += building->constrain(worldpos, radius, NULL, ex);
    }
    
    for (rTile* tile : tiles) {
        depth += tile->constrain(worldpos, radius, NULL, ex);
    }
    
    for (rPadmap* padmap : padmaps) {
        depth += padmap->constrain(worldpos, radius, NULL, ex);
    }
    
    return depth;
};


void CityscapeSystem::drawSolid() {
    
    for (rBuilding* building : buildings) {
        building->drawSolid();
    }
    
    for (rTile* tile : tiles) {
        tile->drawSolid();
    }
    
    for (rPadmap* padmap : padmaps) {
        padmap->drawSolid();
    }
}


void CityscapeSystem::drawEffect() {
    
    for (rBuilding* building : buildings) {
        building->drawEffect();
    }
    
    for (rTile* tile : tiles) {
        tile->drawEffect();
    }
    
    for (rPadmap* padmap : padmaps) {
        padmap->drawEffect();
    }
}

