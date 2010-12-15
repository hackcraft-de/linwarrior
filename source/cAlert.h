/* 
 * File:     cAlert.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on February 14, 2010, 7:02 PM
 */

#ifndef _CALERT_H
#define	_CALERT_H

#include "cObject.h"
#include "cParticle.h"

#include <string>
#include <hash_set>
#include <set>

/**
 * Checks a defined area for certain intruding objects
 * (most likely the player). When such an intrusion is
 * detected a message is sent to a given group of objects.
 * Those receivers may then react on that message.
 *
 * Some ideas:
 * - Enemies powering up as soon as you enter a zone (pop WAIT state).
 * - Enemies collectively attacking the intruder (push ATTACK target).
 * - Allied backup moving in after you (push GOTO destination).
 * - Joining you when reaching a roundevous point (push FOLLOW target).
 * - Warning and mission abort when leaving mission area.
 *
 * Some further ideas:
 * Message aggregation and processing objects for and, or, xor, forwarding,
 * would allow extended mission scripting.
 * Likewise switching alerts on and off would present opportunities.
 *
 * A cone or pyramid for detection would make a nice surveilance camera.
 *
 */
class cAlert : public cObject {
public:
    static bool sDrawzone;
    struct rShape {

        enum Shapes {
            CYLINDER, BOX, SPHERE, CONE, MAX_SHAPES
        };
        int type;
        float center[3];
        float range[3];
    };
private:
    rShape shape;
    std::string msgtype;
    std::string msgtext;
    OID group;
    /// Lists object currently inside the tracked zone.
    std::set<OID> intruders;
    /// Use positive shape (true) or negative of shape (false) as zone?
    bool positive;
    /// Fire message when entering (true) or exiting (false) the zone?
    bool posedge;
    /// Fire message only once (true) or always when conditions are true?
    bool once;
    /// Set true when the message was fired at least once - initially false.
    bool fired;
    /// Delay for the delivery of the notification message.
    OID fusedelay;
    /// Reacting only on objects with such roles:
    std::set<OID> sensitivity;
public:

    cAlert(float* center, float* range, int shapetype, std::string msgtype, std::string msgtext, OID group, bool positive = true, bool posedge = true, bool once = false, OID fusedelay = 0);

    virtual float constrainParticle(float* worldpos, float radius = 0.0f, float* localpos = NULL, cObject* enactor = NULL);
    virtual void drawEffect();
};

#endif	/* _CALERT_H */

