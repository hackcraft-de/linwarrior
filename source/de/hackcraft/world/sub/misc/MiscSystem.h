/* 
 * File:    MiscSystem.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on August 7, 2012, 8:26 PM
 */


#ifndef MISCSYSTEM_H
#define	MISCSYSTEM_H

#include "de/hackcraft/lang/OID.h"

#include "de/hackcraft/world/Subsystem.h"

#include <map>
#include <vector>

class Logger;
class rSoundsource;
class rLightsource;
class rInputsource;

/**
 * Incubator subsystem for components
 * until they deserve their own subsystem.
 */
class MiscSystem : public Subsystem {
public:
    /** Get current singleton instance.*/
    static MiscSystem* getInstance();

public:
    MiscSystem();
    virtual ~MiscSystem();
    
    /** Add a component of that type to the managed components. */
    void add(rSoundsource* component);
    
    /** Add a component of that type to the managed components. */
    void add(rLightsource* component);
    
    /** Add a component of that type to the managed components. */
    void add(rInputsource* component);
    
    rInputsource* findInputsourceByEntity(OID entityID);

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
    virtual void drawSolid() {};
    
    /** Draw all Object's effects (calls their drawEffect method). */
    virtual void drawEffect() {};
    
private:
    static Logger* logger;
    
    /** Current singleton instance */
    static MiscSystem* instance;
    
private:
    /** List of allocated components of this type. */
    std::vector<rSoundsource*> soundsources;
    /** Index on id of leased components of this type. */
    std::map<OID,rSoundsource*> soundsourcesIndex;
    
    /** List of allocated components of this type. */
    std::vector<rLightsource*> lightsources;
    /** Index on id of leased components of this type. */
    std::map<OID,rLightsource*> lightsourcesIndex;
    
    /** List of allocated components of this type. */
    std::vector<rInputsource*> inputsources;
    /** Index on id of leased components of this type. */
    std::map<OID,rInputsource*> inputsourcesIndex;
};

#endif	/* MISCSYSTEM_H */

