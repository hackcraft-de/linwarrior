/* 
 * File:    LandscapeSystem.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on March 25, 2012, 6:02 PM
 */

#ifndef LANDSCAPESYSTEM_H
#define	LANDSCAPESYSTEM_H

class LandscapeSystem;

#include "de/hackcraft/world/Subsystem.h"

class rPlanetmap;

/**
 * Models omniscient ultra largescale landscape with
 * all aspects like rendering, collision and etc.
 */
class LandscapeSystem : public Subsystem {
public:
    static LandscapeSystem* getInstance();
    
public:
    LandscapeSystem();
    virtual ~LandscapeSystem();
    
    void add(rPlanetmap* map);
        
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
    virtual void animateObjects();
    
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
    rPlanetmap* planetmap;

    static LandscapeSystem* instance;
};

#endif	/* LANDSCAPESYSTEM_H */

