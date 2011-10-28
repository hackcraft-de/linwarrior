/* 
 * File:     rWeaponPlasmagun.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on 27. November 2010, 17:11
 */

#ifndef RPLASMAGUN_H
#define	RPLASMAGUN_H

#include "rWeapon.h"

/**
 * Energetic Particle Rail-Gun.
 */
class rWeaponPlasmagun : public rWeapon {
public:
    rWeaponPlasmagun(Entity* obj = NULL);

    virtual void fire();
    virtual void animate(float spf);
    virtual void drawSolid();
    virtual void drawEffect();
    virtual void drawHUD();
};

#endif	/* CPLASMAGUN_H */

