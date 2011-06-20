/* 
 * File:     rWeaponRaybeam.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on 27. November 2010, 17:14
 */

#ifndef RRAYBEAM_H
#define	RRAYBEAM_H

#include "rWeapon.h"

/**
 * Laser Beam kind of Weapon.
 */
class rWeaponRaybeam : public rWeapon {
public:
    rWeaponRaybeam(cObject* obj = NULL);

    virtual void fire();
    virtual void animate(float spf);
    virtual void drawSolid();
    virtual void drawEffect();
    virtual void drawHUD();
};

#endif	/* CRAYBEAM_H */

