/**
 * File:     Mission.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on March 28, 2008, 10:23 PM
 */

#ifndef _GAMEMISSION_H
#define _GAMEMISSION_H

class GameMission;

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
class GameMission {
public:

    GameMission();
    virtual ~GameMission();
    
    /**
     * Called with the world which needs to be populated by this mission.
     * So this method should spawn objects into the world and setup
     * other mission critical data of the derived mission.
     */
    virtual Entity* init(World* world);
    
    /**
     * Calls checkConditions().
     * @param deltamsec
     */
    virtual void advanceTime(int deltamsec);
    
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
    
protected:
    // Mission Objectives
    std::vector<Entity*> mVictory; // Shall be destroyed
    std::vector<Entity*> mDefeat; // May not be destroyed includes player
    int mState;
};

#endif

