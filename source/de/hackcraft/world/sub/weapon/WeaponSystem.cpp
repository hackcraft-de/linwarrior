#include "WeaponSystem.h"

#include "de/hackcraft/log/Logger.h"

#include "de/hackcraft/opengl/GL.h"

#include "de/hackcraft/world/IModel.h"
#include "de/hackcraft/world/World.h"

#include "de/hackcraft/world/sub/weapon/rTarcom.h"
#include "de/hackcraft/world/sub/weapon/rTarget.h"
#include "de/hackcraft/world/sub/weapon/rWeapon.h"
#include "de/hackcraft/world/sub/weapon/rWepcom.h"

#include <cassert>

Logger* WeaponSystem::logger = Logger::getLogger("de.hackcraft.world.sub.weapon.WeaponSystem");

WeaponSystem* WeaponSystem::instance = NULL;


WeaponSystem* WeaponSystem::getInstance() {
    return instance;
}


WeaponSystem::WeaponSystem() {
    instance = this;
    
    visobjects = new std::list<IModel*>();
    viewdistance = 500;
    vector_zero(visorigin);
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


rTarcom* WeaponSystem::findTarcomByEntity(OID entityID) {
    if (tarcomByEntity.find(entityID) != tarcomByEntity.end()) {
        return tarcomByEntity.at(entityID);
    }
    
    for(std::pair<OID,rTarcom*> p : tarcoms) {
        rTarcom* tarcom = p.second;
        
        if (tarcom->object->oid == entityID) {
            tarcomByEntity[entityID] = tarcom;
            return tarcom;
        }
    }
    
    return NULL;
}


rTarget* WeaponSystem::findTargetByEntity(OID entityID) {
    if (targetByEntity.find(entityID) != targetByEntity.end()) {
        return targetByEntity.at(entityID);
    }
    
    for(std::pair<OID,rTarget*> p : targets) {
        rTarget* target = p.second;
        
        if (target->object->oid == entityID) {
            targetByEntity[entityID] = target;
            return target;
        }
    }
    
    return NULL;
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
        logger->error() << "Could not cluster weapons: " << s << "\n";
    }
    
    try {
        int j = 0;
        
        geoTargets.clear();

        for(std::pair<OID,rTarget*> p : targets) {
            rTarget* model = p.second;
            float px = model->getPosX();
            float pz = model->getPosZ();
            if (!finitef(px) || !finitef(pz)) {
                //mUncluster.push_back(model);
            } else {
                geoTargets.put(px, pz, model);
            }
            j++;
        }
        
    } catch (char* s) {
        logger->error() << "Could not cluster targets: " << s << "\n";
    }
}


void WeaponSystem::animateObjects() {
    
    float spf = World::getInstance()->getTiming()->getSPF();
    
    for (std::pair<OID,rWepcom*> p : wepcoms) {
        rWepcom* model = p.second;
        model->prebind();
        model->animate(spf);
    }
    
    for (std::pair<OID,rWeapon*> p : weapons) {
        rWeapon* model = p.second;
        model->prebind();
        model->animate(spf);
    }
    
    for (std::pair<OID,rTarget*> p : targets) {
        rTarget* model = p.second;
        model->prebind();
        model->animate(spf);
    }
    
    for (std::pair<OID,rTarcom*> p : tarcoms) {
        rTarcom* model = p.second;
        model->prebind();
        model->animate(spf);
    }
}


void WeaponSystem::transformObjects() {
    //cout << "transformObjects()\n";

    for (std::pair<OID,rWeapon*> p : weapons) {
        IModel* model = p.second;
        GL::glPushMatrix();
        model->transform();
        GL::glPopMatrix();
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
    //cout << "vis:" << objects->size() << " vs " << mObjects.size() << "\n";
    
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
        
        GL::glPushMatrix();
        {
            model->drawSolid();
        }
        GL::glPopMatrix();
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
        
        GL::glPushMatrix();
        {
            model->drawEffect();
        }
        GL::glPopMatrix();
    }
}


std::list<rTarget*>* WeaponSystem::filterByTags(Entity* ex, std::set<OID>* rolemask, bool all, int maxamount, std::list<rTarget*>* objects) {
    std::list<rTarget*>* result = new std::list<rTarget*>;
    int amount = maxamount;
    if (objects == NULL) {
        return result;
    }

    for(rTarget* target: *objects) {
        if (amount == 0) break;
        
        assert(target->object != NULL);
        if (target->object->oid == 0) continue;
        if (target->object == ex) continue;
        
        // Filter Condition
        if (all) {
            if (!target->allTags(rolemask)) continue;
        } else {
            if (!target->anyTags(rolemask)) continue;
        }
        
        amount--;
        result->push_back(target);
    }
    return result;
}


std::list<rTarget*>* WeaponSystem::filterByRange(Entity* ex, float* origin, float minrange, float maxrange, int maxamount, std::list<rTarget*>* objects) {
    std::list<rTarget*>* result = new std::list<rTarget*>;
    int amount = maxamount;
    bool all = false;
    
    if (objects == NULL) {
        //objects = &mObjects;
        all = true;
        float maxrange_ = maxrange + 1;
        float min[] = {origin[0] - maxrange_, origin[2] - maxrange_};
        float max[] = {origin[0] + maxrange_, origin[2] + maxrange_};
        objects = geoTargets.getGeoInterval(min, max);
        //cout << "clustered:" << objects->size() << " vs " << mObjects.size() << "\n";
    }

    for(rTarget* target: *objects) {
        if (amount == 0) break;
        assert(target->object != NULL);
        if (target->object == ex) continue;
        if (target->object->oid == 0) continue;
        // Filter Condition
        float diff[3];
        vector_sub(diff, origin, target->pos0);
        float d2 = vector_dot(diff, diff);
        //float d = vector_distance(origin, object->mPos);
        float r1 = fmax(0.0f, minrange - target->radius);
        float r2 = maxrange + target->radius;
        if (d2 < r1 * r1 || d2 > r2 * r2) continue;
        amount--;
        result->push_back(target);
    }
    if (all) {
        // Add global objects to result.
        //result->insert(result->end(), mUncluster.begin(), mUncluster.end());
        // Cluster-Result is always freshly allocated for us => delete.
        delete objects;
    }
    return result;
}


