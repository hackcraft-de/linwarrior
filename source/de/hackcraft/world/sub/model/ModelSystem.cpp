#include "ModelSystem.h"
#include "de/hackcraft/world/World.h"

#include <iostream>
using std::cout;
using std::endl;

ModelSystem* ModelSystem::instance = NULL;

ModelSystem::ModelSystem() {
    instance = this;
}

ModelSystem::ModelSystem(const ModelSystem& orig) {
}

ModelSystem::~ModelSystem() {
}

ModelSystem* ModelSystem::getInstance() {
    return instance;
}


void ModelSystem::clusterObjects() {
    try {
        int j = 0;
        unsigned long geosize = geomap.size();
        geomap.clear();
        geomap.reserve(geosize * 2);

        for(IModel* model : models) {
            float px = model->getPosX();
            float pz = model->getPosZ();
            if (!finitef(px) || !finitef(pz)) {
                //mUncluster.push_back(model);
            } else {
                geomap[getGeokey(px, pz)].push_back(model);
            }
            j++;
        }
        
    } catch (char* s) {
        cout << "Could not cluster models: " << s << endl;
    }
}


void ModelSystem::animateObjects() {
    for (IModel* model : models) {
        model->animate(World::getInstance()->getTiming()->getSPF());
    }
}


void ModelSystem::transformObjects() {
    //cout << "transformObjects()\n";

    for (IModel* model: models) {
        glPushMatrix();
        model->transform();
        glPopMatrix();
    }
}


void ModelSystem::setupView(float* pos, float* ori) {
    // Find objects in visible range.
    viewdistance = World::getInstance()->getViewdistance();
    
    float* origin = pos;
    
    float min[] = {origin[0] - viewdistance, origin[2] - viewdistance};
    float max[] = {origin[0] + viewdistance, origin[2] + viewdistance};
    
    if (visobjects != NULL) {
        delete visobjects;
    }
    
    visobjects = getGeoInterval(min, max, true);
    assert(visobjects != NULL);
    //cout << "vis:" << objects->size() << " vs " << mObjects.size() << endl;
    
    visorigin[0] = pos[0];
    visorigin[1] = pos[1];
    visorigin[2] = pos[2];
}


void ModelSystem::drawSolid() {
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


void ModelSystem::drawEffect() {
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


OID ModelSystem::getGeokey(long x, long z) {
    OID xpart = ((OID) ((long(x))&0xFFFFFFFF)) >> 5;
    //cout << x << " ~ " << xpart << endl;
    OID zpart = ((OID) ((long(z))&0xFFFFFFFF)) >> 5;
    OID key = (xpart << 32) | zpart;
    //cout << key << endl;
    return key;
}


std::list<IModel*>* ModelSystem::getGeoInterval(float* min2f, float* max2f, bool addunclustered) {
    std::list<IModel*>* found = new std::list<IModel*>();
    const long f = 1 << 5;

    long ax = ((long) min2f[0] / f) * f;
    long az = ((long) min2f[1] / f) * f;
    long bx = ((long) max2f[0] / f) * f;
    long bz = ((long) max2f[1] / f) * f;

    int n = 0;
    for (long j = az - f * 1; j <= bz + f * 1; j += f) {
        for (long i = ax - f * 1; i <= bx + f * 1; i += f) {
            std::list<IModel*>* l = &(geomap[getGeokey(i, j)]);
            if (l != NULL) {
                //cout << "found " << l->size() << endl;
                found->insert(found->begin(), l->begin(), l->end());
            }
            n++;
        }
    }
    //cout << min2f[0] << ":" << min2f[1] << " - " << max2f[0] << ":" << max2f[1] << " " << n <<  " FOUND " << found->size() << endl;
    //cout << ax << ":" << az << " - " << bx << ":" << bz << " " << n <<  " FOUND " << found->size() << endl;

    return found;
}
