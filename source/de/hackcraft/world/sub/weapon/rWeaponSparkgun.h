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
public: // SYSTEM
    /// Identifier for this component (all uppercase letters without leading "r").
    static std::string cname;
    /// A unique random number (0-9999) to identify this component.
    static unsigned int cid;
public:
    rWeaponSparkgun(Entity* obj = NULL);

    virtual void fire();
    virtual void animate(float spf);
    virtual void drawSolid();
    virtual void drawEffect();
    virtual void drawHUD();
};

#endif	/* CSPARKGUN_H */

