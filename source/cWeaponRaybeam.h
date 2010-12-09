/* 
 * File:     cWeaponRaybeam.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on 27. November 2010, 17:14
 */

#ifndef CRAYBEAM_H
#define	CRAYBEAM_H

#include "cWeapon.h"

/**
 * Laser Beam kind of Weapon.
 */
class cWeaponRaybeam : public cWeapon {
public:
    cWeaponRaybeam();

    virtual void fire(OID target);
    virtual void animate(float spf);
    virtual void drawSolid();
    virtual void drawEffect();
    virtual void drawHUD();
};

#endif	/* CRAYBEAM_H */

