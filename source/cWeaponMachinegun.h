/* 
 * File:     cWeaponMachinegun.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on 27. November 2010, 17:10
 */

#ifndef CMACHINEGUN_H
#define	CMACHINEGUN_H

#include "cWeapon.h"

/**
 * Your usual Machine or Mini-Gun.
 */
class cWeaponMachinegun : public cWeapon {
public:
    cWeaponMachinegun();

    virtual void fire(OID target);
    virtual void animate(float spf);
    virtual void drawSolid();
    virtual void drawEffect();
    virtual void drawHUD();
};


#endif	/* CMACHINEGUN_H */

