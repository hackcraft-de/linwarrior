/**
 * File:     Entity.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 * 
 * Created on March 28, 2008, 10:23 PM (1999)
 */

#ifndef _CENTITYOBJECT_H
#define _CENTITYOBJECT_H

class Entity;

#include "de/hackcraft/lang/OID.h"

#include "de/hackcraft/world/Component.h"

#include "de/hackcraft/psi3d/math3d.h"
#include "de/hackcraft/psi3d/macros.h"

#include <algorithm>
#include <map>
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
 * Generic Game-Object (Object composed from components) which
 * collects as few common attributes as needed by World-Management.
 * 
 * Most methods and attributes should be removed eventually.
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

public: // Components

    /** Components of subsystems this entity is built from. */
    std::vector<Component*> components;

public:

    Entity() {
        name = "";
        oid = 0;
        pos0[0] = pos0[1] = pos0[2] = float_NAN;
        ori0[0] = ori0[1] = ori0[2] = 0.0f;
        ori0[3] = 1.0f;
        radius = 0.1f;
        seconds = 0;
    }

    virtual ~Entity() {
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
