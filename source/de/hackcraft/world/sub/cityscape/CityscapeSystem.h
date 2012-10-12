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
#include <vector>

class rBuilding;
class rTile;
class rPadmap;

/**
 * Represents all aspects (rendering, collision, ...)
 * for largescale city models.
 */
class CityscapeSystem : public Subsystem {
public:
    static CityscapeSystem* getInstance();
    
public:
    CityscapeSystem();
    virtual ~CityscapeSystem();
    
    void add(rBuilding* building);
    void add(rTile* tile);
    void add(rPadmap* padmap);
    
public:
    virtual float constrainParticle(Entity* ex, float* worldpos, float radius);
    
public:
    /** Advance simulation time for one frame. */
    virtual void advanceTime(int deltamsec) {};
    
    /** Re-build spatial clustering of objects. */
    virtual void clusterObjects() {};
    
    /** Deliver overdue messages to objects. */
    virtual void dispatchMessages() {};
    
    /** Let all objects process input, adjust pose and calculate physics. */
    virtual void animateObjects() {};
    
    /** Let all objects calculate transformation matrices etc. */
    virtual void transformObjects() {};
    
    /** Setup structures for rendering */
    virtual void setupView(float* pos, float* ori) {};
    
    /** Draws background (skybox). 
     * @return False if the background was not painted or is not covered completely - true otherwise.
     */
    virtual bool drawBack() { return false; };
    
    /** Draw all Object's solid surfaces (calls their drawSolid method). */
    virtual void drawSolid();
    
    /** Draw all Object's effects (calls their drawEffect method). */
    virtual void drawEffect();

private:
    static CityscapeSystem* instance;
    
private:
    std::vector<rBuilding*> buildings;
    std::map<OID,rBuilding*> buildingsIndex;
    
    std::vector<rTile*> tiles;
    std::map<OID,rTile*> tilesIndex;
    
    std::vector<rPadmap*> padmaps;
    std::map<OID,rPadmap*> padmapsIndex;
};

#endif	/* CITYSCAPESYSTEM_H */

