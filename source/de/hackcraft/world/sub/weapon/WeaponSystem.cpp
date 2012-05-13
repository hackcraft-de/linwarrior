#include "WeaponSystem.h"

#include "de/hackcraft/world/World.h"

WeaponSystem* WeaponSystem::instance = NULL;


WeaponSystem::WeaponSystem() {
    instance = this;
    
    visobjects = NULL;
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

void WeaponSystem::clusterObjects() {
    try {
        int j = 0;
        
        geoWeapons.clear();

        for(std::pair<OID,rWeapon*> p : weapons) {
            rWeapon* model = p.second;
            float px = model->getPosX();
            float pz = model->getPosZ();
            if (!finitef(px) || !finitef(pz)) {
                //mUncluster.push_back(model);
            } else {
                geoWeapons.put(px, pz, model);
            }
            j++;
        }
        
    } catch (char* s) {
        std::cout << "Could not cluster models: " << s << "\n";
    }
    
    try {
        int j = 0;
        
        getTargets.clear();

        for(std::pair<OID,rTarget*> p : targets) {
            rTarget* model = p.second;
            float px = model->getPosX();
            float pz = model->getPosZ();
            if (!finitef(px) || !finitef(pz)) {
                //mUncluster.push_back(model);
            } else {
                getTargets.put(px, pz, model);
            }
            j++;
        }
        
    } catch (char* s) {
        std::cout << "Could not cluster models: " << s << "\n";
    }
}


void WeaponSystem::animateObjects() {
    
    float spf = World::getInstance()->getTiming()->getSPF();
    
    for (std::pair<OID,rWeapon*> p : weapons) {
        rWeapon* model = p.second;
        model->animate(spf);
    }
    
    for (std::pair<OID,rTarget*> p : targets) {
        rTarget* model = p.second;
        model->animate(spf);
    }
    
    for (std::pair<OID,rTarcom*> p : tarcoms) {
        rTarcom* model = p.second;
        model->animate(spf);
    }
    
    for (std::pair<OID,rWepcom*> p : wepcoms) {
        rWepcom* model = p.second;
        model->animate(spf);
    }
}


void WeaponSystem::transformObjects() {
    //cout << "transformObjects()\n";

    for (std::pair<OID,rWeapon*> p : weapons) {
        IModel* model = p.second;
        glPushMatrix();
        model->transform();
        glPopMatrix();
    }
}


void WeaponSystem::setupView(float* pos, float* ori) {
    // Find objects in visible range.
    viewdistance = World::getInstance()->getViewdistance();
    
    float* origin = pos;
    
    float min[] = {origin[0] - viewdistance, origin[2] - viewdistance};
    float max[] = {origin[0] + viewdistance, origin[2] + viewdistance};
    
    if (visobjects != NULL) {
        delete visobjects;
    }
    
    visobjects = geoWeapons.getGeoInterval(min, max);
    //cout << "vis:" << objects->size() << " vs " << mObjects.size() << endl;
    
    visorigin[0] = pos[0];
    visorigin[1] = pos[1];
    visorigin[2] = pos[2];
}


void WeaponSystem::drawSolid() {
    //cout << "drawSolid()\n";
    
    float* origin = visorigin;

    float maxrange = viewdistance;
    float maxrange2 = maxrange*maxrange;

    for(IModel* model: *visobjects) {
        float x = model->getPosX() - origin[0];
        float z = model->getPosZ() - origin[2];
        float d2 = x * x + z * z;
        
        if (d2 > maxrange2) continue;
        
        glPushMatrix();
        {
            model->drawSolid();
        }
        glPopMatrix();
    }
}


void WeaponSystem::drawEffect() {
    //cout << "drawEffect()\n";
    
    float* origin = visorigin;

    float maxrange = viewdistance;
    float maxrange2 = maxrange*maxrange;

    for(IModel* model: *visobjects) {
        float x = model->getPosX() - origin[0];
        float z = model->getPosZ() - origin[2];
        float d2 = x * x + z * z;
        
        if (d2 > maxrange2) continue;
        
        glPushMatrix();
        {
            model->drawEffect();
        }
        glPopMatrix();
    }
}


