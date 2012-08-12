#include "ModelSystem.h"

#include "de/hackcraft/log/Logger.h"

#include "de/hackcraft/opengl/GL.h"

#include "de/hackcraft/world/World.h"

#include "de/hackcraft/world/sub/model/rBillboard.h"
#include "de/hackcraft/world/sub/model/rRigged.h"
#include "de/hackcraft/world/sub/model/rScatter.h"
#include "de/hackcraft/world/sub/model/rTree.h"

#include <cassert>


Logger* ModelSystem::logger = Logger::getLogger("de.hackcraft.world.sub.model.ModelSystem");

ModelSystem* ModelSystem::instance = NULL;


ModelSystem* ModelSystem::getInstance() {
    return instance;
}


ModelSystem::ModelSystem() {
    instance = this;
    
    billboardsVisible = new std::list<rBillboard*>();
    riggedsVisible = new std::list<rRigged*>();
    scattersVisible = new std::list<rScatter*>();
    treesVisible = new std::list<rTree*>();
    
    viewdistance = 500;
    vector_zero(visorigin);
}


ModelSystem::~ModelSystem() {
}


void ModelSystem::add(rBillboard* model) {
    billboards.push_back(model);
}


void ModelSystem::add(rRigged* model) {
    riggeds.push_back(model);
}


void ModelSystem::add(rScatter* model) {
    scatters.push_back(model);
}


void ModelSystem::add(rTree* model) {
    trees.push_back(model);
}


void ModelSystem::clusterObjects() {
    try {
        int count = 0;
        
        billboardsGeodex.clear();
        riggedsGeodex.clear();
        scattersGeodex.clear();
        treesGeodex.clear();

        for(rBillboard* model : billboards) {
            float px = model->getPosX();
            float pz = model->getPosZ();
            if (finitef(px) && finitef(pz)) {
                billboardsGeodex.put(px, pz, model);
            }
            count++;
        }

        for(rRigged* model : riggeds) {
            float px = model->getPosX();
            float pz = model->getPosZ();
            if (finitef(px) && finitef(pz)) {
                riggedsGeodex.put(px, pz, model);
            }
            count++;
        }

        for(rScatter* model : scatters) {
            float px = model->getPosX();
            float pz = model->getPosZ();
            if (finitef(px) && finitef(pz)) {
                scattersGeodex.put(px, pz, model);
            }
            count++;
        }
        
        for(rTree* model : trees) {
            float px = model->getPosX();
            float pz = model->getPosZ();
            if (finitef(px) && finitef(pz)) {
                treesGeodex.put(px, pz, model);
            }
            count++;
        }
        
    } catch (char* s) {
        logger->error() << "Could not cluster models: " << s << "\n";
    }
}


void ModelSystem::animateObjects() {
    
    float spf = World::getInstance()->getTiming()->getSPF();
    
    for (rBillboard* model : billboards) {
        model->prebind();
        model->animate(spf);
        model->postbind();
    }
    
    for (rRigged* model : riggeds) {
        model->prebind();
        model->animate(spf);
        model->postbind();
    }
    
    for (rScatter* model : scatters) {
        model->prebind();
        model->animate(spf);
        model->postbind();
    }
    
    for (rTree* model : trees) {
        model->prebind();
        model->animate(spf);
        model->postbind();
    }
}


void ModelSystem::transformObjects() {
    //cout << "transformObjects()\n";

    // Currently just rigged models need transformation - can it be moved to animation?
    for (rRigged* model: riggeds) {
        GL::glPushMatrix();
        {
            model->transform();
        }
        GL::glPopMatrix();
    }
}


void ModelSystem::setupView(float* pos, float* ori) {
    // Find objects in visible range.
    viewdistance = World::getInstance()->getViewdistance();
    
    visorigin[0] = pos[0];
    visorigin[1] = pos[1];
    visorigin[2] = pos[2];
    
    float min[] = {visorigin[0] - viewdistance, visorigin[2] - viewdistance};
    float max[] = {visorigin[0] + viewdistance, visorigin[2] + viewdistance};
    
    if (billboardsVisible != NULL) {
        delete billboardsVisible;
    }
    billboardsVisible = billboardsGeodex.getGeoInterval(min, max);
    assert(billboardsVisible != NULL);
    
    if (riggedsVisible != NULL) {
        delete riggedsVisible;
    }
    riggedsVisible = riggedsGeodex.getGeoInterval(min, max);
    assert(riggedsVisible != NULL);
    
    if (scattersVisible != NULL) {
        delete scattersVisible;
    }
    scattersVisible = scattersGeodex.getGeoInterval(min, max);
    assert(scattersVisible != NULL);
    
    if (treesVisible != NULL) {
        delete treesVisible;
    }
    treesVisible = treesGeodex.getGeoInterval(min, max);
    assert(treesVisible != NULL);
    
    //cout << "vis:" << objects->size() << " vs " << mObjects.size() << "\n";
}


void ModelSystem::drawSolid() {
    //cout << "drawSolid()\n";

    float maxrange = viewdistance;
    float maxrange2 = maxrange*maxrange;

    for(rRigged* model: *riggedsVisible) {
        float x = model->getPosX() - visorigin[0];
        float z = model->getPosZ() - visorigin[2];
        float d2 = x * x + z * z;
        
        if (d2 > maxrange2) continue;
        
        GL::glPushMatrix();
        {
            model->drawSolid();
        }
        GL::glPopMatrix();
    }

    for(rTree* model: *treesVisible) {
        float x = model->getPosX() - visorigin[0];
        float z = model->getPosZ() - visorigin[2];
        float d2 = x * x + z * z;
        
        if (d2 > maxrange2) continue;
        
        GL::glPushMatrix();
        {
            model->drawSolid();
        }
        GL::glPopMatrix();
    }
}


void ModelSystem::drawEffect() {
    //cout << "drawEffect()\n";
    
    float maxrange = viewdistance;
    float maxrange2 = maxrange*maxrange;

    for(rBillboard* model: *billboardsVisible) {
        float x = model->getPosX() - visorigin[0];
        float z = model->getPosZ() - visorigin[2];
        float d2 = x * x + z * z;
        
        if (d2 > maxrange2) continue;
        
        GL::glPushMatrix();
        {
            model->drawEffect();
        }
        GL::glPopMatrix();
    }

    // These are here for debug bone rendering only.
    for(rRigged* model: *riggedsVisible) {
        float x = model->getPosX() - visorigin[0];
        float z = model->getPosZ() - visorigin[2];
        float d2 = x * x + z * z;
        
        if (d2 > maxrange2) continue;
        
        GL::glPushMatrix();
        {
            model->drawEffect();
        }
        GL::glPopMatrix();
    }

    for(rScatter* model: *scattersVisible) {
        float x = model->getPosX() - visorigin[0];
        float z = model->getPosZ() - visorigin[2];
        float d2 = x * x + z * z;
        
        if (d2 > maxrange2) continue;
        
        GL::glPushMatrix();
        {
            model->drawEffect();
        }
        GL::glPopMatrix();
    }

    for(rTree* model: *treesVisible) {
        float x = model->getPosX() - visorigin[0];
        float z = model->getPosZ() - visorigin[2];
        float d2 = x * x + z * z;
        
        if (d2 > maxrange2) continue;
        
        GL::glPushMatrix();
        {
            model->drawEffect();
        }
        GL::glPopMatrix();
    }
}

