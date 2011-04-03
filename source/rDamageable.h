/* 
 * File:   rDamageable.h
 * Author: benben
 *
 * Created on April 3, 2011, 8:49 PM
 */

#ifndef RDAMAGEABLE_H
#define	RDAMAGEABLE_H

#include "rComponent.h"
#include "OID.h"

/**
 * Encapsulates attributes related to body damage and armor state.
 */
struct rDamageable : public rComponent {
    /// Marks object as still alife.
    bool alife;
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
    /// Some object that dealt some damage to the object. (hook o) [(oid, damage)]?
    OID disturber;

    /// Constructor.
    rDamageable(cObject* obj = NULL);
    /// Copy Constructor.
    rDamageable(rDamageable * original);
    /// Clone this.
    virtual rComponent* clone();
    /// Apply damage to a hitzone, and return alife.
    virtual bool damage(int hitzone, float damage, cObject* enactor);
    /// Display damaging.
    virtual void drawHUD();
};

#endif	/* RDAMAGEABLE_H */

