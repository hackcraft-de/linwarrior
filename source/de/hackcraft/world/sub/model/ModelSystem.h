/* 
 * File:    ModelSystem.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on March 22, 2012, 10:52 PM
 */


#ifndef MODELSYSTEM_H
#define	MODELSYSTEM_H

#include "IModel.h"

#include "rBillboard.h"
#include "rRigged.h"
#include "rScatter.h"
#include "rTree.h"

#include "de/hackcraft/world/Subsystem.h"

#include <unordered_map>
#include <vector>

class ModelSystem : public Subsystem {
public:
    ModelSystem();
    ModelSystem(const ModelSystem& orig);
    virtual ~ModelSystem();
    
    void add(IModel* model) {
        models.push_back(model);
    };
    
     /// Advance simulation time for one frame.
    virtual void advanceTime(int deltamsec) {};
    /// Re-build spatial clustering of objects.
    virtual void clusterObjects();
    /// Deliver overdue messages to objects.
    virtual void dispatchMessages() {};
    /// Let all objects process input, adjust pose and calculate physics.
    virtual void animateObjects();
    /// Let all objects calculate transformation matrices etc.
    virtual void transformObjects();
    /// Setup structures for rendering
    virtual void setupView(float* pos, float* ori);
   /// Draw all Object's solid surfaces (calls their drawSolid method).
    virtual void drawSolid();
    /// Draw all Object's effects (calls their drawEffect method).
    virtual void drawEffect();
    
    static ModelSystem* getInstance();
private:
    /// Allows searching the world in a structured manner.
    std::unordered_map<OID, std::list<IModel*> > geomap;
    
    std::vector<IModel*> models;
    
    /// Seconds per frame.
    float spf;
    
    /// Render only objects that far away.
    float viewdistance;
    
    /// Rendering origin or camera position.
    float visorigin[3];
    
    /// Visible objects for next rendering - (re-)set in setupView.
    std::list<IModel*>* visobjects;
    
    static ModelSystem* instance;


    /**
     * Hash key for location.
     *
     * @param x east west coord
     * @param z north south coord
     * @return Hash key for location.
     */
    OID getGeokey(long x, long z);

    /**
     * Find objects within 2d area using location hashkeys.
     * 
     * @param min2f First corner of square area (smaller values).
     * @param max2f Second corner of square area (larger values).
     * @param addunclustered Return global unclustered objects, too.
     * @return List of found objects.
     */
    std::list<IModel*>* getGeoInterval(float* min2f, float* max2f, bool addunclustered = false);
};

#endif	/* MODELSYSTEM_H */

