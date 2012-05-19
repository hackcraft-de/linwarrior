/* 
 * File:     rDamageable.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on April 3, 2011, 8:49 PM
 */

#ifndef RTARGET_H
#define	RTARGET_H

struct rTarget;

#include "de/hackcraft/world/sub/weapon/rTarcom.h"

#include "de/hackcraft/world/Component.h"

#include "de/hackcraft/world/OID.h"

#include "de/hackcraft/psi3d/math3d.h"

#include <map>
#include <set>
#include <algorithm>

/**
 * Encapsulates attributes related to body damage and armor state.
 */
struct rTarget : public Component {
public: // SYSTEM
    /// Identifier for this component (all uppercase letters without leading "r").
    static std::string cname;
    /// A unique random number (0-9999) to identify this component.
    static unsigned int cid;
public: // INPUT
    /// Base orientation (hook i).
    vec3 pos0;
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
    /// Tags (IDs, Social-Roles, Parties, States...) this target has (=groups?).
    std::set<OID> tags;
public:
    /// Constructor.
    rTarget(Entity* obj = NULL);
    /// Copy Constructor.
    rTarget(rTarget * original);
    /// Clone this.
    virtual Component * clone();
    /// Get id - address for now.
    virtual OID getId() { return (OID) this; }
    /// Apply damage to a hitzone, and return alife.
    virtual bool damage(float* localpos, float damage, Entity * enactor);
    /// Average disturbance (damage) and find max disturbing enactor.
    virtual void animate(float spf);
    /// Display damaging.
    virtual void drawHUD();
    
    virtual int getPosX() { return (int) pos0[0]; }
    virtual int getPosY() { return (int) pos0[1]; }
    virtual int getPosZ() { return (int) pos0[2]; }
    
    bool isEnemy(rTarcom* tarcom);
    
    /**
     * Check whether this target has at least one of the given tags.
     * @param tagset
     * @return 
     */
    bool anyTags(std::set<OID>* tagset);

    /**
     * Check whether this target has all the given tags.
     * @param tagset
     * @return 
     */
    bool allTags(std::set<OID>* tagset);

    /**
     * Check whether this target has the given tag.
     * @param tag
     * @return 
     */
    bool hasTag(OID tag);

    /**
     * Add a tag to this object.
     * @param tag
     */
    void addTag(OID tag);

    /**
     * Remove a tag from this object.
     * @param tag
     */
    void remTag(OID tag);
};

#endif	/* RDAMAGEABLE_H */

