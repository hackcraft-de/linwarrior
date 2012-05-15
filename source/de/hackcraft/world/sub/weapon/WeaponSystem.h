/* 
 * File:    WeaponSystem.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on March 22, 2012, 10:50 PM
 */


#ifndef WEAPONSYSTEM_H
#define	WEAPONSYSTEM_H

#include "rTarcom.h"
#include "rTarget.h"
#include "rWeapon.h"
#include "rWepcom.h"

#include "de/hackcraft/util/Geomap.h"

#include "de/hackcraft/world/Subsystem.h"

#include <map>


class WeaponSystem : public Subsystem {
public:
    WeaponSystem();
    WeaponSystem(const WeaponSystem& orig);
    virtual ~WeaponSystem();
    static WeaponSystem* getInstance();
    
    void add(rTarcom* tarcom);
    void add(rWepcom* wepcom);
    void add(rWeapon* weapon);
    void add(rTarget* target);
    
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
    
    /** Draws background (skybox). */
    virtual bool drawBack() { return false; };
    
    /** Draw all Object's solid surfaces (calls their drawSolid method). */
    virtual void drawSolid();
    
    /** Draw all Object's effects (calls their drawEffect method). */
    virtual void drawEffect();
private:
    std::map<OID,rTarcom*> tarcoms;
    std::map<OID,rWepcom*> wepcoms;
    std::map<OID,rWeapon*> weapons;
    std::map<OID,rTarget*> targets;
    
    Geomap<IModel*> geoWeapons;
    Geomap<rTarget*> geoTargets;
    
    /// Render only objects that far away.
    float viewdistance;
    
    /// Rendering origin or camera position.
    float visorigin[3];
    
    /// Visible objects for next rendering - (re-)set in setupView.
    std::list<IModel*>* visobjects;
    
    static WeaponSystem* instance;
};

#endif	/* WEAPONSYSTEM_H */

