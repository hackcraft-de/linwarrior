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

#include "de/hackcraft/lang/OID.h"

#include "de/hackcraft/world/Subsystem.h"

#include <map>

class rAlert;
class rTrigger;

/**
 * Manages alerts and triggers for alerts.
 */
class TriggerSystem : public Subsystem {
public:
    TriggerSystem();
    virtual ~TriggerSystem();
    static TriggerSystem* getInstance();
    virtual void animateObjects();
    virtual void drawEffect();
    void add(rAlert* alert);
    void add(rTrigger* trigger);
private:
    static TriggerSystem* instance;
    std::map<OID,rAlert*> alerts;
    std::map<OID,rTrigger*> triggers;
};

#endif	/* TRIGGERSYSTEM_H */

