#include "TriggerSystem.h"

#include "de/hackcraft/world/sub/trigger/rAlert.h"
#include "de/hackcraft/world/sub/trigger/rTrigger.h"
#include "de/hackcraft/world/World.h"


TriggerSystem* TriggerSystem::instance = NULL;


TriggerSystem* TriggerSystem::getInstance() {
    return instance;
}


TriggerSystem::TriggerSystem() {
    instance = this;
}


TriggerSystem::~TriggerSystem() {
}


void TriggerSystem::animateObjects() {
    
    float spf = World::getInstance()->getTiming()->getSPF();

    for (std::pair<OID,rTrigger*> p : triggers) {
        Component* component = p.second;
        
        component->prebind();
        component->animate(spf);
        component->postbind();
    }

    for (std::pair<OID,rAlert*> p : alerts) {
        Component* component = p.second;
        
        component->prebind();
        component->animate(spf);
        component->postbind();
    }
    
    // OPTIMIZE: Should use spatial index.
    for (std::pair<OID,rAlert*> alert : alerts) {
        for (std::pair<OID,rTrigger*> trigger : triggers) {
            alert.second->detect(trigger.second);
        }
    }
}


void TriggerSystem::drawEffect() {
    for (std::pair<OID,rAlert*> alert : alerts) {
        alert.second->drawSystemEffect();
    }
    for (std::pair<OID,rTrigger*> trigger : triggers) {
        trigger.second->drawSystemEffect();
    }
}


void TriggerSystem::add(rAlert* alert) {
    alerts[alert->getId()] = alert;
}


void TriggerSystem::add(rTrigger* trigger) {
    triggers[trigger->getId()] = trigger;
}

