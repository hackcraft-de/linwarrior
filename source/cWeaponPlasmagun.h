/* 
 * File:     cWeaponPlasmagun.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on 27. November 2010, 17:11
 */

#ifndef CPLASMAGUN_H
#define	CPLASMAGUN_H

#include "cWeapon.h"

/**
 * Energetic Particle Rail-Gun.
 */
class cWeaponPlasmagun : public cWeapon {
public:
    cWeaponPlasmagun();

    virtual void fire(OID target);
    virtual void animate(float spf);
    virtual void drawSolid();
    virtual void drawEffect();
    virtual void drawHUD();
};

#endif	/* CPLASMAGUN_H */

