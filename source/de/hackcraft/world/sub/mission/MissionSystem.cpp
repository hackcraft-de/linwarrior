#include "MissionSystem.h"

#include "de/hackcraft/world/sub/computer/rController.h"

#include "de/hackcraft/world/sub/model/ModelSystem.h"

#include "de/hackcraft/world/sub/weapon/WeaponSystem.h"

#include "de/hackcraft/world/Entity.h"
#include "de/hackcraft/world/object/cMech.h"


#include <cassert>
#include <ctime>
    
MissionSystem* MissionSystem::instance = NULL;

MissionSystem::MissionSystem() {
    instance = this;
    
    mState = 0;
}

void MissionSystem::advanceTime(int deltamsec) {
    checkConditions();
}

void MissionSystem::checkConditions() {
    
    OID deadTag = World::getInstance()->getGroup(HLT_DEAD);
    
    // Destroyed all necessary enemies?
    unsigned int n = 0;
    
    for (Entity* object: mVictory) {
        rTarget* target = WeaponSystem::getInstance()->findTargetByEntity(object->oid);
        if (target != NULL && target->hasTag(deadTag)) {
            n++;
        }
    }
    
    if (n != 0 && n == mVictory.size()) {
        onVictory();
        return;
    }
    
    // Destroyed any necessary friend?
    n = 0;
    for (Entity* object: mDefeat) {
        rTarget* target = WeaponSystem::getInstance()->findTargetByEntity(object->oid);
        if (target != NULL && target->hasTag(deadTag)) {
            n++;
        }
    }
    
    if (n != 0) {
        onDefeat();
        return;
    }
}

void MissionSystem::onVictory() {
    if (mState != -1) {
        mState = -1;
        World::getInstance()->sendMessage(0, 0, 0,
                "DISPLAY",
                std::string("   (+) Primary target fullfilled.\n   (+) You Won!\n   (+) Hit ESC to finish Mission\n")
                );
    }
}

void MissionSystem::onDefeat() {
    if (mState != -2) {
        mState = -2;
        World::getInstance()->sendMessage(0, 0, 0,
                "DISPLAY",
                std::string("   (-) You Lost!\n   (+) Hit ESC to finish Mission\n")
                );
    }
}

Entity* MissionSystem::init(World* world) {
    if (true) {
        world->getTiming()->setTime(12);
    } else {
        float hour;
        getSystemHour(hour);
        world->getTiming()->setTime(hour);
    }

    Entity* player = NULL;

    if (true) {
        float pos[3] = {-50, 0, -50};
        cMech* mech = new cMech(pos, NULL, "frogger");
        assert(mech != NULL);
        mech->name = "Playermech";

        mech->nameable->name = "Alpha";

        mech->controller->enabled = false; // Disable Autopilot.
        mech->target->addTag(World::getInstance()->getGroup(FAC_BLUE));
        mech->target->addTag(World::getInstance()->getGroup(PLR_HUMAN));
        mech->tarcom->addEnemy(World::getInstance()->getGroup(FAC_RED));
        mech->tarcom->addEnemy(World::getInstance()->getGroup(HLT_DEAD), false);

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

    //world->spawnObject(new rBuilding(0, 0, 0, 10, 3, 10));

    mVictory.push_back(player);
    return player;
}

