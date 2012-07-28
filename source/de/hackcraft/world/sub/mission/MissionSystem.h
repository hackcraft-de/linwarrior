/**
 * File:     Mission.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on March 28, 2008, 10:23 PM
 */

#ifndef _CMISSIONSYSTEM_H
#define _CMISSIONSYSTEM_H

class MissionSystem;

#include "de/hackcraft/world/Subsystem.h"

#include <vector>
#include <string>

class Entity;
class World;

/**
 * Missions are responsible for initial and midtime spawning of objects
 * (including the player object), tracking mission objectives (frags).
 *
 * Future: support for timetriggers like onTime().
 *
 */
class MissionSystem : public Subsystem {
public:
    static MissionSystem* getInstance();

public:

    MissionSystem();
    virtual ~MissionSystem();
    
    /**
     * Called with the world which needs to be populated by this mission.
     * So this method should spawn objects into the world and setup
     * other mission critical data of the derived mission.
     */
    virtual Entity* init(World* world);
    
    /**
     * Through this method mission objectives can be traced.
     * Victory (ANDed) and Defeat (ORed) Conditions are checked.
     * onVictory or onDefeat are called accordingly.
     * Enhance as desired.
     */
    virtual void checkConditions();

    /** Called when checkConditions() sees the victory condition(s) fullfilled. */
    virtual void onVictory();

    /** Called when checkConditions() sees the defeat condition(s) fullfilled. */
    virtual void onDefeat();

    /**
     * Indicates whether this Mission is a success.
     */
    //virtual bool isSuccess() = 0;

    /**
     * Indicates whether this Mission is a failure.
     */
    //virtual bool isFailure() = 0;

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
    /**
     * Calls checkConditions().
     * @param deltamsec
     */
    virtual void advanceTime(int deltamsec);

    /** Re-build spatial clustering of objects. */
    virtual void clusterObjects() {};
    
    /** Deliver overdue messages to objects. */
    virtual void dispatchMessages() {};
    
    /** Let all objects process input, adjust pose and calculate physics. */
    virtual void animateObjects() {};
    
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
    static MissionSystem* instance;
    
protected:
    // Mission Objectives
    std::vector<Entity*> mVictory; // Shall be destroyed
    std::vector<Entity*> mDefeat; // May not be destroyed includes player
    int mState;
};

#endif

