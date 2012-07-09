#include "ModelSystem.h"

#include "de/hackcraft/log/Logger.h"

#include "de/hackcraft/world/World.h"

#include <cassert>

Logger* ModelSystem::logger = Logger::getLogger("de.hackcraft.world.sub.model.ModelSystem");

ModelSystem* ModelSystem::instance = NULL;

ModelSystem::ModelSystem() {
    instance = this;
    
    visobjects = new std::list<IModel*>();
    viewdistance = 500;
    vector_zero(visorigin);
}

ModelSystem::~ModelSystem() {
}

ModelSystem* ModelSystem::getInstance() {
    return instance;
}


void ModelSystem::clusterObjects() {
    try {
        int j = 0;
        
        geomap.clear();

        for(IModel* model : models) {
            float px = model->getPosX();
            float pz = model->getPosZ();
            if (!finitef(px) || !finitef(pz)) {
                //mUncluster.push_back(model);
            } else {
                geomap.put(px, pz, model);
            }
            j++;
        }
        
    } catch (char* s) {
        logger->error() << "Could not cluster models: " << s << "\n";
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
    
    visobjects = geomap.getGeoInterval(min, max);
    assert(visobjects != NULL);
    //cout << "vis:" << objects->size() << " vs " << mObjects.size() << "\n";
    
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

