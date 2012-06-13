#include "TriggerSystem.h"

TriggerSystem* TriggerSystem::instance = NULL;


TriggerSystem::TriggerSystem() {
    instance = this;
}

TriggerSystem::~TriggerSystem() {
}

TriggerSystem* TriggerSystem::getInstance() {
    return instance;
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
