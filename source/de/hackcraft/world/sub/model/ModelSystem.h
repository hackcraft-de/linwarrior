/* 
 * File:    ModelSystem.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on March 22, 2012, 10:52 PM
 */

#ifndef MODELSYSTEM_H
#define	MODELSYSTEM_H

class ModelSystem;

#include "de/hackcraft/util/Geomap.h"

#include "de/hackcraft/world/IModel.h"
#include "de/hackcraft/world/Subsystem.h"

#include <unordered_map>
#include <vector>

class Logger;

/**
 * Manages small to medium scale model rendering,
 * collision and so on.
 */
class ModelSystem : public Subsystem {
public:
    static ModelSystem* getInstance();
    
public:
    ModelSystem();
    virtual ~ModelSystem();
    
    void add(IModel* model);
    
public:
    /**
     * Limit position to accessible areas and returns impact depth.
     * 
     * @param worldpos The particle position to be constrained given in world coordinates.
     * @param radius The size of the particle in terms of a radius.
     * @return Zero if there was no collision, else the maximum impact depth.
     */
    virtual float constrainParticle(Entity* ex, float* worldpos, float radius) { return 0.0f; };
    
public:
    /** Advance simulation time for one frame. */
    virtual void advanceTime(int deltamsec) {};
    
    /** Re-build spatial clustering of objects. */
    virtual void clusterObjects();
    
    /** Deliver overdue messages to objects. */
    virtual void dispatchMessages() {};
    
    /** Let all objects process input, adjust pose and calculate physics. */
    virtual void animateObjects();
    
    /** Let all objects calculate transformation matrices etc. */
    virtual void transformObjects();
    
    /** Setup structures for rendering */
    virtual void setupView(float* pos, float* ori);
    
    /** Draws background (skybox). 
     * @return False if the background was not painted or is not covered completely - true otherwise.
     */
    virtual bool drawBack() { return false; };
    
    /** Draw all Object's solid surfaces (calls their drawSolid method). */
    virtual void drawSolid();
    
    /** Draw all Object's effects (calls their drawEffect method). */
    virtual void drawEffect();
    
private:
    static Logger* logger;
    
    static ModelSystem* instance;
    
private:
    /** Allows searching the world in a structured manner. */
    Geomap<IModel*> geomap;
    
    std::vector<IModel*> models;
    
    /** Render only objects that far away. */
    float viewdistance;
    
    /** Rendering origin or camera position. */
    float visorigin[3];
    
    /** Visible objects for next rendering - (re-)set in setupView. */
    std::list<IModel*>* visobjects;
};

#endif	/* MODELSYSTEM_H */

