/* 
 * File:    WeaponSystem.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on March 22, 2012, 10:50 PM
 */


#ifndef WEAPONSYSTEM_H
#define	WEAPONSYSTEM_H

#include "rTarcom.h"
#include "rTarget.h"
#include "rWeapon.h"
#include "rWepcom.h"

#include "de/hackcraft/world/Subsystem.h"

#include <map>


class WeaponSystem : public Subsystem {
public:
    WeaponSystem();
    WeaponSystem(const WeaponSystem& orig);
    virtual ~WeaponSystem();
private:
    std::map<OID,rTarcom*> targetComputers;
    std::map<OID,rWepcom*> weaponComputers;
    std::map<OID,rWeapon*> weapons;
    std::map<OID,rTarget*> targets;
};

#endif	/* WEAPONSYSTEM_H */

