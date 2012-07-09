/* 
 * File:    CityscapeSystem.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on March 25, 2012, 6:03 PM
 */


#ifndef CITYSCAPESYSTEM_H
#define	CITYSCAPESYSTEM_H

class CityscapeSystem;

#include "de/hackcraft/lang/OID.h"

#include "de/hackcraft/world/Subsystem.h"

#include <map>

class rBuilding;
class rTile;
class rPadmap;

/**
 * Represents all aspects (rendering, collision, ...)
 * for largescale city models.
 */
class CityscapeSystem : public Subsystem {
public:
    CityscapeSystem();
    virtual ~CityscapeSystem();
    void add(rBuilding* building);
    void add(rTile* tile);
    void add(rPadmap* padmap);
    
    virtual float constrainParticle(Entity* ex, float* worldpos, float radius);
    
    /** Draw all Object's solid surfaces (calls their drawSolid method). */
    virtual void drawSolid();
    
    /** Draw all Object's effects (calls their drawEffect method). */
    virtual void drawEffect();
    
    static CityscapeSystem* getInstance();
private:
    std::map<OID,rBuilding*> buildings;
    std::map<OID,rTile*> tiles;
    std::map<OID,rPadmap*> padmaps;
    
    static CityscapeSystem* instance;
};

#endif	/* CITYSCAPESYSTEM_H */

