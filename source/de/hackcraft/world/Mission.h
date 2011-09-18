/**
 * File:     Mission.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on March 28, 2008, 10:23 PM
 */

#ifndef _CMISSION_H
#define _CMISSION_H

class Mission;

#include "World.h"

#include "de/hackcraft/world/object/cObject.h"
#include "de/hackcraft/world/object/cPlanetmap.h"

#include "de/hackcraft/util/Pad.h"

#include <vector>
#include <string>

/**
 * Missions are responsible for initial and midtime spawning of objects
 * (including the player object), tracking mission objectives (frags).
 *
 * Future: support for timetriggers like onTime().
 *
 */
class Mission {
protected:
    // Mission Objectives
    std::vector<cObject*> mVictory; // Shall be destroyed
    std::vector<cObject*> mDefeat; // May not be destroyed includes player
    int mState;
public:

    Mission() : mState(0) {
    };

    /**
     * Called with the world which needs to be populated by this mission.
     * So this method should spawn objects into the world and setup
     * other mission critical data of the derived mission.
     */
    virtual cObject* init(World* world);

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
 * Mission for testing new Mission features.
 */
class OpenMission : public Mission {
private:
    OID group_alliance_player;
    OID group_alliance_wingmen;
    OID group_alliance_all;
    OID group_enemies_all;
    std::set<OID> inc_sense;
    std::set<OID> exc_sense;
private:
    virtual void onVictory();
    virtual void onDefeat();
    virtual cObject* init(World* world);
private:
    cObject* initPlayerParty(World* world, cPlanetmap* planetmap, float* position);
    void initSkytideCity(World* world, cPlanetmap* planetmap);
    void initStarcircleTown(World* world, cPlanetmap* planetmap);
    void initPentaSpaceport(World* world, cPlanetmap* planetmap);
    void initPyraNanoCorp(World* world, cPlanetmap* planetmap);
    void initAcroloidMines(World* world, cPlanetmap* planetmap);
    void initCollapsiumFactory(World* world, cPlanetmap* planetmap);
    void initJurataJail(World* world, cPlanetmap* planetmap);
    void initSpadenixFactory(World* world, cPlanetmap* planetmap);
private:
    static void battleField(World* world);
    static void smallArmy(int wx, int wy, int wz, World* world, const char* name, int n, bool blue, int wpn /*0*/, std::string model);
    static void smallSettling(int wx, int wy, int wz, World* world, int n);
    static void roundForrest(int wx, int wy, int wz, World* world, int r1, int r2, int nmax = 17);
    static void capitalCity(int wx, int wy, int wz, World* world);
    static void pyramidBuilding(int x, int y, int z, World* world);
    static void spacePort(int x, int y, int z, World* world, std::vector<cObject*>& defeat, std::vector<cObject*>& victory);
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

