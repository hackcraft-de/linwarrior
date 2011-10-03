/* 
 * File:     rDamageable.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on April 3, 2011, 8:49 PM
 */

#ifndef RDAMAGEABLE_H
#define	RDAMAGEABLE_H

#include "rComponent.h"

#include "de/hackcraft/world/OID.h"

#include <map>

/**
 * Encapsulates attributes related to body damage and armor state.
 */
struct rDamageable : public rComponent {
public: // INPUT
    /// Radius for hitzone calculation.
    float radius;
    /// Height for hitzone calculation.
    float height;
public: // OUTPUT
    /// Marks object as still alife.
    bool alife;
    /// Marks object as dead.
    bool dead;
    /// Enumeration of entity body part units: Body, Legs, Left, Right.

    enum Parts {
        BODY = 0, // Some Objects only have this
        LEGS,
        LEFT,
        RIGHT,
        MAX_PARTS
    };
    /// Health-Points for Body-Parts.
    float hp[MAX_PARTS];
    // Diverse armor for Body-Parts.
    // float beam[MAX_PARTS];
    // float pierce[MAX_PARTS];
    // float splash[MAX_PARTS];
    // float heat[MAX_PARTS];
    // float sinks[MAX_PARTS];
    /// Some object that dealt some damage to the object. (hook o)
    OID disturber;
    float disturbance;
protected: // INTERNALS
    // Smarter disturbance with non-persistent memory.
    std::map<OID,float> damageAverage;
    std::map<OID,float> damageImpulse;
public:
    /// Constructor.
    rDamageable(cObject* obj = NULL);
    /// Copy Constructor.
    rDamageable(rDamageable * original);
    /// Clone this.
    virtual rComponent * clone();
    /// Apply damage to a hitzone, and return alife.
    virtual bool damage(float* localpos, float damage, cObject * enactor);
    /// Average disturbance (damage) and find max disturbing enactor.
    virtual void animate(float spf);
    /// Display damaging.
    virtual void drawHUD();
};

#endif	/* RDAMAGEABLE_H */

