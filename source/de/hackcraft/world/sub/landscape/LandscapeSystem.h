/* 
 * File:    LandscapeSystem.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on March 25, 2012, 6:02 PM
 */


#ifndef LANDSCAPESYSTEM_H
#define	LANDSCAPESYSTEM_H

#include "de/hackcraft/world/Subsystem.h"

#include "rPlanetmap.h"

class LandscapeSystem : public Subsystem {
public:
    LandscapeSystem();
    LandscapeSystem(const LandscapeSystem& orig);
    virtual ~LandscapeSystem();
    void setPlanetmap(cPlanetmap* map) { planetmap = map; };
    
    virtual float constrainParticle(Entity* ex, float* worldpos, float radius);
    
    /// Let all objects process input, adjust pose and calculate physics.
    /*
    virtual void animateObjects() {
        if (planetmap) {
            planetmap->animate();
        }
    };
    */
    
    /// Draw all Object's solid surfaces (calls their drawSolid method).
    virtual void drawSolid();
    
    /// Draw all Object's effects (calls their drawEffect method).
    virtual void drawEffect();
private:
    cPlanetmap* planetmap;
};

#endif	/* LANDSCAPESYSTEM_H */

