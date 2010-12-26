/**
 * File:     cMission.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on March 28, 2008, 10:23 PM
 */

#ifndef _CMISSION_H
#define _CMISSION_H

class cMission;

#include "cWorld.h"
#include "cObject.h"
#include "cPad.h"
#include "cPlanetmap.h"

#include <vector>


/**
 * Missions are responsible for initial and midtime spawning of objects
 * (including the player object), tracking mission objectives (frags).
 *
 * Future: support for timetriggers like onTime().
 *
 */
class cMission {
protected:
    // Mission Objectives
    std::vector<cObject*> mVictory; // Shall be destroyed
    std::vector<cObject*> mDefeat; // May not be destroyed includes player
    int mState;
public:
    cMission() :mState(0) {};
    
    /**
     * Called with the world which needs to be populated by this mission.
     * So this method should spawn objects into the world and setup
     * other mission critical data of the derived mission.
     */
    virtual cObject* init(cWorld* world) = 0;
    
    /**
     * Through this method mission objectives can be traced.
     * Victory (ANDed) and Defeat (ORed) Conditions are checked.
     * onVictory or onDefeat are called accordingly.
     * Enhance as desired.
     */
    virtual void checkConditions();

    /// Called when checkConditions() sees the victory condition(s) fullfilled.
    virtual void onVictory();

    /// Called when checkConditions() sees the defeat condition(s) fullfilled.
    virtual void onDefeat();
    
    /**
     * Indicates wether this Mission is a success.
     */
    //virtual bool isSuccess() = 0;
    
    /**
     * Indicates wether this Mission is a failure.
     */
    //virtual bool isFailure() = 0;
};


/**
 * Just a Mech in an infinite empty plane world.
 */
class cEmptyMission : public cMission {
    virtual cObject* init(cWorld* world);
};


/**
 * Mission for testing new Mission features.
 */
class cOpenMission : public cMission {
private:
    cObject group_alliance_player;
    cObject group_alliance_wingmen;
    cObject group_alliance_all;
    cObject group_enemies_all;
private:
    virtual void onVictory();
    virtual void onDefeat();
    virtual cObject* init(cWorld* world);
private:
    cObject* initPlayerParty(cWorld* world, cPlanetmap* planetmap);
    void initSkytideCity(cWorld* world, cPlanetmap* planetmap);
    void initStarcircleTown(cWorld* world, cPlanetmap* planetmap);
    void initPentaSpaceport(cWorld* world, cPlanetmap* planetmap);
    void initPyraNanoCorp(cWorld* world, cPlanetmap* planetmap);
    void initAcroloidMines(cWorld* world, cPlanetmap* planetmap);
    void initCollapsiumFactory(cWorld* world, cPlanetmap* planetmap);
    void initJurataJail(cWorld* world, cPlanetmap* planetmap);
    void initSpadenixFactory(cWorld* world, cPlanetmap* planetmap);
private:
    static void battleField(cWorld* world);
    static void smallArmy(int wx, int wy, int wz, cWorld* world, const char* name, int n, bool blue, int wpn=0);
    static void smallSettling(int wx, int wy, int wz, cWorld* world, int n);
    static void roundForrest(int wx, int wy, int wz, cWorld* world, int r1, int r2, int nmax = 17);
    static void capitalCity(int wx, int wy, int wz, cWorld* world);
    static void pyramidBuilding(int x, int y, int z, cWorld* world);
    static void spacePort(int x, int y, int z, cWorld* world, std::vector<cObject*>& defeat, std::vector<cObject*>& victory);
};

/** 
 * Mission 1: Dawn of a Pirate Space Opera
 *
 * In Preparation to conquer an small planetary space habour
 * this Mission has lead a young space pirate (you) down to the planet
 * Orbitan's surface to disable some shield generators and energy hubs to
 * enable pirate reenforcements to land.
 * Landing zone is a blind spot in habitants defenses.
 * Nighttime
 */

#endif

