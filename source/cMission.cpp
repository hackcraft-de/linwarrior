#include "cMission.h"

#include "main.h"

#include "cObject.h"
#include "cAlert.h"
#include "cPlanetmap.h"

#include "rComcom.h"
#include "rNameable.h"

#include <list>
#include <vector>
#include <cassert>
#include <time.h>
#include <iostream>
using std::cout;
using std::endl;
#include <string>
using std::string;

void adjustHeight(cPlanetmap* planetmap, float* pos) {
    float color[16];
    planetmap->getHeight(pos[0], pos[2], color);
    pos[1] = color[3] + 0.001f;
}

void cMission::checkConditions() {
    // Destroyed all necessary enemies?
    unsigned int n = 0;
    loopiv(mVictory) if (mVictory[i]->hasTag(cObject::DEAD)) n++;
    if (n != 0 && n == mVictory.size()) {
        onVictory();
        return;
    }
    // Destroyed any neccessary friend?
    n = 0;
    loopiv(mDefeat) if (mDefeat[i]->hasTag(cObject::DEAD)) n++;
    if (n != 0) {
        onDefeat();
        return;
    }
}

void cMission::onVictory() {
    if (mState != -1) {
        mState = -1;
        cWorld::instance->sendMessage(0, 0, 0,
                "DISPLAY",
                std::string("   (+) Primary target fullfilled.\n   (+) You Won!\n   (+) Hit ESC to finish Mission\n")
                );
    }
}

void cMission::onDefeat() {
    if (mState != -2) {
        mState = -2;
        cWorld::instance->sendMessage(0, 0, 0,
                "DISPLAY",
                std::string("   (-) You Lost!\n   (+) Hit ESC to finish Mission\n")
                );
    }
}

cObject* cMission::init(cWorld* world) {
    if (true) {
        world->getTiming()->setTime(12);
    } else {
        float hour;
        getSystemHour(hour);
        world->getTiming()->setTime(hour);
    }

    cObject* player = NULL;

    if (true) {
        float pos[3] = {-50, 0, -50};
        cMech* mech = new cMech(pos);
        assert(mech != NULL);
        mech->name = "Playermech";

        mech->nameable->name = "Alpha";

        mech->controlled->controller->controllerEnabled = false; // Disable Autopilot.
        mech->addTag(cObject::BLUE);
        mech->addTag(cObject::HUMANPLAYER);
        mech->tarcom->addEnemy(cObject::RED);
        mech->tarcom->addEnemy(cObject::DEAD, false);

        player = mech;

        //cController* ai = new cController;
        //ai->newState(cController::TARGET, "Delta");
        //mech->mController = ai;

        world->spawnObject(mech);

        if (true) {

            //mech->mountWeapon((char*)"LTorsor", new cExplosion);
            mech->mountWeapon((char*) "LTorsor", new rWeaponPlasmagun);
            if (true) {
                mech->mountWeapon((char*) "LLoArm", new rWeaponMachinegun);
                //mech->mountWeapon((char*)"LTorsor",  new cMachineGun);
                //mech->mountWeapon((char*)"RTorsor",  new cMachineGun);
                mech->mountWeapon((char*) "RLoArm", new rWeaponMachinegun);
            }
            mech->mountWeapon((char*) "RTorsor", new rWeaponPlasmagun);
            //mech->mountWeapon((char*)"RLoArm",  new cHomingMissile);
            //mech->mountWeapon((char*)"RLoArm",  new cRaybeam);
        }
    }

    world->spawnObject(new cBuilding(0, 0, 0, 10, 3, 10));

    mVictory.push_back(player);
    return player;
}


// ------------------------------------------------------------


#define SKYTIDE     { 0, 0, 0 }
#define SPACEPORT   { -270, 0, -180 }
#define PYRANANO    { -210, 0, 270 }

#define STARCIRCLE  { 250, 0, 0 }
#define COLLAPSIUM  { 300, 0, -150 }
#define ACROLOID    { 400, 0, -200 }
#define JURATA      { 400, 0, +100 }
#define SPADENIX    { 300, 0, +150 }

#define PLAYERPOS SKYTIDE

void cOpenMission::onVictory() {
    //cMission::onVictory();
    if (mState != -1) {
        mState = -1;
        cout << "onVictory!";
        cWorld::instance->sendMessage(0, 0, group_alliance_player.oid,
                "DISPLAY",
                //std::string("   (+) Primary target fullfilled.\n   (+) You Won!\n   (+) Hit ESC to finish Mission\n")
                std::string("Stortebeker:\n  Good work!\n  We now have control over the town.\n  You may have a look around:\n  Pyra Nano Corp: SW of Skytide.\n  Starcircle: E of Skytide\n  Spaceport: NW\n  And don't get yourself killed.")
                );
    }
}

void cOpenMission::onDefeat() {
    //cMission::onDefeat();
    if (mState != -2) {
        mState = -2;
        cout << "onDefeat!";
        cWorld::instance->sendMessage(0, 0, group_alliance_player.oid,
                "DISPLAY",
                //std::string("   (-) You Lost!\n   (+) Hit ESC to finish Mission\n")
                std::string("Stortebeker:\n  And I told you not to die!\n  We will share your booty evenly\n  and spread your ashes in space.\n")
                );
    }
}

cObject* cOpenMission::init(cWorld* world) {
    srand(0);
    
    cout << "Setting mission date and time...\n";
    if (!true) {
        world->getTiming()->setTime(12);
    } else {
        float hour;
        getSystemHour(hour);
        hour = fmod(hour, 24);
        world->getTiming()->setTime(hour, 60 * (hour - (int) hour));
    }

    cout << "Initialising planetary maps...\n";
    cPlanetmap* planetmap = new cPlanetmap();
    world->spawnObject(planetmap);

    cout << "Initialising call groups...\n";
    {
        group_alliance_player.name = "/call/alliance/player";
        group_alliance_player.grouping = new rGrouping();
        world->spawnObject(&group_alliance_player);

        group_alliance_wingmen.name = "/call/alliance/wingmen";
        group_alliance_wingmen.grouping = new rGrouping();
        world->spawnObject(&group_alliance_wingmen);

        group_alliance_all.name = "/call/alliance/all";
        group_alliance_all.grouping = new rGrouping();
        world->spawnObject(&group_alliance_all);

        group_enemies_all.name = "/call/enemies/all";
        group_enemies_all.grouping = new rGrouping();
        world->spawnObject(&group_enemies_all);
    }

    cout << "Initialising default sensitivities...\n";
    {
        inc_sense.insert(cObject::HUMANPLAYER);
        exc_sense.insert(cObject::DEAD);
    }

    cout << "Initialising Skytide City...\n";
    initSkytideCity(world, planetmap);

    cout << "Initialising Starcircle Town...\n";
    initStarcircleTown(world, planetmap);

    cout << "Initialising Penta Spaceport...\n";
    initPentaSpaceport(world, planetmap);

    cout << "Initialising Pyra Nano Corp...\n";
    initPyraNanoCorp(world, planetmap);

    //initAcroloidMines(world, planetmap);
    //initCollapsiumFactory(world, planetmap);
    //initJurataJail(world, planetmap);
    //initSpadenixFactory(world, planetmap);

    if (!true) {
        cout << "Initialising Experimental Structure...\n";
        float loc[] = { -70, 0, -70 };

        float color[16];
        planetmap->getHeight(loc[0],loc[2], color);
        loc[1] += color[3];

        // Provide flat terrain at that height.
        cPlanetmap::sMod* mod = new cPlanetmap::sMod();
        mod->pos[0] = loc[0]+7;
        mod->pos[1] = loc[1];
        mod->pos[2] = loc[2]+7;
        mod->height = loc[1] + 0.019f;
        mod->range = 15;
        planetmap->mods.push_back(mod);
        // Experimental
        world->spawnObject(new cPadmap(loc[0], loc[1], loc[2]));
    }

    cout << "Initialising vehicles...\n";
    cObject* player = NULL;
    {
        float position[] = PLAYERPOS;
        player = initPlayerParty(world, planetmap, position);
    }

    cout << "Initialising mission triggers...\n";
    {
        // Setup Leaving Mission Area Warning-Alert.
        {
            float pos[] = {50, -1, 50};
            float range[] = {400, 100, 400};
            OID group = group_alliance_player.oid;
            std::string type = "RADIO";
            std::string message = std::string("YOU ARE LEAVING THE MISSION AREA!");
            bool positive = false;
            bool posedge = true;
            OID delay = 0;
            cObject* oob = new cAlert(pos, range, cAlert::rShape::BOX, type, message, group, &inc_sense, &exc_sense, positive, posedge, delay);
            world->spawnObject(oob);
        }

        // Setup Contract Voided Aleart.
        {
            float pos[] = {50, -1, 50};
            float range[] = {490, 190, 490};
            OID group = group_alliance_player.oid;
            std::string type = "RADIO";
            std::string message = std::string("YOU HAVE LEFT THE MISSION AREA, CONTRACT VOIDED!");
            bool positive = false;
            bool posedge = true;
            OID delay = 0;
            cObject* oob = new cAlert(pos, range, cAlert::rShape::BOX, type, message, group, &inc_sense, &exc_sense, positive, posedge, delay);
            world->spawnObject(oob);
        }
        // Defeated when killed.
        mDefeat.push_back(player);
    }

    return player;
}

cObject* cOpenMission::initPlayerParty(cWorld* world, cPlanetmap* planetmap, float* position) {
    cObject* player = NULL;

    float* p = position;

    // Player
    if (true) {
        float pos[3] = {p[0]-20, 0.1, p[2]-50};
        float rot[3] = {0, -170, 0};
        adjustHeight(planetmap, pos);
        cMech* mech = new cMech(pos, rot);
        if (mech == NULL) throw "No memory for player mech!";
        player = mech;
        mech->name = "Playermech";
        mech->nameable->name = "Alpha";
        mech->controlled->controller->controllerEnabled = false; // Disable Autopilot.
        mech->addTag(cObject::BLUE);
        mech->addTag(cObject::HUMANPLAYER);
        mech->tarcom->addEnemy(cObject::RED);
        mech->tarcom->addEnemy(cObject::DEAD, false);

        world->spawnObject(mech);
        group_alliance_player.grouping->members.insert(mech->oid);
        group_alliance_wingmen.grouping->members.insert(mech->oid);
        group_alliance_all.grouping->members.insert(mech->oid);
        cout << "after first spawn\n";

        if (true) {
            if (!true) {
                mech->mountWeapon((char*) "LTorsor", new rWeaponExplosion);
                //mech->mountWeapon((char*) "RTorsor", new cExplosion);
            }
            if (true) {
                mech->mountWeapon((char*) "LTorsor", new rWeaponPlasmagun);
                //mech->mountWeapon((char*) "RTorsor", new cPlasmaGun);
            }
            if (true) {
                mech->mountWeapon((char*) "LLoArm", new rWeaponMachinegun);
                mech->mountWeapon((char*) "RLoArm", new rWeaponMachinegun);
            }
            if (!true) {
                mech->mountWeapon((char*) "RLoArm", new rWeaponHoming);
                mech->mountWeapon((char*) "RLoArm", new rWeaponRaybeam);
            }
            if (true) {
                //mech->mountWeapon((char*) "Center", new cRaybeam);
                mech->mountWeapon((char*) "RTorsor", new rWeaponRaybeam);
            }
        }
        cout << "after first weapons\n";
    }

    // Wing 1
    if (true) {
        float pos[3] = {p[0]+13 * 1, 0.1, p[2]-50 * 1};
        float rot[3] = {0, -210, 0};
        adjustHeight(planetmap, pos);
        cMech* mech = new cMech(pos, rot);
        assert(mech != NULL);

        mech->name = "Player's Wing 1";
        mech->nameable->name = "Joe";

        mech->addTag(cObject::BLUE);
        mech->tarcom->addEnemy(cObject::RED);
        mech->tarcom->addEnemy(cObject::DEAD, false);

        world->spawnObject(mech);
        group_alliance_wingmen.grouping->members.insert(mech->oid);
        group_alliance_all.grouping->members.insert(mech->oid);
        bool patrol = true;
        mech->controlled->controller->pushFollowLeader(player->oid, patrol);
        mech->mountWeapon((char*) "LLoArm", new rWeaponMachinegun);
    }

    // Wing 2
    if (true) {
        float pos[3] = {p[0]+7 * 1, 0.1, p[2]-50 * 1};
        float rot[3] = {0, -210, 0};
        adjustHeight(planetmap, pos);
        cMech* mech = new cMech(pos, rot);
        assert(mech != NULL);

        mech->name = "Player's Wing 2";
        mech->nameable->name = "Jack";

        mech->addTag(cObject::BLUE);
        mech->tarcom->addEnemy(cObject::RED);
        mech->tarcom->addEnemy(cObject::DEAD, false);

        world->spawnObject(mech);
        group_alliance_wingmen.grouping->members.insert(mech->oid);
        group_alliance_all.grouping->members.insert(mech->oid);
        bool patrol = true;
        mech->controlled->controller->pushFollowLeader(player->oid, patrol);
        mech->mountWeapon((char*) "RLoArm", new rWeaponMachinegun);
    }

    world->sendMessage(0, player->oid, group_alliance_all.oid, "RADIO", string("Stay alert there have been intruders!"));
    world->sendMessage(0, player->oid, group_alliance_wingmen.oid, "RADIO", string("Wingmen into formation!"));
    world->sendMessage(0, player->oid, group_alliance_player.oid, "RADIO", string("Stortebeker:\n  You guys search the town for offenders.\n  Then I'll give you more work!\n  I'll search the sourrounding."));

    return player;
}

void cOpenMission::initSkytideCity(cWorld* world, cPlanetmap* planetmap) {
    float loc[] = SKYTIDE;
    
    // Get natural height.
    float color[16];
    planetmap->getHeight(loc[0],loc[2], color);
    loc[1] = color[3];

    // Provide flat terrain at that height.
    cPlanetmap::sMod* mod = new cPlanetmap::sMod();
    mod->pos[0] = loc[0] + 50;
    mod->pos[1] = loc[1] + 0;
    mod->pos[2] = loc[2] + 50;
    mod->height = loc[1] - 0.009f;
    mod->range = 155;
    planetmap->mods.push_back(mod);

    //planetmap->getHeight(loc[0],loc[2], color);
    //cout << "height before: " << loc[1] << "  height after: " << color[3] << endl;

    capitalCity(loc[0], loc[1], loc[2], world);
    roundForrest(loc[0]+50, loc[1], loc[2]+50, world, 65, 150, 1);

    {
        OID group = group_alliance_player.oid;
        float pos[] = { loc[0]+48, loc[1] + 9, loc[2]+58 };
        float range[] = {60.5, 40.5, 60.5};
        string message = string("Skytide City");
        cObject* oob = new cAlert(pos, range, cAlert::rShape::BOX, "RADIO", message, group, &inc_sense, &exc_sense);
        oob->name = message;
        world->spawnObject(oob);
    }

    // Allied Patrol
    if (true) {
        float pos[3] = { loc[0]+10, loc[1], loc[2]-50 };
        float rot[3] = { 0, -210, 0 };
        adjustHeight(planetmap, pos);
        cMech* mech = new cMech(pos, rot);
        assert(mech != NULL);

        mech->name = "Mech/Stortebeker";
        mech->nameable->name = "Stortebeker";

        mech->addTag(cObject::BLUE);
        mech->tarcom->addEnemy(cObject::RED);
        mech->tarcom->addEnemy(cObject::DEAD, false);

        world->spawnObject(mech);
        group_alliance_all.grouping->members.insert(mech->oid);

        if (true) {
            //mech->mountWeapon((char*)"LLoArm",  new cMachineGun);
            //mech->mountWeapon((char*)"RLoArm",  new cMachineGun);
            mech->mountWeapon((char*) "RTorsor", new rWeaponHoming);
            mech->mountWeapon((char*) "LTorsor", new rWeaponHoming);
        }

        float d[] = { loc[0]+100, loc[1]+0, loc[1]-50 };
        adjustHeight(planetmap, d);
        mech->controlled->controller->pushGotoDestination(d);
        float c[] = { loc[0]+100, loc[1]+0, loc[2]+120 };
        adjustHeight(planetmap, c);
        mech->controlled->controller->pushGotoDestination(c);
        float b[] = { loc[0]-20, loc[1]+0, loc[2]+120 };
        adjustHeight(planetmap, b);
        mech->controlled->controller->pushGotoDestination(b, false);
        float a[] = { loc[0]-15, loc[1]+0, loc[2]-50 };
        adjustHeight(planetmap, a);
        mech->controlled->controller->pushGotoDestination(a);
        mech->controlled->controller->pushRepeatInstructions(4);
        mech->controlled->controller->pushWaitEvent(1000 * 2);
        //mech->mController->pushFollowLeader(player->mSerial, true);
    }

    // Primary Target
    if (true) {
        float pos[3] = {loc[0]+50, loc[1], loc[2]+50};
        adjustHeight(planetmap, pos);
        cMech* mech = new cMech(pos);
        assert(mech != NULL);

        mech->name = "Mech/OffendingVillain";
        mech->nameable->name = "Offending Villain";

        mech->addTag(cObject::RED);
        mech->tarcom->addEnemy(cObject::BLUE);
        mech->tarcom->addEnemy(cObject::DEAD, false);

        //ai->newState(cController::WAIT, (char*) NULL);
        //ai->newState(cController::TARGET, "Delta");
        //ai->newState(cController::ROUTE, "Attackroute");

        mVictory.push_back(mech);

        world->spawnObject(mech);
        group_enemies_all.grouping->members.insert(mech->oid);

        if (true) {
            //mech->mountWeapon((char*)"LTorsor", new cExplosion);
            mech->mountWeapon((char*) "LLoArm", new rWeaponSparkgun);
            mech->mountWeapon((char*) "RLoArm", new rWeaponSparkgun);
            //mech->mountWeapon((char*)"RTorsor",  new cHomingMissile);
        }
    }

    // Secondary Target
    if (true) {
        float pos[3] = {loc[0]-50, loc[1], loc[2]+50};
        adjustHeight(planetmap, pos);
        cMech* mech = new cMech(pos);
        assert(mech != NULL);

        mech->name = "Mech/LurkingBandit";
        mech->nameable->name = "Lurking Bandit";

        mech->addTag(cObject::RED);
        mech->tarcom->addEnemy(cObject::BLUE);
        mech->tarcom->addEnemy(cObject::DEAD, false);

        //ai->newState(cController::ENGAGE, "Charly");
        //ai->newState(cController::ROUTE, navs);

        //gCameraEntity = mech;
        world->spawnObject(mech);
        group_enemies_all.grouping->members.insert(mech->oid);

        if (true) {
            mech->mountWeapon((char*) "LLoArm", new rWeaponSparkgun);
            //mech->mountWeapon((char*)"LLoArm",  new cRaybeam);
            //mech->mountWeapon((char*)"RLoArm",  new cRaybeam);
            //mech->mountWeapon((char*)"RLoArm",  new cMachineGun);
            //mech->mountWeapon((char*)"RTorsor",  new cHomingMissile);
        }
    }
}

void cOpenMission::initStarcircleTown(cWorld* world, cPlanetmap* planetmap) {
    float loc[] = STARCIRCLE;

    // Get natural height.
    float color[16];
    planetmap->getHeight(loc[0],loc[2], color);
    loc[1] = color[3];
    loc[1] = 0.0;
    cout << "STARCIRCLE height: " << loc[1] << endl;
    
    cPlanetmap::sMod* mod = new cPlanetmap::sMod();
    mod->pos[0] = loc[0];
    mod->pos[1] = loc[1];
    mod->pos[2] = loc[2];
    mod->height = loc[1] - 0.009f;
    mod->range = 80;
    planetmap->mods.push_back(mod);

    {
        OID group = group_alliance_player.oid;
        float pos[] = { loc[0], loc[1]+9, loc[2] };
        float range[] = {50.5, 40.5, 50.5};
        string message = string("Starcircle Town\n(currently under bandit control)");
        cObject* oob = new cAlert(pos, range, cAlert::rShape::BOX, "RADIO", message, group, &inc_sense, &exc_sense);
        oob->name = message;
        world->spawnObject(oob);
    }
    
    smallSettling(loc[0], loc[1], loc[2], world, 15);
    roundForrest(loc[0],loc[1], loc[2], world, 7*2, 22*2, 3);
    
    if (true) {
        smallArmy(loc[0], loc[1]+5, loc[2], world, "Bandit Leader", 1, false, 1);
        smallArmy(loc[0]-48, loc[1]+5, loc[2]+17, world, "Bandits A", 3, false, 3);
        smallArmy(loc[0]-50, loc[1]+5, loc[2]-50, world, "Bandits B", 2, false, 2);
    }
    
    {
        float pos[] = STARCIRCLE;
        float range[] = {10, 3, 10};
        OID group = group_alliance_player.oid;
        std::string type = "RADIO";
        std::string message = std::string("EVACUATION ZONE REACHED, FINISHING MISSION!");
        bool positive = true;
        bool posedge = true;
        OID delay = 0;
        cObject* oob = new cAlert(pos, range, cAlert::rShape::BOX, type, message, group, &inc_sense, &exc_sense, positive, posedge, delay);
        world->spawnObject(oob);
    }
}

void cOpenMission::initAcroloidMines(cWorld* world, cPlanetmap* planetmap) {
    float loc[] = ACROLOID;

    // Get natural height.
    float color[16];
    planetmap->getHeight(loc[0],loc[2], color);
    loc[1] = color[3];

    cPlanetmap::sMod* mod = new cPlanetmap::sMod();
    mod->pos[0] = loc[0];
    mod->pos[1] = loc[1];
    mod->pos[2] = loc[2];
    mod->height = loc[1] - 0.009f;
    mod->range = 30;
    planetmap->mods.push_back(mod);

    {
        OID group = group_alliance_player.oid;
        float pos[] = { loc[0], loc[1]+9, loc[2] };
        float range[] = {0.5, 0.5, 0.5};
        string message = string("Acroloid Surface Mining Ltd. Co.\n(under construction)");
        cObject* oob = new cAlert(pos, range, cAlert::rShape::BOX, "RADIO", message, group, &inc_sense, &exc_sense);
        oob->name = message;
        world->spawnObject(oob);
    }
}

void cOpenMission::initCollapsiumFactory(cWorld* world, cPlanetmap* planetmap) {
    float loc[] = COLLAPSIUM;

    // Get natural height.
    float color[16];
    planetmap->getHeight(loc[0],loc[2], color);
    loc[1] = color[3];

    cPlanetmap::sMod* mod = new cPlanetmap::sMod();
    mod->pos[0] = loc[0];
    mod->pos[1] = loc[1];
    mod->pos[2] = loc[2];
    mod->height = loc[1] - 0.009f;
    mod->range = 30;
    planetmap->mods.push_back(mod);

    {
        OID group = group_alliance_player.oid;
        float pos[] = { loc[0], loc[1]+9, loc[2] };
        float range[] = {0.5, 0.5, 0.5};
        string message = string("Collapsium Factory Ltd.\n(under construction)");
        cObject* oob = new cAlert(pos, range, cAlert::rShape::BOX, "RADIO", message, group, &inc_sense, &exc_sense);
        oob->name = message;
        world->spawnObject(oob);
    }
}

void cOpenMission::initJurataJail(cWorld* world, cPlanetmap* planetmap) {
    float loc[] = JURATA;

    // Get natural height.
    float color[16];
    planetmap->getHeight(loc[0],loc[2], color);
    loc[1] = color[3];

    cPlanetmap::sMod* mod = new cPlanetmap::sMod();
    mod->pos[0] = loc[0];
    mod->pos[1] = loc[1];
    mod->pos[2] = loc[2];
    mod->height = loc[1] - 0.009f;
    mod->range = 30;
    planetmap->mods.push_back(mod);

    {
        OID group = group_alliance_player.oid;
        float pos[] = { loc[0], loc[1]+9, loc[2] };
        float range[] = {0.5, 0.5, 0.5};
        string message = string("Jurata Jail Ltd.\n- Service Company -\n(under construction)");
        cObject* oob = new cAlert(pos, range, cAlert::rShape::BOX, "RADIO", message, group, &inc_sense, &exc_sense);
        oob->name = message;
        world->spawnObject(oob);
    }
}

void cOpenMission::initSpadenixFactory(cWorld* world, cPlanetmap* planetmap) {
    float loc[] = SPADENIX;

    // Get natural height.
    float color[16];
    planetmap->getHeight(loc[0],loc[2], color);
    loc[1] = color[3];

    cPlanetmap::sMod* mod = new cPlanetmap::sMod();
    mod->pos[0] = loc[0];
    mod->pos[1] = loc[1];
    mod->pos[2] = loc[2];
    mod->height = loc[1] - 0.009f;
    mod->range = 30;
    planetmap->mods.push_back(mod);

    {
        OID group = group_alliance_player.oid;
        float pos[] = { loc[0], loc[1]+9, loc[2] };
        float range[] = {0.5, 0.5, 0.5};
        string message = string("Spadenix Mechanical Factory Ltd.\n(under construction)");
        cObject* oob = new cAlert(pos, range, cAlert::rShape::BOX, "RADIO", message, group, &inc_sense, &exc_sense);
        oob->name = message;
        world->spawnObject(oob);
    }
}

void cOpenMission::initPyraNanoCorp(cWorld* world, cPlanetmap* planetmap) {
    float loc[] = PYRANANO;

    // Get natural height.
    float color[16];
    planetmap->getHeight(loc[0],loc[2], color);
    loc[1] = color[3];
    
    cPlanetmap::sMod* mod = new cPlanetmap::sMod();
    mod->pos[0] = loc[0];
    mod->pos[1] = loc[1];
    mod->pos[2] = loc[2];
    mod->height = loc[1] - 0.009f;
    mod->range = 100;
    planetmap->mods.push_back(mod);
    
    {
        OID group = group_alliance_player.oid;
        float pos[] = { loc[0], loc[1] + 9, loc[2] };
        float range[] = {50.5, 40.5, 50.5};
        string message = string("Pyra Nano Corporation\n(currently under bandit control)");
        cObject* oob = new cAlert(pos, range, cAlert::rShape::BOX, "RADIO", message, group, &inc_sense, &exc_sense);
        oob->name = message;
        world->spawnObject(oob);
    }

    pyramidBuilding(loc[0], loc[1], loc[2], world);
    roundForrest(loc[0], loc[1], loc[2], world, 28, 50, 3);
    smallArmy(loc[0]+15, loc[1], loc[2]-30, world, "Bandito", 3, false);
}

void cOpenMission::initPentaSpaceport(cWorld* world, cPlanetmap* planetmap) {
    float loc[] = SPACEPORT;

    // Get natural height.
    float color[16];
    planetmap->getHeight(loc[0],loc[2], color);
    loc[1] = color[3];
    
    cPlanetmap::sMod* mod = new cPlanetmap::sMod();
    mod->pos[0] = loc[0];
    mod->pos[1] = loc[1];
    mod->pos[2] = loc[2];
    mod->height = loc[1] - 0.009f;
    mod->range = 130;
    planetmap->mods.push_back(mod);
    
    {
        OID group = group_alliance_player.oid;
        float pos[] = { loc[0] + 5, loc[1] + 9, loc[2] + 3 };
        float range[] = {90.5, 40.5, 90.5};
        string message = string("Penta Interstellar Spaceport\n(currently under bandit control)");
        cObject* oob = new cAlert(pos, range, cAlert::rShape::BOX, "RADIO", message, group, &inc_sense, &exc_sense);
        oob->name = message;
        world->spawnObject(oob);
    }

    //roundForrest(loc[0],loc[1],loc[2], planet, 40*2, 70*2, 3);
    
    // The Space-Ship Port

    cBuilding* port = new cBuilding(-3 + loc[0], 3 + loc[1], 0 + loc[2], 9, 1, 3);
    world->spawnObject(port);
    //mDefeat.push_back(port);
    port = new cBuilding(-4 + loc[0], 0 + loc[1], -3 + loc[2], 12, 1, 5);
    world->spawnObject(port);
    //mDefeat.push_back(port);

    // Bay-Road

    loopi(6) {
        world->spawnObject(new cTile(9*i + loc[0], 0 + loc[1], 0 + loc[2], cTile::KIND_ROAD_EW));
    }

    // Generators

    loopi(5) {
        float a = (360 / 5) * i * 0.017453f;
        float r = 26;
        cBuilding* hub = new cBuilding(3 * r * sin(a) + loc[0], 0 + loc[1], 3 * r * cos(a) + loc[2], 3, 1, 2);
        world->spawnObject(hub);
        //mVictory.push_back(hub);
    }

    // Mechs
    
    float pos[3] = { loc[0], loc[1], loc[2] + 20};
    float rot[3] = {0, 0, 0};

    loopi(2) {
        pos[0] += 20;
        cMech* mech = new cMech(pos, rot);
        if (mech == NULL) throw "No memory for cMech in spacePortMechs.";
        mech->name = "Mech/SpacePortGuard_i";
        mech->nameable->name = "Space-Bandit";
        mech->addTag(cObject::RED);
        mech->tarcom->addEnemy(cObject::BLUE);
        mech->tarcom->addEnemy(cObject::DEAD, false);
        world->spawnObject(mech);
        mech->mountWeapon((char*) "Center", new rWeaponMachinegun);
    }
}

void cOpenMission::battleField(cWorld* world) {

    loopi(25) {
        float s = 30;
        float pos[3] = {(i % 6) * s - 50, 0, 20 + (i / 6) * s + (i / 6 % 2) * s / 2};
        cMech* mech = new cMech(pos);
        if (mech == NULL) throw "No memory for cMech in battleField.";

        mech->name = "Mech/Battlemech_i";
        mech->addTag(cObject::RED);
        mech->tarcom->addEnemy(cObject::BLUE);
        mech->tarcom->addEnemy(cObject::DEAD, false);

        world->spawnObject(mech);

        if (true) {
            mech->mountWeapon((char*) "LLoArm", new rWeaponMachinegun);
            mech->mountWeapon((char*) "RLoArm", new rWeaponMachinegun);
            mech->mountWeapon((char*) "RTorsor", new rWeaponMachinegun);
            mech->mountWeapon((char*) "LTorsor", new rWeaponMachinegun);
        }
    }

    loopi(20) {
        float pos[3] = {(i % 6) * 10 - 50, 0, -20 + (i / 6)*10 + (i / 6 % 2)*5};
        cMech* mech = new cMech(pos);
        if (mech == NULL) throw "No memory for cMech in battleField.";

        mech->name = "Mech/Fieldmech_i";
        mech->addTag(cObject::BLUE);
        mech->tarcom->addEnemy(cObject::RED);
        mech->tarcom->addEnemy(cObject::DEAD, false);

        world->spawnObject(mech);

        if (true) {
            mech->mountWeapon((char*) "LLoArm", new rWeaponSparkgun);
            mech->mountWeapon((char*) "RLoArm", new rWeaponSparkgun);
            mech->mountWeapon((char*) "RTorsor", new rWeaponSparkgun);
            mech->mountWeapon((char*) "LTorsor", new rWeaponRaybeam);
        }
    }
}

void cOpenMission::smallArmy(int wx, int wy, int wz, cWorld* world, const char* name, int n, bool blue, int wpn) {
    float r = 2;
    float a = 0;

    loopi(n) {
        float x = r * sin(a) + wx;
        float y = wy;
        float z = r * cos(a) + wz;

        float pos[3] = {x, y, z};
        float rot[3] = {0, rand() % 360, 0};
        cMech* mech = new cMech(pos, rot);
        if (mech == NULL) throw "No memory for cMech in smallArmy.";
        mech->name = name;
        mech->nameable->name = name;
        if (blue) {
            mech->addTag(cObject::BLUE);
            mech->tarcom->addEnemy(cObject::RED);
            mech->tarcom->addEnemy(cObject::DEAD, false);
        } else {
            mech->addTag(cObject::RED);
            mech->tarcom->addEnemy(cObject::BLUE);
            mech->tarcom->addEnemy(cObject::DEAD, false);
        }
        world->spawnObject(mech);
        if (wpn == 0) {
            mech->mountWeapon((char*) "Left", new rWeaponSparkgun);
        } else if (wpn == 1) {
            mech->mountWeapon((char*) "Right", new rWeaponPlasmagun);
        } else if (wpn == 2) {
            mech->mountWeapon((char*) "Left", new rWeaponHoming);
        } else if (wpn == 3) {
            mech->mountWeapon((char*) "LLoArm", new rWeaponMachinegun);
        }

        a += M_PI * 0.2f;
        r += 0.4f * 6;
    }
}

void cOpenMission::smallSettling(int wx, int wy, int wz, cWorld* world, int n) {
    float r = 2;
    float a = 0;

    loopi(n) {
        float x = wx + 3 * r * sin(a);
        float y = wy;
        float z = wz + 3 * r * cos(a);
        world->spawnObject(new cBuilding(x, y, z, 1, 1 + (n / (3 * (i + 1))), 2));
        a += M_PI * 0.2;
        r += 0.4;
    }
    //world->spawnObject(new cScatterfield(wx*3.0f, wy, wz*3.0f, 25, 0.2));
}

void cOpenMission::roundForrest(int wx, int wy, int wz, cWorld* world, int r1, int r2, int nmax) {

    struct plantplan {
        int n, seed, type, age;
    } pp[] = {
        { nmax, 1234, 0, 3},
        { nmax * 9 / 10, 1236, 0, 3},
        { nmax * 8 / 10, 1234, 1, 2},
        { nmax * 7 / 10, 1237, 1, 2},
        { nmax * 6 / 10, 1234, 2, 3},
        { nmax * 5 / 10, 1236, 2, 3},
        { nmax * 1 / 10, 1234, 0, 4},
        { nmax * 1 / 10, 1234, 1, 5},
    };

    loopj(sizeof (pp) / sizeof (plantplan)) {

        loopi(pp[j].n) {
            float a = (rand() % 360) * 0.017453f;
            float b = r1 + (rand() % r2);
            float pos[3] = {wx + sin(a) * b, 0 + wy, wz + cos(a) * b};
            float rot[3] = {0, 10 * (rand() % 36), 0};
            world->spawnObject(new cTree(pos, rot, pp[j].seed, pp[j].type, pp[j].age));
        };
    }
}

void cOpenMission::capitalCity(int wx, int wy, int wz, cWorld* world) {
    const int b = 0;
    const int width = 11;
    const int height = 11;
    int buildings[][width] = {
        { b, b, b, b, b, b, b, b, b, b, b},
        { b, 4, 4, 0, 6, 6, 0, 3, 4, 3, b},
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, b},
        { b, 3, 3, 0, 5, 12, 0, 4, 6, 6, b},
        { b, 4, 6, 0, 8, 10, 0, 4, 11, 11, b},
        { b, 6, 4, 0, 0, 0, 0, 0, 0, 0, b},
        { b, 3, 3, 0, 11, 9, 0, 7, 5, 3, b},
        { b, 0, 0, 0, 0, 0, 0, 0, 0, 0, b},
        { b, 6, 6, 0, 5, 5, 0, 9, 4, 3, b},
        { b, 6, 6, 0, 5, 5, 0, 7, 6, 3, b},
        { b, b, b, b, b, b, b, b, b, b, b},
    };

    int roads[][width] = {
        { 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
        { 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0},
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        { 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1},
        { 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1},
        { 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1},
        { 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1},
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        { 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1},
        { 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1},
        { 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1},
    };

    // Ground-Tiles

    loopi(width) {

        loopj(height) {
            int center = roads[i][j];
            int east = (i == 0) ? 0 : roads[(i + width - 1) % width][j];
            int west = ((i + 1) == width) ? 0 : roads[(i + 1) % width][j];
            int north = (j == 0) ? 0 : roads[i][(j + height - 1) % height];
            int south = ((j + 1) == height) ? 0 : roads[i][(j + 1) % height];

            if (center == 1) {
                //if (east == 1 && west == 1 && north == 1 && south == 1) {
                if (east + west >= 1 && north + south >= 1) {
                    world->spawnObject(new cTile(9*i + wx, 0 + wy, 9*j + wz, cTile::KIND_ROAD_NEWS));
                } else if (west == 1 || east == 1) {
                    world->spawnObject(new cTile(9*i + wx, 0 + wy, 9*j + wz, cTile::KIND_ROAD_EW));
                } else if (south == 1 || north == 1) {
                    world->spawnObject(new cTile(9*i + wx, 0 + wy, 9*j + wz, cTile::KIND_ROAD_NS));
                }

            } else {
                //planet->spawnObject(new cTile(i +x,0 +y,j +z, cTile::KIND_CONCRETE));
            }
        }
    }

    // Stacked House-Tiles

    loopi(width) {

        loopj(height) {
            int h_ = buildings[i][j] / 100;
            int h = buildings[i][j] % 100;
            if (h > 0) {
                // Determine width of chunk.
                int a = i;
                while ((buildings[a][j] % 100) >= h && a < width) {
                    a++;
                }
                a--;

                // Determine height of chunk.
                int b = j + 1;
                while (b < height) {
                    int t;
                    for (t = i; (t <= a) && ((buildings[t][b] % 100) >= h); t++);
                    if (t <= a) break;
                    b++;
                }
                b--;
                //cout << "building at: " << i << " " << h_ << " " << j " - " << a << " " << h << " " << b "\n";
                for (int x = i; x <= a; x++) {
                    for (int y = j; y <= b; y++) {
                        //buildings[x][y] -= h;
                        //int old = buildings[x][y];
                        buildings[x][y] += h * 100 - h;
                        while (buildings[x][y] < 0) buildings[x][y] += 1;
                        //cout << old << " => " << buildings[x][y] << " " << (old + h*100 -h) << "\n";
                    }
                }
                world->spawnObject(new cBuilding(i * 3*3 + wx, h_*3 + wy, j * 3*3 + wz, (a - i + 1)*3, h, (b - j + 1)*3));
            }
        }
    }
}

void cOpenMission::pyramidBuilding(int x, int y, int z, cWorld* world) {
    int n = 5;

    loopi(n) {
        world->spawnObject(new cBuilding(i*3 + x, i*3 + y, i*3 + z, (n * 2) - 2 * i, 1, (n * 2) - 2 * i));
    }
}

//---------------------------------------------------------------------------


/*

#include <iostream>
using std::cout;
using std::endl;

//#include <expat.h>

struct Data {
    int depth;
};

void startElement(void *data, const char *el, const char **attr) {
    Data* d = (Data*) data;
    loopi(d->depth) cout << "\t";
    cout << el << endl;
    d->depth++;
}

void endElement(void *data, const char *el) {
    Data* d = (Data*) data;
    d->depth--;
}

void charHandler(void *data, const XML_Char *s, int len) {

}


    Data data;
    data.depth = 0;

    char buf[1024];
    XML_Parser parser = XML_ParserCreate(NULL);
    XML_SetUserData(parser, &data);
    XML_SetElementHandler(parser, startElement, endElement);
    XML_SetCharacterDataHandler(parser, charHandler);
    int done = 0;
    FILE* in = stdin;
    do {
        size_t len = fread(buf, 1, sizeof (buf), in);
        done = len < sizeof (buf);
        if (!XML_Parse(parser, buf, sizeof (buf), done)) {
            done = -1;
            // error.
        }
    } while (!done);
    XML_ParserFree(parser);
    if (in != stdin) fclose(in);
 */

