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

#include "de/hackcraft/world/sub/model/rBillboard.h"
#include "de/hackcraft/world/sub/model/rRigged.h"
#include "de/hackcraft/world/sub/model/rScatter.h"
#include "de/hackcraft/world/sub/model/rTree.h"

#include <unordered_map>
#include <vector>

/**
 * Manages small to medium scale model rendering,
 * collision and so on.
 */
class ModelSystem : public Subsystem {
public:
    ModelSystem();
    virtual ~ModelSystem();
    
    void add(IModel* model) {
        models.push_back(model);
    };
    
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
   /** Draw all Object's solid surfaces (calls their drawSolid method). */
    virtual void drawSolid();
    /** Draw all Object's effects (calls their drawEffect method). */
    virtual void drawEffect();
    
    static ModelSystem* getInstance();
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
    
    static ModelSystem* instance;
};

#endif	/* MODELSYSTEM_H */

