#include "TriggerSystem.h"

TriggerSystem::TriggerSystem() {
}

TriggerSystem::TriggerSystem(const TriggerSystem& orig) {
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

