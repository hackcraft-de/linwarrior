#include "cMission.h"

#include "main.h"

#include "cObject.h"
#include "cAlert.h"
#include "cPlanetmap.h"

#include <list>
#include <vector>
#include <cassert>
#include <time.h>
#include <iostream>
using std::cout;
using std::endl;

void cMission::checkConditions() {
    // Destroyed all necessary enemies?
    unsigned int n = 0;
    loopiv(mVictory) if (mVictory[i]->hasRole(DEAD)) n++;
    if (n != 0 && n == mVictory.size()) {
        onVictory();
        return;
    }
    // Destroyed any neccessary friend?
    n = 0;
    loopiv(mDefeat) if (mDefeat[i]->hasRole(DEAD)) n++;
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


// ------------------------------------------------------------

cObject* cEmptyMission::init(cWorld* world) {
    world->mBackground = new cBackground;

    if (true) {
        world->mTiming.setTime(12);
    } else {
        float hour;
        getSystemHour(hour);
        world->mTiming.setTime(hour);
    }

    cObject* player = NULL;

    if (true) {
        float pos[3] = {-50, 0, -50};
        cMech* mech = new cMech(pos);
        assert(mech != NULL);

        mech->entity->pad = new cPad;
        mech->nameable->name = "Alpha";

        mech->entity->controller->controllerEnabled = false; // Disable Autopilot.
        mech->addRole(BLUE);
        mech->addRole(HUMANPLAYER);
        mech->socialised->addEnemy(RED);

        player = mech;

        //cController* ai = new cController;
        //ai->newState(cController::TARGET, "Delta");
        //mech->mController = ai;

        world->spawnObject(mech);

        if (true) {

            //mech->mountWeapon((char*)"LTorsor", new cExplosion);
            mech->mountWeapon((char*) "LTorsor", new cWeaponPlasmagun);
            if (true) {
                mech->mountWeapon((char*) "LLoArm", new cWeaponMachinegun);
                //mech->mountWeapon((char*)"LTorsor",  new cMachineGun);
                //mech->mountWeapon((char*)"RTorsor",  new cMachineGun);
                mech->mountWeapon((char*) "RLoArm", new cWeaponMachinegun);
            }
            mech->mountWeapon((char*) "RTorsor", new cWeaponPlasmagun);
            //mech->mountWeapon((char*)"RLoArm",  new cHomingMissile);
            //mech->mountWeapon((char*)"RLoArm",  new cRaybeam);
        }
    }

    world->spawnObject(new cBuilding(0, 0, 0, 10, 3, 10));

    mVictory.push_back(player);
    return player;
}


// ------------------------------------------------------------

void cOpenMission::onVictory() {
    cMission::onVictory();
}

void cOpenMission::onDefeat() {
    cMission::onDefeat();
}

cObject* cOpenMission::init(cWorld* world) {
    srand(0);
    
    cout << "Setting up world background...\n";
    world->mBackground = new cBackground;

    cout << "Setting mission date and time...\n";
    if (!true) {
        world->mTiming.setTime(9);
    } else {
        float hour;
        getSystemHour(hour);
        hour = fmod(hour, 24);
        world->mTiming.setTime(hour, 60 * (hour - (int) hour));
    }

    cout << "Initialising planetary maps...\n";
    cPlanetmap* planetmap = new cPlanetmap();
    world->spawnObject(planetmap);

    cout << "Initialising call groups...\n";
    {
        cWorld::rGroup* group1 = new cWorld::rGroup();
        group1->mID = 1;
        group1->mName = "/alliance/player";
        world->mGroups[group1->mID] = group1;

        cWorld::rGroup* group2 = new cWorld::rGroup();
        group2->mID = 2;
        group2->mName = "/alliance/wingmen";
        world->mGroups[group2->mID] = group2;

        cWorld::rGroup* group3 = new cWorld::rGroup();
        group3->mID = 3;
        group3->mName = "/alliance/all";
        world->mGroups[group3->mID] = group3;

        cWorld::rGroup* group4 = new cWorld::rGroup();
        group4->mID = 4;
        group4->mName = "/enemies/all";
        world->mGroups[group4->mID] = group4;
    }

    cout << "Initialising Skytide City...\n";
    initSkytideCity(world, planetmap);

    cout << "Initialising Starcircle Town...\n";
    initStarcircleTown(world, planetmap);

    cout << "Initialising Penta Spaceport...\n";
    initPentaSpaceport(world, planetmap);

    cout << "Initialising Pyra Nano Corp...\n";
    initPyraNanoCorp(world, planetmap);

    if (true) {
        // Buggy/Experimental
        //planet->spawnObject(new cPadmap(-0,-0));
    }

    cout << "Initialising vehicles...\n";
    cObject* player = initPlayerParty(world);

    cout << "Initialising mission triggers...\n";
    {
        // Setup Leaving Mission Area Warning-Alert.
        {
            float pos[] = {30, -1, 30};
            float range[] = {90, 40, 90};
            OID group = 1;
            std::string type = "RADIO";
            std::string message = std::string("YOU ARE LEAVING THE MISSION AREA!");
            bool positive = false;
            bool posedge = true;
            OID delay = 0;
            cObject* oob = new cAlert(pos, range, cAlert::rShape::BOX, type, message, group, positive, posedge, delay);
            world->spawnObject(oob);
        }

        // Setup Contract Voided Aleart.
        {
            float pos[] = {30, -1, 30};
            float range[] = {110, 50, 110};
            OID group = 1;
            std::string type = "RADIO";
            std::string message = std::string("YOU HAVE LEFT THE MISSION AREA, CONTRACT VOIDED!");
            bool positive = false;
            bool posedge = true;
            OID delay = 0;
            cObject* oob = new cAlert(pos, range, cAlert::rShape::BOX, type, message, group, positive, posedge, delay);
            world->spawnObject(oob);
        }
        // Defeated when killed.
        mDefeat.push_back(player);
    }

    return player;
}

cObject* cOpenMission::initPlayerParty(cWorld* world) {
    cObject* player = NULL;

    // Player
    if (true) {
        float pos[3] = {-20, 0, -50};
        float rot[3] = {0, -170, 0};
        cMech* mech = new cMech(pos, rot);
        if (mech == NULL) throw "No memory for player mech!";
        player = mech;

        mech->entity->pad = new cPad;
        if (mech->entity->pad == NULL) throw "No memory for player pad!";
        mech->nameable->name = "Alpha";
        mech->entity->controller->controllerEnabled = false; // Disable Autopilot.
        mech->addRole(BLUE);
        mech->addRole(HUMANPLAYER);
        mech->socialised->addEnemy(RED);

        world->spawnObject(mech);
        world->mGroups[1]->mMembers.insert(mech->base->oid);
        world->mGroups[2]->mMembers.insert(mech->base->oid);
        world->mGroups[3]->mMembers.insert(mech->base->oid);
        //cout << "after first spawn\n";

        if (true) {
            if (!true) {
                mech->mountWeapon((char*) "LTorsor", new cWeaponExplosion);
                //mech->mountWeapon((char*) "RTorsor", new cExplosion);
            }
            if (true) {
                mech->mountWeapon((char*) "LTorsor", new cWeaponPlasmagun);
                //mech->mountWeapon((char*) "RTorsor", new cPlasmaGun);
            }
            if (true) {
                mech->mountWeapon((char*) "LLoArm", new cWeaponMachinegun);
                mech->mountWeapon((char*) "RLoArm", new cWeaponMachinegun);
            }
            if (!true) {
                mech->mountWeapon((char*) "RLoArm", new cWeaponHoming);
                mech->mountWeapon((char*) "RLoArm", new cWeaponRaybeam);
            }
            if (true) {
                //mech->mountWeapon((char*) "Center", new cRaybeam);
                mech->mountWeapon((char*) "RTorsor", new cWeaponRaybeam);
            }
        }
        //cout << "after first weapons\n";
    }

    // Wing 1
    if (true) {
        float pos[3] = {+13 * 1, 0, -50 * 1};
        float rot[3] = {0, -210, 0};
        cMech* mech = new cMech(pos, rot);
        assert(mech != NULL);

        mech->entity->pad = new cPad();
        mech->nameable->name = "Wing 1";

        mech->addRole(BLUE);
        mech->socialised->addEnemy(RED);

        world->spawnObject(mech);
        world->mGroups[2]->mMembers.insert(mech->base->oid);
        world->mGroups[3]->mMembers.insert(mech->base->oid);
        bool patrol = true;
        mech->entity->controller->pushFollowLeader(player->base->oid, patrol);
        mech->mountWeapon((char*) "LLoArm", new cWeaponMachinegun);
    }

    // Wing 2
    if (true) {
        float pos[3] = {+7 * 1, 0, -50 * 1};
        float rot[3] = {0, -210, 0};
        cMech* mech = new cMech(pos, rot);
        assert(mech != NULL);

        mech->entity->pad = new cPad();
        mech->nameable->name = "Wing 2";

        mech->addRole(BLUE);
        mech->socialised->addEnemy(RED);

        world->spawnObject(mech);
        world->mGroups[2]->mMembers.insert(mech->base->oid);
        world->mGroups[3]->mMembers.insert(mech->base->oid);
        bool patrol = true;
        mech->entity->controller->pushFollowLeader(player->base->oid, patrol);
        mech->mountWeapon((char*) "RLoArm", new cWeaponMachinegun);
    }

    world->sendMessage(0, player->base->oid, 3, "RADIO", string("Stay alert there have been intruders!"));
    world->sendMessage(0, player->base->oid, 2, "RADIO", string("Wingmen into formation!"));
    world->sendMessage(0, player->base->oid, 1, "RADIO", string("Objective: Your group has the order to search the town for possible offending forces. Good luck!"));

    return player;
}

void cOpenMission::initSkytideCity(cWorld* world, cPlanetmap* planetmap) {
    capitalCity(0, 0, 0, world);
    //roundForrest(50,0,50, planet, 65, 150, 5);

    {
        OID group = 1;
        float pos[] = {0, 1.76, 0};
        float color[16];
        planetmap->getCachedHeight(pos[0],pos[2], color);
        pos[1] += color[3];
        float range[] = {0.5, 0.5, 0.5};
        string message = string("Skytide City");
        cObject* oob = new cAlert(pos, range, cAlert::rShape::BOX, "RADIO", message, group);
        oob->nameable->name = message;
        world->spawnObject(oob);
    }

    // Allied Patrol
    if (true) {
        float pos[3] = {+10 * 1, 0, -50 * 1};
        float rot[3] = {0, -210, 0};
        cMech* mech = new cMech(pos, rot);
        assert(mech != NULL);

        mech->entity->pad = new cPad();
        mech->nameable->name = "City Patrol";

        mech->addRole(BLUE);
        mech->socialised->addEnemy(RED);

        world->spawnObject(mech);
        world->mGroups[3]->mMembers.insert(mech->base->oid);

        if (true) {
            //mech->mountWeapon((char*)"LLoArm",  new cMachineGun);
            //mech->mountWeapon((char*)"RLoArm",  new cMachineGun);
            mech->mountWeapon((char*) "RTorsor", new cWeaponHoming);
            mech->mountWeapon((char*) "LTorsor", new cWeaponHoming);
        }

        float d[] = {+100, 0, -50};
        mech->entity->controller->pushGotoDestination(d);
        float c[] = {+100, 0, +120};
        mech->entity->controller->pushGotoDestination(c);
        float b[] = {-20, 0, +120};
        mech->entity->controller->pushGotoDestination(b, false);
        float a[] = {-15, 0, -50};
        mech->entity->controller->pushGotoDestination(a);
        mech->entity->controller->pushRepeatInstructions(4);
        mech->entity->controller->pushWaitEvent(1000 * 2);
        //mech->mController->pushFollowLeader(player->mSerial, true);
    }

    // Primary Target
    if (true) {
        float pos[3] = {+50, 0, +50};
        cMech* mech = new cMech(pos);
        assert(mech != NULL);

        mech->entity->pad = new cPad();
        mech->nameable->name = "City Villain";

        mech->addRole(RED);
        mech->socialised->addEnemy(BLUE);

        //ai->newState(cController::WAIT, (char*) NULL);
        //ai->newState(cController::TARGET, "Delta");
        //ai->newState(cController::ROUTE, "Attackroute");

        mVictory.push_back(mech);

        world->spawnObject(mech);
        world->mGroups[4]->mMembers.insert(mech->base->oid);

        if (true) {
            //mech->mountWeapon((char*)"LTorsor", new cExplosion);
            mech->mountWeapon((char*) "LLoArm", new cWeaponSparkgun);
            mech->mountWeapon((char*) "RLoArm", new cWeaponSparkgun);
            //mech->mountWeapon((char*)"RTorsor",  new cHomingMissile);
        }
    }

    // Secondary Target
    if (true) {
        float pos[3] = {-50, 0, +50};
        cMech* mech = new cMech(pos);
        assert(mech != NULL);

        mech->entity->pad = new cPad();
        mech->nameable->name = "City Bandit";

        mech->addRole(RED);
        mech->socialised->addEnemy(BLUE);

        //ai->newState(cController::ENGAGE, "Charly");
        //ai->newState(cController::ROUTE, navs);

        //gCameraEntity = mech;
        world->spawnObject(mech);
        world->mGroups[4]->mMembers.insert(mech->base->oid);

        if (true) {
            mech->mountWeapon((char*) "LLoArm", new cWeaponSparkgun);
            //mech->mountWeapon((char*)"LLoArm",  new cRaybeam);
            //mech->mountWeapon((char*)"RLoArm",  new cRaybeam);
            //mech->mountWeapon((char*)"RLoArm",  new cMachineGun);
            //mech->mountWeapon((char*)"RTorsor",  new cHomingMissile);
        }
    }
}

void cOpenMission::initStarcircleTown(cWorld* world, cPlanetmap* planetmap) {
    {
        OID group = 1;
        float pos[] = {3*120, 8.76, 3*40};
        float color[16];
        planetmap->getCachedHeight(pos[0],pos[2], color);
        pos[1] += color[3];
        float range[] = {0.5, 0.5, 0.5};
        string message = string("Starcircle Town");
        cObject* oob = new cAlert(pos, range, cAlert::rShape::BOX, "RADIO", message, group);
        oob->nameable->name = message;
        world->spawnObject(oob);
    }
    
    smallSettling(120, 0, 40, world, 15);
    //roundForrest(120*3,0, 40*3, planet, 7*2, 22*2, 3);
    
    if (true) {
        smallArmy(120 * 3, 0, 40 * 3, world, "Ambusher", 1, false, 1);
        smallArmy(312, 0, 137, world, "Def A", 3, false, 3);
        smallArmy(310, 0, 71, world, "Def B", 2, false, 2);
    }
    
    {
        float pos[] = {3 * 120, 0, 3 * 40};
        float range[] = {10, 3, 10};
        OID group = 1;
        std::string type = "RADIO";
        std::string message = std::string("EVACUATION ZONE REACHED, FINISHING MISSION!");
        bool positive = true;
        bool posedge = true;
        OID delay = 0;
        cObject* oob = new cAlert(pos, range, cAlert::rShape::BOX, type, message, group, positive, posedge, delay);
        world->spawnObject(oob);
    }
}

void cOpenMission::initPyraNanoCorp(cWorld* world, cPlanetmap* planetmap) {
    {
        OID group = 1;
        float pos[] = {-75*3, 8.76, 90*3};
        float color[16];
        planetmap->getCachedHeight(pos[0],pos[2], color);
        pos[1] += color[3];
        float range[] = {0.5, 0.5, 0.5};
        string message = string("Pyra Nano Corporation");
        cObject* oob = new cAlert(pos, range, cAlert::rShape::BOX, "RADIO", message, group);
        oob->nameable->name = message;
        world->spawnObject(oob);
    }

    pyramidBuilding(-75, 0, 90, world);
    //roundForrest(-75*3,0,90*3, planet, 25*2, 30*2, 3);
    smallArmy(-65 * 3, 0, 80 * 3, world, "Pyramid Guard", 3, false);
}

void cOpenMission::initPentaSpaceport(cWorld* world, cPlanetmap* planetmap) {
    float x = -90;
    float y = 0;
    float z = -60;
    
    {
        OID group = 1;
        float pos[] = {x*3, 8.76, z*3};
        float color[16];
        planetmap->getCachedHeight(pos[0],pos[2], color);
        pos[1] += color[3];
        float range[] = {0.5, 0.5, 0.5};
        string message = string("Penta Interstellar Spaceport");
        cObject* oob = new cAlert(pos, range, cAlert::rShape::BOX, "RADIO", message, group);
        oob->nameable->name = message;
        world->spawnObject(oob);
    }

    //roundForrest(-90*3,0,-60*3, planet, 40*2, 70*2, 3);
    
    // The Space-Ship Port

    cBuilding* port = new cBuilding(-3 * 3 + x, 1 + y, 0 + z, 9, 1, 3);
    world->spawnObject(port);
    mDefeat.push_back(port);
    port = new cBuilding(-4 * 3 + x, 0 + y, -1 + z, 12, 1, 5);
    world->spawnObject(port);
    mDefeat.push_back(port);

    // Bay-Road

    loopi(6) {
        world->spawnObject(new cTile(i + x / 3, 0 + y, 0 + z / 3, cTile::KIND_ROAD_EW));
    }

    // Generators

    loopi(5) {
        float a = (360 / 5) * i * 0.017453f;
        float r = 26;
        cBuilding* hub = new cBuilding(r * sin(a) + x, 0 + y, r * cos(a) + z, 3, 1, 2);
        world->spawnObject(hub);
        mVictory.push_back(hub);
    }

    // Mechs
    
    float pos[3] = {3*x, y, 3*z + 2};
    float rot[3] = {0, 0, 0};

    loopi(2) {
        pos[0] += 20;
        cMech* mech = new cMech(pos, rot);
        if (mech == NULL) throw "No memory for cMech in spacePortMechs.";
        mech->entity->pad = new cPad;
        if (mech->entity->pad == NULL) throw "No memory for cPad in spacePortMechs.";
        mech->nameable->name = "Space Port Guard";
        mech->addRole(RED);
        mech->socialised->addEnemy(BLUE);
        world->spawnObject(mech);
        mech->mountWeapon((char*) "Center", new cWeaponMachinegun);
    }
}

void cOpenMission::battleField(cWorld* world) {

    loopi(25) {
        float s = 30;
        float pos[3] = {(i % 6) * s - 50, 0, 20 + (i / 6) * s + (i / 6 % 2) * s / 2};
        cMech* mech = new cMech(pos);
        if (mech == NULL) throw "No memory for cMech in battleField.";

        mech->entity->pad = new cPad();
        if (mech->entity->pad == NULL) throw "No memory for cPad in battleField.";
        mech->nameable->name = "X";
        mech->addRole(RED);
        mech->socialised->addEnemy(BLUE);

        world->spawnObject(mech);

        if (true) {
            mech->mountWeapon((char*) "LLoArm", new cWeaponMachinegun);
            mech->mountWeapon((char*) "RLoArm", new cWeaponMachinegun);
            mech->mountWeapon((char*) "RTorsor", new cWeaponMachinegun);
            mech->mountWeapon((char*) "LTorsor", new cWeaponMachinegun);
        }
    }

    loopi(20) {
        float pos[3] = {(i % 6) * 10 - 50, 0, -20 + (i / 6)*10 + (i / 6 % 2)*5};
        cMech* mech = new cMech(pos);
        if (mech == NULL) throw "No memory for cMech in battleField.";

        mech->entity->pad = new cPad();
        if (mech->entity->pad == NULL) throw "No memory for cPad in battleField.";
        mech->nameable->name = "Y";
        mech->addRole(BLUE);
        mech->socialised->addEnemy(RED);

        world->spawnObject(mech);

        if (true) {
            mech->mountWeapon((char*) "LLoArm", new cWeaponSparkgun);
            mech->mountWeapon((char*) "RLoArm", new cWeaponSparkgun);
            mech->mountWeapon((char*) "RTorsor", new cWeaponSparkgun);
            mech->mountWeapon((char*) "LTorsor", new cWeaponRaybeam);
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
        mech->entity->pad = new cPad;
        if (mech->entity->pad == NULL) throw "No memory for cPad in smallArmy.";
        mech->nameable->name = name;
        if (blue) {
            mech->addRole(BLUE);
            mech->socialised->addEnemy(RED);
        } else {
            mech->addRole(RED);
            mech->socialised->addEnemy(BLUE);
        }
        world->spawnObject(mech);
        if (wpn == 0) {
            mech->mountWeapon((char*) "Left", new cWeaponSparkgun);
        } else if (wpn == 1) {
            mech->mountWeapon((char*) "Right", new cWeaponPlasmagun);
        } else if (wpn == 2) {
            mech->mountWeapon((char*) "Left", new cWeaponHoming);
        } else if (wpn == 3) {
            mech->mountWeapon((char*) "Center", new cWeaponMachinegun);
        }

        a += M_PI * 0.2f;
        r += 0.4f * 6;
    }
}

void cOpenMission::smallSettling(int wx, int wy, int wz, cWorld* world, int n) {
    float r = 2;
    float a = 0;

    loopi(n) {
        float x = r * sin(a) + wx;
        float y = wy;
        float z = r * cos(a) + wz;
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
                    world->spawnObject(new cTile(i + wx, 0 + wy, j + wz, cTile::KIND_ROAD_NEWS));
                } else if (west == 1 || east == 1) {
                    world->spawnObject(new cTile(i + wx, 0 + wy, j + wz, cTile::KIND_ROAD_EW));
                } else if (south == 1 || north == 1) {
                    world->spawnObject(new cTile(i + wx, 0 + wy, j + wz, cTile::KIND_ROAD_NS));
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
                world->spawnObject(new cBuilding(i * 3 + wx * 3, h_ + wy * 3, j * 3 + wz * 3, (a - i + 1)*3, h, (b - j + 1)*3));
            }
        }
    }
}

void cOpenMission::pyramidBuilding(int x, int y, int z, cWorld* world) {
    int n = 5;

    loopi(n) {
        world->spawnObject(new cBuilding(i + x, i + y, i + z, (n * 2) - 2 * i, 1, (n * 2) - 2 * i));
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

