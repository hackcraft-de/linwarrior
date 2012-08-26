/* 
 * File:    rInputsource.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on August 26, 2012, 2:18 PM
 */


#ifndef RINPUTSOURCE_H
#define	RINPUTSOURCE_H

#include "de/hackcraft/world/Component.h"

struct Pad;

/**
 * Configurable (not yet) input source.
 * Can be filled by binding other components (for auto-piloting)
 * or by a plugged in input device.
 */
class rInputsource : public Component {
public:
    bool fire;
    
    bool nextTarget;
    bool prevTarget;
    
    float nextCamera;
    
    float chassisUD;
    float chassisLR;
    
    float turretUD;
    float turretLR;
    
    float jetEnable;
    float driveEnable;
public:
    rInputsource(Entity* obj);
    virtual ~rInputsource();
    
    void setPad(Pad* pad);
    Pad* getPad();
    
    /** Copies values from pad to variables if applicable. */
    virtual void animate(float spf);
private:
    Pad* pad;

};

#endif	/* RINPUTSOURCE_H */

