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

#include "de/hackcraft/world/Subsystem.h"

#include "de/hackcraft/world/sub/cityscape/rBuilding.h"
#include "de/hackcraft/world/sub/cityscape/rTile.h"
#include "de/hackcraft/world/sub/cityscape/rPadmap.h"

#include <map>

/**
 * Represents all aspects (rendering, collision, ...)
 * for largescale city models.
 */
class CityscapeSystem : public Subsystem {
public:
    CityscapeSystem();
    CityscapeSystem(const CityscapeSystem& orig);
    virtual ~CityscapeSystem();
    void add(rBuilding* building) { buildings[building->id] = building; };
    void add(rTile* tile) { tiles[tile->id] = tile; };
    void add(rPadmap* padmap) { padmaps[padmap->id] = padmap; };
    
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

