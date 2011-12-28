#include "Mission.h"

#include "de/hackcraft/world/comp/computer/rController.h"

#include "de/hackcraft/world/comp/weapon/rWeapon.h"

#include "de/hackcraft/world/Entity.h"
#include "de/hackcraft/world/object/cMech.h"
#include "de/hackcraft/world/object/cBuilding.h"

#include "de/hackcraft/world/comp/model/rNameable.h"

#include <cassert>


void Mission::checkConditions() {
    // Destroyed all necessary enemies?
    unsigned int n = 0;
    for (Entity* object: mVictory) if (object->hasTag(World::instance->getGroup(HLT_DEAD))) n++;
    if (n != 0 && n == mVictory.size()) {
        onVictory();
        return;
    }
    // Destroyed any neccessary friend?
    n = 0;
    for (Entity* object: mDefeat) if (object->hasTag(World::instance->getGroup(HLT_DEAD))) n++;
    if (n != 0) {
        onDefeat();
        return;
    }
}

void Mission::onVictory() {
    if (mState != -1) {
        mState = -1;
        World::instance->sendMessage(0, 0, 0,
                "DISPLAY",
                std::string("   (+) Primary target fullfilled.\n   (+) You Won!\n   (+) Hit ESC to finish Mission\n")
                );
    }
}

void Mission::onDefeat() {
    if (mState != -2) {
        mState = -2;
        World::instance->sendMessage(0, 0, 0,
                "DISPLAY",
                std::string("   (-) You Lost!\n   (+) Hit ESC to finish Mission\n")
                );
    }
}

Entity* Mission::init(World* world) {
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
        mech->addTag(World::instance->getGroup(FAC_BLUE));
        mech->addTag(World::instance->getGroup(PLR_HUMAN));
        mech->tarcom->addEnemy(World::instance->getGroup(FAC_RED));
        mech->tarcom->addEnemy(World::instance->getGroup(HLT_DEAD), false);

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

