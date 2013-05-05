/* 
 * File:     BatchMission.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on May 2, 2013, 9:20 PM
 */

#ifndef BATCHMISSION_H
#define	BATCHMISSION_H

#include "de/hackcraft/game/GameMission.h"

#include "de/hackcraft/util/Propmap.h"

#include <list>
#include <string>

class Logger;
class rPlanetmap;

class BatchMission : public GameMission {
    
private:
    static Logger* logger;
    
private:
    Propmap globalProperties;
    rPlanetmap* planetmap;
    
public:
    BatchMission();
    virtual ~BatchMission();

    /**
     * Called with the world which needs to be populated by this mission.
     * So this method should spawn objects into the world and setup
     * other mission critical data of the derived mission.
     */
    virtual Entity* init(World* world);
    
    void initSystems();
    
    int execScript(std::istream* is);
    
    int execCommand(std::list<std::string>* args);
    
    int execCommandSet(std::list<std::string>* args);
    
    int execCommandSpawn(std::list<std::string>* args);
    
    void spawnMech();
    
    Entity* initPlayerParty(float* position);
    
    void adjustHeight(float* pos);
    
    /**
     * Calls checkConditions().
     * @param deltamsec
     */
    //virtual void advanceTime(int deltamsec);
    
    /**
     * Through this method mission objectives can be traced.
     * Victory (ANDed) and Defeat (ORed) Conditions are checked.
     * onVictory or onDefeat are called accordingly.
     * Enhance as desired.
     */
    //virtual void checkConditions();

    /** Called when checkConditions() sees the victory condition(s) fullfilled. */
    //virtual void onVictory();

    /** Called when checkConditions() sees the defeat condition(s) fullfilled. */
    //virtual void onDefeat();
private:

};

#endif	/* BATCHMISSION_H */

