/**
 * File:     Entity.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 * 
 * Created on March 28, 2008, 10:23 PM (1999)
 *
 * Generic Game-Object with as much common
 * attributes as sensible and as needed
 * for various World-Filtering functions (Worldsense for Objects).
 */

#ifndef _CENTITYOBJECT_H
#define _CENTITYOBJECT_H

class Entity;

#include "de/hackcraft/lang/OID.h"

#include "de/hackcraft/world/Component.h"

#include "de/hackcraft/io/Pad.h"

#include "de/hackcraft/psi3d/math3d.h"
#include "de/hackcraft/psi3d/macros.h"

#ifdef __APPLE__
#include <OpenAL/al.h>
#include <ALUT/alut.h>
#else
#include <AL/al.h>
#include <AL/alut.h>
#endif

#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

// Some predefined groups (move to groups class file later).

// Roles indicating belonging to certain Social-Parties
#define FAC_RED "/faction/red"
#define FAC_BLUE "/faction/blue"
#define FAC_GREEN "/faction/green"
#define FAC_YELLOW "/faction/yellow"

// Roles indicating State
//  <= 75% hp
#define HLT_WOUNDED "/health/wounded"
// <= 50% hp
#define HLT_SERIOUS "/health/serious"
// <= 25% hp
#define HLT_CRITICAL "/health/critical"
// <= 0% hp, note that the later include the former.
#define HLT_DEAD "/health/dead"

// Special
#define PLR_HUMAN "/player/human"
#define PLR_AI "/player/ai"

#define FIELDOFS(attribute)     (((OID)&attribute) - ((OID)this))
#define ROLEPTR(attribute)      ((rRole* Entity::*) &attribute)
#define GETROLE(offset)         ((rRole*) *((OID*)( ((OID)this) + offset )))

class World;

/**
 * Obsolete description:
 * 
 * Generic Game-Object (Object with Roles) which
 * collects as much common attributes as needed
 * by the different roles and as needed by World-Filtering
 * (Worldsense for subclasses implementing the roles).
 * This object does again hold as much methods as needed
 * but without (real) implementation (which is subclass-responsibility).
 * Nevertheless every method has an empty body or a body returning
 * some default value, so that callers need not care about
 * the kind of the object (roles) too much.
 */
class Entity {
    friend class World;

public: // Basic Object attributes for managing.

    /** Unique Object ID, simulation date:time:deltacycle of spawn. */
    OID oid;
    /** Position for clustering, targeting and placing. */
    vec3 pos0;
    /** Radius for clustering and visibility. */
    float radius;
    /** Basic object orientation. */
    quat ori0;
    /** Age in seconds since spawn, updated by world before animate() call. */
    double seconds;
    /** Internal qualified name of the object. */
    std::string name;
    /** Input to the object through common input device if not null. */
    Pad* pad;

public: // Components

    /** Components of subsystems this entity is built from. */
    std::vector<Component*> components;

public: // Experimental Component "Managing"

    //static std::map<std::string, rComponent*> roleprotos;
    //static std::map<std::string, OID> roleoffsets;
    //static std::map<std::string, OID> roleids;
    //static std::map<OID, rRole*> roletypes;
    //std::map<OID, rRole*> roles;
    //std::set<OID> roleset;

public: // Object Tags

    /** Tags (IDs, Social-Roles, Parties, States...) this object has (=groups?). */
    std::set<OID> tags;

public:

    Entity() {
        name = "";
        oid = 0;
        pos0[0] = pos0[1] = pos0[2] = float_NAN;
        ori0[0] = ori0[1] = ori0[2] = 0.0f;
        ori0[3] = 1.0f;
        pad = NULL;
        radius = 0.1f;
        seconds = 0;
    }

    virtual ~Entity() {
    }

    /*
    bool anyRoles(std::set<OID>* test) {
        std::set<OID> result;
        std::set_intersection(roleset.begin(), roleset.end(), test->begin(), test->end(), std::inserter(result, result.begin()));
        return (!result.empty());
    }

    bool allRoles(std::set<OID>* test) {
        std::set<OID> result;
        std::set_intersection(roleset.begin(), roleset.end(), test->begin(), test->end(), std::inserter(result, result.begin()));
        return (result.size() == test->size());
    }

    // Check wether this Object is playing the given role.
    bool hasRole(OID role) {
        return (roleset.find(role) != roleset.end());
    }

    // Add a role this objects has to play.
    void addRole(OID role, rRole* roleobj = NULL) {
        roles[role] = roleobj;
        roleset.insert(role);
    }

    // Remove a role this object wont play anymore.
    void remRole(OID role, bool deleteobj = false) {
        if (deleteobj) delete roles[role];
        roles.erase(role);
        roleset.erase(role);
    }
     */

    /** Check whether this Object has at least one of the given tags. */
    bool anyTags(std::set<OID>* tagset) {
        std::set<OID> result;
        std::set_intersection(tags.begin(), tags.end(), tagset->begin(), tagset->end(), std::inserter(result, result.begin()));
        return (!result.empty());
    }

    /** Check whether this Object has all the given tags. */
    bool allTags(std::set<OID>* tagset) {
        std::set<OID> result;
        std::set_intersection(tags.begin(), tags.end(), tagset->begin(), tagset->end(), std::inserter(result, result.begin()));
        return (result.size() == tagset->size());
    }

    /** Check whether this Object has the given tag. */
    bool hasTag(OID tag) {
        return (tags.find(tag) != tags.end());
    }

    /** Add a tag to this object. */
    void addTag(OID tag) {
        tags.insert(tag);
    }

    /** Remove a tag from this object. */
    void remTag(OID tag) {
        tags.erase(tag);
    }

    /** Called right after object was spawned into the world. */
    virtual void spawn() {}

    /** Called right after object has been removed from object list and index. */
    virtual void frag() {}

    /** Called to glMultiply in the Object's camera matrix. */
    virtual void camera() {}

    /** Called to set Object's location, orientation and vel. as audio listener. */
    virtual void listener() {}

    /** Called to advance internal timers,animation state and pose, check gamepad. */
    virtual void animate(float dt) {}

    /** Deprecated, use animate? Called to precalculate necessary transformations - matrices, mountpoints, pos .. */
    virtual void transform() {}

    /** Called to render solid non-translucent parts of the object. */
    virtual void drawSolid() {}

    /** Called to render translucent object-parts and visual effects. */
    virtual void drawEffect() {}

    /** Called to render HUD-contents as seen when looking through the objects-eyes. */
    virtual void drawHUD() {}

    /**
     * Called to deal damage to the object with the damage arriving at or
     * from the given local position ie. relative to the object itself.
     * @localpos input location vector relative to the object.
     * @damage amount of damage to deal to the object.
     * @enactor the object dealing the damage to this object.
     */
    virtual void damage(float* localpos, float damage, Entity* enactor = NULL) {}

    /**
     * Called to constrain the given world position particle (with radius) to
     * the object boundary hull ie. place the worldpos to the nearest
     * boundary hull position if the worldpos is inside the object's hull.
     * localpos delivers in case of impact the local position relative
     * to the object of the corrected worldpos.
     * @worldpos input location vector in world coordinates.
     * @radius radius of the checked particle in world dimensions.
     * @localpos output adjusted location vector.
     * @return the intrusion depth.
     */
    virtual float constrain(float* worldpos, float radius = 0.0f, float* localpos = NULL, Entity* enactor = NULL) {
        return 0.0f;
    }

};

#endif
