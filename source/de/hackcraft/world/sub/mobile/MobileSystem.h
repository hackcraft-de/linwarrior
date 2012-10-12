/* 
 * File:    MobileSystem.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on August 7, 2012, 8:53 PM
 */


#ifndef MOBILESYSTEM_H
#define	MOBILESYSTEM_H

#include "de/hackcraft/lang/OID.h"

#include "de/hackcraft/world/Subsystem.h"

#include <map>
#include <vector>

class Logger;
class rMobile;


class MobileSystem : public Subsystem {
public:
    /** Get current singleton instance.*/
    static MobileSystem* getInstance();

public:
    MobileSystem();
    virtual ~MobileSystem();
    
    /** Add a mobile unit to the managed components. */
    void add(rMobile* mobile);

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
    static MobileSystem* instance;
    
private:
    /** Mobile units. */
    std::vector<rMobile*> mobiles;
    std::map<OID,rMobile*> mobilesIndex;
};

#endif	/* MOBILESYSTEM_H */

