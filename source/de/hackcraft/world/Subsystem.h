/* 
 * File:     Subsystem.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on February 19, 2012, 5:41 PM
 */

#ifndef SUBSYSTEM_H
#define	SUBSYSTEM_H

class Subsystem;

#include <stdlib.h>

class Entity;

/**
 * Abstract base class for subsystems that form the whole world system.
 */
class Subsystem {

public: // Housekeeping
    /**
     * Returns the id of the subsystem
     * @return Id of this subsystem.
     */
    virtual int getId() { return 0; };
    
    /**
     * Returns a once allocated string with the name
     * of the system for debugging purposes - don't change or delete.
     * Shall be implemented in all subclasses.
     * 
     * @return Name of this subsystem.
     */
    virtual char* getName() { return (char*) NULL; };

public: // Interaction
    /**
     * Limit position to accessible areas and returns impact depth.
     * 
     * @param worldpos The particle position to be constrained given in world coordinates.
     * @param radius The size of the particle in terms of a radius.
     * @return Zero if there was no collision, else the maximum impact depth.
     */
    virtual float constrainParticle(Entity* ex, float* worldpos, float radius) = 0;
    
    //virtual float constrainParticleInc(Entity* inc, float* worldpos, float radius) { return 0.0f; };
    
    //virtual float inflictUppon(Entity* enactor, Entity* reactor, float* localpos, int key, float value) { return 0.0f; };
    
public: // Frame cycle
    /** Advance simulation time for one frame. */
    virtual void advanceTime(int deltamsec) = 0;
    
    /** Re-build spatial clustering of objects. */
    virtual void clusterObjects() = 0;
    
    /** Deliver overdue messages to objects. */
    virtual void dispatchMessages() = 0;
    
    /** Let all objects process input, adjust pose and calculate physics. */
    virtual void animateObjects() = 0;
    
    /** Let all objects calculate transformation matrices etc. */
    virtual void transformObjects() = 0;
    
    /** Setup structures for rendering */
    virtual void setupView(float* pos, float* ori) = 0;
    
    /** Draws background (skybox). 
     * @return False if the background was not painted or is not covered completely - true otherwise.
     */
    virtual bool drawBack() = 0;
    
    /** Draw all Object's solid surfaces (calls their drawSolid method). */
    virtual void drawSolid() = 0;
    
    /** Draw all Object's effects (calls their drawEffect method). */
    virtual void drawEffect() = 0;
};

#endif	/* SUBSYSTEM_H */

