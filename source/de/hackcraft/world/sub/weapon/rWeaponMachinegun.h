/* 
 * File:     rWeaponMachinegun.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on 27. November 2010, 17:10
 */

#ifndef RMACHINEGUN_H
#define	RMACHINEGUN_H

class rWeaponMachinegun;

#include "de/hackcraft/world/sub/weapon/rWeapon.h"

/**
 * Your usual Machine or Mini-Gun.
 */
class rWeaponMachinegun : public rWeapon {
public: // SYSTEM
    /** Identifier for this component (all uppercase letters without leading "r"). */
    static std::string cname;
    /** A unique random number (0-9999) to identify this component. */
    static unsigned int cid;
public:
    rWeaponMachinegun(Entity* obj = NULL);

    virtual void fire();
    virtual void animate(float spf);
    virtual void drawSolid();
    virtual void drawEffect();
    virtual void drawHUD();
};


#endif	/* CMACHINEGUN_H */

