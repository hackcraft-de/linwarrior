/* 
 * File:   OpenMission.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on December 28, 2011, 7:24 PM
 * (March 28, 2008, 10:23 PM)
 */

#ifndef OPENMISSION_H
#define	OPENMISSION_H

#include "de/hackcraft/world/sub/mission/MissionSystem.h"
#include "de/hackcraft/world/Propmap.h"
#include "de/hackcraft/world/sub/trigger/TriggerSystem.h"
#include "de/hackcraft/world/sub/landscape/rPlanetmap.h"
#include "de/hackcraft/world/sub/cityscape/CityscapeSystem.h"

/**
 * Mission for testing new Mission features.
 */
class OpenMission : public MissionSystem {
private:
    OID group_alliance_player;
    OID group_alliance_wingmen;
    OID group_alliance_all;
    OID group_enemies_all;
    std::set<OID> inc_sense;
    std::set<OID> exc_sense;
    Propmap globalProperties;
    TriggerSystem* triggerSystem;
    CityscapeSystem* cityscapeSystem;
private:
    virtual void onVictory();
    virtual void onDefeat();
    virtual Entity* init(World* world);
private:
    Entity* initPlayerParty(World* world, cPlanetmap* planetmap, float* position);
    void initSkytideCity(World* world, cPlanetmap* planetmap);
    void initStarcircleTown(World* world, cPlanetmap* planetmap);
    void initPentaSpaceport(World* world, cPlanetmap* planetmap);
    void initPyraNanoCorp(World* world, cPlanetmap* planetmap);
    void initAcroloidMines(World* world, cPlanetmap* planetmap);
    void initCollapsiumFactory(World* world, cPlanetmap* planetmap);
    void initJurataJail(World* world, cPlanetmap* planetmap);
    void initSpadenixFactory(World* world, cPlanetmap* planetmap);
    void initFortifyDefense(World* world, cPlanetmap* planetmap);
private:
    void battleField(World* world);
    void smallArmy(int wx, int wy, int wz, World* world, const char* name, int n, bool blue, int wpn /*0*/, bool immobile, std::string model);
    void smallSettling(int wx, int wy, int wz, World* world, int n);
    void roundForrest(int wx, int wy, int wz, World* world, int r1, int r2, int nmax = 17);
    void capitalCity(int wx, int wy, int wz, World* world);
    void pyramidBuilding(int x, int y, int z, World* world);
    void spacePort(int x, int y, int z, World* world, std::vector<Entity*>& defeat, std::vector<Entity*>& victory);
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

#endif	/* OPENMISSION_H */

