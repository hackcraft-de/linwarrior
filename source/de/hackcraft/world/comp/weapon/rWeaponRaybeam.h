/* 
 * File:     rWeaponRaybeam.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on 27. November 2010, 17:14
 */

#ifndef RRAYBEAM_H
#define	RRAYBEAM_H

#include "rWeapon.h"

/**
 * Laser Beam kind of Weapon.
 */
class rWeaponRaybeam : public rWeapon {
public: // SYSTEM
    /// Identifier for this component (all uppercase letters without leading "r").
    static std::string cname;
    /// A unique random number (0-9999) to identify this component.
    static unsigned int cid;
public:
    rWeaponRaybeam(Entity* obj = NULL);

    virtual void fire();
    virtual void animate(float spf);
    virtual void drawSolid();
    virtual void drawEffect();
    virtual void drawHUD();
};

#endif	/* CRAYBEAM_H */

