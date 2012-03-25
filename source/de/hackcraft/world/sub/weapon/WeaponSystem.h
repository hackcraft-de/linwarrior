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

#include <vector>


class WeaponSystem : public Subsystem {
public:
    WeaponSystem();
    WeaponSystem(const WeaponSystem& orig);
    virtual ~WeaponSystem();
private:
    std::vector<rTarcom> targetComputers;
    std::vector<rWepcom> weaponComputers;
    std::vector<rWeapon> weapons;
    std::vector<rTarget> targets;
};

#endif	/* WEAPONSYSTEM_H */

