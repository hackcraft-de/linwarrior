/* 
 * File:     cWeaponExplosion.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on 27. November 2010, 17:16
 */

#ifndef CEXPLOSION_H
#define	CEXPLOSION_H

#include "cWeapon.h"

/**
 * Explosion with dust, smoke, dirt and shrapnels.
 */
class cWeaponExplosion : public cWeapon {
public:
    cWeaponExplosion();

    virtual void fire(OID target);
    virtual void animate(float spf);
    virtual void drawSolid();
    virtual void drawEffect();
};

#endif	/* CEXPLOSION_H */

