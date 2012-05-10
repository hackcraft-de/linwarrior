#include "WeaponSystem.h"

WeaponSystem* WeaponSystem::instance = NULL;


WeaponSystem::WeaponSystem() {
    instance = this;
}

WeaponSystem::WeaponSystem(const WeaponSystem& orig) {
}

WeaponSystem::~WeaponSystem() {
}

WeaponSystem* WeaponSystem::getInstance() {
    return instance;
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
