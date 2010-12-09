/* 
 * File:     cWeaponSparkgun.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on 27. November 2010, 17:13
 */

#ifndef CSPARKGUN_H
#define	CSPARKGUN_H

#include "cWeapon.h"

/**
 * Big Flaming slow moving bullet type gun.
 */
class cWeaponSparkgun : public cWeapon {
public:
    cWeaponSparkgun();

    virtual void fire(OID target);
    virtual void animate(float spf);
    virtual void drawSolid();
    virtual void drawEffect();
    virtual void drawHUD();
};

#endif	/* CSPARKGUN_H */

