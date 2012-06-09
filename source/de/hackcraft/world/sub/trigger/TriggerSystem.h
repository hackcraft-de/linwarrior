/* 
 * File:     TriggerSystem.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on March 22, 2012, 10:27 PM
 */

#ifndef TRIGGERSYSTEM_H
#define	TRIGGERSYSTEM_H

class TriggerSystem;

#include "de/hackcraft/world/OID.h"
#include "de/hackcraft/world/Subsystem.h"

#include "de/hackcraft/world/sub/trigger/rAlert.h"
#include "de/hackcraft/world/sub/trigger/rTrigger.h"

#include <map>

class TriggerSystem : public Subsystem {
public:
    TriggerSystem();
    TriggerSystem(const TriggerSystem& orig);
    virtual ~TriggerSystem();
    static TriggerSystem* getInstance();
    virtual void animateObjects();
    virtual void drawEffect();
    void add(rAlert* alert) { alerts[alert->id] = alert; };
    void add(rTrigger* trigger) { triggers[trigger->id] = trigger; };
private:
    static TriggerSystem* instance;
    std::map<OID,rAlert*> alerts;
    std::map<OID,rTrigger*> triggers;
};

#endif	/* TRIGGERSYSTEM_H */

