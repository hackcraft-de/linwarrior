/* 
 * File:     rWeaponExplosion.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on 27. November 2010, 17:16
 */

#ifndef REXPLOSION_H
#define	REXPLOSION_H

#include "de/hackcraft/world/sub/weapon/rWeapon.h"

/**
 * Explosion with dust, smoke, dirt and shrapnels.
 */
class rWeaponExplosion : public rWeapon {
public: // SYSTEM
    /** Identifier for this component (all uppercase letters without leading "r"). */
    static std::string cname;
    /** A unique random number (0-9999) to identify this component. */
    static unsigned int cid;
public:
    rWeaponExplosion(Entity* obj = NULL);

    virtual void fire();
    virtual void animate(float spf);
    virtual void drawSolid();
    virtual void drawEffect();
};

#endif	/* CEXPLOSION_H */

