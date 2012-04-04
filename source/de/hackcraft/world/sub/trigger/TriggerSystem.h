/* 
 * File:     TriggerSystem.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on March 22, 2012, 10:27 PM
 */

#ifndef TRIGGERSYSTEM_H
#define	TRIGGERSYSTEM_H

#include "rAlert.h"
#include "rTrigger.h"

#include "de/hackcraft/world/OID.h"
#include "de/hackcraft/world/Subsystem.h"

#include <map>

class TriggerSystem : public Subsystem {
public:
    TriggerSystem();
    TriggerSystem(const TriggerSystem& orig);
    virtual ~TriggerSystem();
    virtual void animateObjects();
    virtual void drawEffect();
    void addAlert(rAlert* alert) { alerts[alert->id] = alert; };
    void addTrigger(rTrigger* trigger) { triggers[trigger->id] = trigger; };
private:
    std::map<OID,rAlert*> alerts;
    std::map<OID,rTrigger*> triggers;
};

#endif	/* TRIGGERSYSTEM_H */

