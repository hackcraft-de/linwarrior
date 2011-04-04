/* 
 * File:     rControlled.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on April 3, 2011, 8:44 PM
 */

#ifndef RCONTROLLED_H
#define	RCONTROLLED_H

#include "rComponent.h"
#include "OID.h"
#include "psi3d/math3d.h"

class cPad;
class cController;

/**
 * Encapsulates thoughtful steering behavior attributes and code.
 */
struct rControlled : public rComponent {
    /// Aim target.
    OID target;
    /// Movement target, set Not-a-Number for non-validity.
    vec3 destination;
    /// Human, AI/Remote Pad-Control over the Object.
    cPad* pad;
    /// Command Stack controlls the pad when enabled.
    cController* controller;

    /// Constructor.
    rControlled(cObject* obj = NULL);
    /// Copy Constructor.
    rControlled(rControlled * original);
    /// Clone this.
    virtual rComponent* clone();
    /// Process autopilot-controller.
    virtual void animate(float spf);
};

#endif	/* RCONTROLLED_H */

