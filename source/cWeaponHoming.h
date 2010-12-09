/* 
 * File:     cWeaponHoming.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on 27. November 2010, 17:15
 */

#ifndef CHOMING_H
#define	CHOMING_H

#include "cWeapon.h"

/**
 * Small swarming homing missile.
 */
class cWeaponHoming : public cWeapon {
public:
    cWeaponHoming();

    virtual void fire(OID target);
    virtual void animate(float spf);
    virtual void drawSolid();
    virtual void drawEffect();
    virtual void drawHUD();
};

#endif	/* CHOMING_H */

