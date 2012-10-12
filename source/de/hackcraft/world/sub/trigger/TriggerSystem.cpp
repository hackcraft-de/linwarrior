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


void TriggerSystem::add(rAlert* alert) {
    alerts.push_back(alert);
    alertsIndex[alert->getId()] = alert;
}


void TriggerSystem::add(rTrigger* trigger) {
    triggers.push_back(trigger);
    triggersIndex[trigger->getId()] = trigger;
}


void TriggerSystem::animateObjects() {
    
    float spf = World::getInstance()->getTiming()->getSPF();

    for (rTrigger* component : triggers) {
        component->prebind();
        component->animate(spf);
        component->postbind();
    }

    for (rAlert* component : alerts) {
        component->prebind();
        component->animate(spf);
        component->postbind();
    }
    
    // OPTIMIZE: Should use spatial index.
    for (rAlert* alert : alerts) {
        for (rTrigger* trigger : triggers) {
            alert->detect(trigger);
        }
    }
}


void TriggerSystem::drawEffect() {
    
    for (rAlert* alert : alerts) {
        alert->drawSystemEffect();
    }
    
    for (rTrigger* trigger : triggers) {
        trigger->drawSystemEffect();
    }
}

