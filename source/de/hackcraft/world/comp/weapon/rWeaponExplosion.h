/* 
 * File:     rWeaponExplosion.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on 27. November 2010, 17:16
 */

#ifndef REXPLOSION_H
#define	REXPLOSION_H

#include "rWeapon.h"

/**
 * Explosion with dust, smoke, dirt and shrapnels.
 */
class rWeaponExplosion : public rWeapon {
public:
    rWeaponExplosion(Entity* obj = NULL);

    virtual void fire();
    virtual void animate(float spf);
    virtual void drawSolid();
    virtual void drawEffect();
};

#endif	/* CEXPLOSION_H */

