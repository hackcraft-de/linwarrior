/* 
 * File:     rWeaponHoming.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on 27. November 2010, 17:15
 */

#ifndef RHOMING_H
#define	RHOMING_H

#include "rWeapon.h"

/**
 * Small swarming homing missile.
 */
class rWeaponHoming : public rWeapon {
public:
    rWeaponHoming(cObject* obj = NULL);

    virtual void fire();
    virtual void animate(float spf);
    virtual void drawSolid();
    virtual void drawEffect();
    virtual void drawHUD();
};

#endif	/* CHOMING_H */

