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

#include "de/hackcraft/world/sub/landscape/rPlanetmap.h"

/**
 * Models omniscient ultra largescale landscape with
 * all aspects like rendering, collision and etc.
 */
class LandscapeSystem : public Subsystem {
public:
    LandscapeSystem();
    virtual ~LandscapeSystem();
    
    void add(rPlanetmap* map);
    
    virtual float constrainParticle(Entity* ex, float* worldpos, float radius);
    
    /** Let all objects process input, adjust pose and calculate physics. */
    virtual void animateObjects();
    
    /** Draw all Object's solid surfaces (calls their drawSolid method). */
    virtual void drawSolid();
    
    /** Draw all Object's effects (calls their drawEffect method). */
    virtual void drawEffect();
private:
    rPlanetmap* planetmap;
};

#endif	/* LANDSCAPESYSTEM_H */

