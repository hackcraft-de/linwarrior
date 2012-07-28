#include "TriggerSystem.h"

#include "de/hackcraft/world/sub/trigger/rAlert.h"
#include "de/hackcraft/world/sub/trigger/rTrigger.h"

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
    // OPTIMIZE spatial index
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
    alerts[alert->id] = alert;
}


void TriggerSystem::add(rTrigger* trigger) {
    triggers[trigger->id] = trigger;
}

