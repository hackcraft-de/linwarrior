/* 
 * File:    WeaponSystem.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on March 22, 2012, 10:50 PM
 */

#ifndef WEAPONSYSTEM_H
#define	WEAPONSYSTEM_H

class WeaponSystem;

#include "de/hackcraft/lang/OID.h"

#include "de/hackcraft/util/Geomap.h"

#include "de/hackcraft/world/Subsystem.h"

#include <list>
#include <map>
#include <set>

class Entity;
class Logger;
class IModel;
class rTarcom;
class rTarget;
class rWeapon;
class rWepcom;

/**
 * Manages weapons, targets and ranging-/targeting-devices.
 */
class WeaponSystem : public Subsystem {
private:
    static Logger* logger;
public:
    WeaponSystem();
    virtual ~WeaponSystem();
    static WeaponSystem* getInstance();
    
    void add(rTarcom* tarcom);
    void add(rWepcom* wepcom);
    void add(rWeapon* weapon);
    void add(rTarget* target);
    
    rTarcom* findTarcomByEntity(OID entityID);
    rTarget* findTargetByEntity(OID entityID);
    
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
    
    /**
     * Filters the given objectlist by tags (ORed Bitmask) (excluding ex Object).
     * There is the option to match all tags (true=AND) or to select any given tag (false=OR).
     * The returned list is fresh allocated - caller delete responsibility.
     */
    std::list<rTarget*>* filterByTags(Entity* ex, std::set<OID>* rolemask, bool all, int maxamount, std::list<rTarget*>* objects);

    /** Returns a List of objects which are within minimum and maximum range (excluding ex Object).
     * The returned list is fresh allocated - caller delete responsibility.
     */
    std::list<rTarget*>* filterByRange(Entity* ex, float* origin, float minrange, float maxrange, int maxamount, std::list<rTarget*>* objects = NULL);
private:
    std::map<OID,rTarcom*> tarcoms;
    std::map<OID,rWepcom*> wepcoms;
    std::map<OID,rWeapon*> weapons;
    std::map<OID,rTarget*> targets;

    std::map<OID,rTarcom*> tarcomByEntity;
    std::map<OID,rTarget*> targetByEntity;
    
    Geomap<IModel*> geoWeapons;
    Geomap<rTarget*> geoTargets;
    
    /** Render only objects that far away. */
    float viewdistance;
    
    /** Rendering origin or camera position. */
    float visorigin[3];
    
    /** Visible objects for next rendering - (re-)set in setupView. */
    std::list<IModel*>* visobjects;
    
    static WeaponSystem* instance;
};

#endif	/* WEAPONSYSTEM_H */

