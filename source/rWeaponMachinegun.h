/* 
 * File:     rWeaponMachinegun.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on 27. November 2010, 17:10
 */

#ifndef RMACHINEGUN_H
#define	RMACHINEGUN_H

#include "rWeapon.h"

/**
 * Your usual Machine or Mini-Gun.
 */
class rWeaponMachinegun : public rWeapon {
public:
    rWeaponMachinegun(cObject* obj = NULL);

    virtual void fire(OID target);
    virtual void animate(float spf);
    virtual void drawSolid();
    virtual void drawEffect();
    virtual void drawHUD();
};


#endif	/* CMACHINEGUN_H */

