/* 
 * File:     rWeaponSparkgun.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on 27. November 2010, 17:13
 */

#ifndef RSPARKGUN_H
#define	RSPARKGUN_H

#include "rWeapon.h"

/**
 * Big Flaming slow moving bullet type gun.
 */
class rWeaponSparkgun : public rWeapon {
public:
    rWeaponSparkgun(cObject* obj = NULL);

    virtual void fire(OID target);
    virtual void animate(float spf);
    virtual void drawSolid();
    virtual void drawEffect();
    virtual void drawHUD();
};

#endif	/* CSPARKGUN_H */

