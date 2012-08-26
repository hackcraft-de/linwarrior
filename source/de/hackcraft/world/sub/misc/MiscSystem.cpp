#include "MiscSystem.h"

#include "de/hackcraft/log/Logger.h"

#include "de/hackcraft/world/World.h"

#include "de/hackcraft/world/sub/misc/rLightsource.h"
#include "de/hackcraft/world/sub/misc/rSoundsource.h"
#include "de/hackcraft/world/sub/misc/rInputsource.h"


Logger* MiscSystem::logger = Logger::getLogger("de.hackcraft.world.sub.misc.MiscSystem");

MiscSystem* MiscSystem::instance = NULL;


MiscSystem* MiscSystem::getInstance() {
    return instance;
}


MiscSystem::MiscSystem() {
    instance = this;
}


MiscSystem::~MiscSystem() {
}


void MiscSystem::add(rSoundsource* component) {
    
    soundsources.push_back(component);
    soundsourcesIndex[component->getId()] = component;
}


void MiscSystem::add(rLightsource* component) {
    
    lightsources.push_back(component);
    lightsourcesIndex[component->getId()] = component;
}


void MiscSystem::add(rInputsource* component) {
    
    inputsources.push_back(component);
    inputsourcesIndex[component->getId()] = component;
}


rInputsource* MiscSystem::findInputsourceByEntity(OID entityID) {
    
    for(rInputsource* component : inputsources) {
        
        if (component->object->oid == entityID) {
            return component;
        }
    }
    
    return NULL;
}


void MiscSystem::animateObjects() {
    
    float spf = World::getInstance()->getTiming()->getSPF();
    
    for(Component* component : soundsources) {
        
        component->prebind();
        component->animate(spf);
        component->postbind();
    }
    
    for(Component* component : lightsources) {
        
        component->prebind();
        component->animate(spf);
        component->postbind();
    }
    
    for(Component* component : inputsources) {
        
        component->prebind();
        component->animate(spf);
        component->postbind();
    }
}


