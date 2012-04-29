#include "WeaponSystem.h"

WeaponSystem::WeaponSystem() {
}

WeaponSystem::WeaponSystem(const WeaponSystem& orig) {
}

WeaponSystem::~WeaponSystem() {
}

void WeaponSystem::add(rTarcom* tarcom){
    tarcoms[tarcom->getId()] = tarcom;
}

void WeaponSystem::add(rWepcom* wepcom){
    wepcoms[wepcom->getId()] = wepcom;
}

void WeaponSystem::add(rWeapon* weapon){
    weapons[weapon->getId()] = weapon;
}

void WeaponSystem::add(rTarget* target){
    targets[target->getId()] = target;
}
