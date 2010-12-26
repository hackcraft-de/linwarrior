/**
 * File:     cObject.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 * 
 * Created on March 28, 2008, 10:23 PM (1999)
 *
 * Generic Game-Object with as much common
 * attributes as sensible and as needed
 * for various World-Filtering functions (Worldsense for Objects).
 */

#ifndef _COBJECT_H
#define _COBJECT_H

class cObject;
class cPad;
class cController;
class cWorld;

//#include "cWorld.h"
#include "OID.h"
#include "cMessage.h"
#include "cParticle.h"

#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <string>

#include <GL/glew.h>
#include <AL/al.h>
#include <AL/alut.h>


/**
 * Utility function to load uncompressed lower-left 24 or 32 bit tga bitmaps.
 * Note that zipped tga files may be smaller than other common compressed
 * formats when zipped => Distribution size is less than or similar to
 * when png or jpg is used, the installation size is larger but
 * the quality is as good as it gets - assuming 8 bits per pixel.
 * Besides tga is one of the simplest and fastest to load.
 * An enhancement may be to have *.tga.gz if at all.
 */
unsigned char* loadTGA(const char *fname, int *w, int* h, int* bpp);

/**
 * Utility function to save uncompressed lower-left 24 or 32 bit tga bitmaps.
 * Note that zipped tga files may be smaller than other common compressed
 * formats when zipped => Distribution size is less than or similar to
 * when png or jpg is used, the installation size is larger but
 * the quality is as good as it gets - assuming 8 bits per pixel.
 * Besides tga is one of the simplest and fastest to load.
 * An enhancement may be to have *.tga.gz if at all.
 */
int saveTGA(const char *fname, int w, int h, int bpp, unsigned char* image);


/**
 * Base-Class of all roles (mainy to just replace void*).
 * An instance of it may be used to flag a role as being "set" (future).
 * Note that a Role of an Object can be sub-classed individually and
 * replace the parent role.
 * A role can be understood as a set of object attributes reflecting
 * a certain part or aspect of the object.
 */
struct rRole {
    std::string role;

    rRole(std::string role) {
        this->role = role;
    }

    /// Enumeration of roles an object may assume.
    enum Roles {
        // Roles indicating Attributes
        BASE,
        NAMEABLE,
        SOCIALISED,
        TRACEABLE,
        COLLIDEABLE,
        DAMAGEABLE,
        CONTROLLED,
        GROUPING,

        // Roles indicating Kind of Objects
        MECH,
        BUILDING,
        TREE,
        IMMOBILE,

        // Roles indicating belonging to certain Social-Parties
        RED,
        BLUE,
        GREEN,
        YELLOW,
        CIVIL,

        // Roles indicating State
        WOUNDED, //  <= 75% hp
        SERIOUS, // <= 50% hp
        CRITICAL, // <= 25% hp
        DEAD, // <= 0% hp, note that the later include the former.

        // Special Roles
        HUMANPLAYER,
        FRAGGED,

        MAX_ROLES
    };
};

/**
 * Role containing the very basic traits of an object - for world managing etc.
 */
struct rBase : public rRole {
    /// Unique Object ID, simulation date:time:deltacycle of spawn.
    OID oid;
    /// Age in seconds since spawn, update in animate().
    double seconds;
    /// Constructor.

    rBase() : rRole("BASE"), oid(0), seconds(0) {
    }
    /// Copy Constructor.

    rBase(rBase * original) : rRole("BASE"), oid(0), seconds(0) {
        if (original != NULL) {
            oid = original->oid;
            seconds = original->seconds;
        }
    }
};

/**
 *  Encapsulates Textual descriptive texts about an object.
 */
struct rNameable : public rRole {
    /// Name of object.
    std::string name;
    /// String describing the object.
    std::string description;
    /// { 0, .., 25 } for { a, ..., z }.
    unsigned int designation;
    /// Constructor.

    rNameable() : rRole("NAMEABLE"), name("Unnamed"), description("Undescribed"), designation(0) {
    }
    /// Copy Constructor.

    rNameable(rNameable * original) : rRole("NAMEABLE"), name("Unnamed"), description("Undescribed"), designation(0) {
        if (original != NULL) {
            name = original->name;
            description = original->description;
            designation = original->designation;
        }
    }
};

/**
 * Encapsulates physical state as far as movement and position is concerned.
 */
struct rTraceable : public cParticle, public rRole {
    /// Constructor.

    rTraceable() : cParticle(), rRole("TRACEABLE") {
    }
    /// Copy Constructor.

    rTraceable(cParticle * original) : cParticle(original), rRole("TRACEABLE") {
    }
};

/**
 * Marks an object being collideable.
 */
struct rCollideable : public rRole {
    /// Constructor.

    rCollideable() : rRole("COLLIDEABLE") {
    };

    /// Copy Constructor.

    rCollideable(rCollideable * original) : rRole("COLLIDEABLE") {
    };
};

/**
 * Encapsulates attributes related to body damage and armor state.
 */
struct rDamageable : public rRole {
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
    /// Constructor.

    rDamageable() : rRole("DAMAGEABLE") {
        loopi(MAX_PARTS) hp[i] = 100.0f;
    }
    /// Copy Constructor.

    rDamageable(rDamageable * original) : rRole("DAMAGEABLE") {
        if (original == NULL) {
            rDamageable();
        } else {
            loopi(MAX_PARTS) hp[i] = original->hp[i];
        }
    }
};

/**
 * Encapsulates thoughtful steering behavior attributes and code.
 */
struct rControlled : public rRole {
    /// Aim target
    OID target;
    /// Who disturbed me? Replace with a map of (OID, intensity)-pairs?
    OID disturber;
    /// Movement target, set Not-a-Number for non-validity.
    std::vector<float> destination;
    /// Human, AI/Remote Pad-Control over the Object.
    cPad* pad;
    /// Command Stack controlls the pad when enabled.
    cController* controller;
    /// Constructor.

    rControlled() : rRole("CONTROLLED"), target(0), disturber(0), destination(3), pad(NULL), controller(NULL) {
        destination[0] = destination[1] = destination[2] = float_NAN;
    }
    /// Copy Constructor.

    rControlled(rControlled * original) : rRole("CONTROLLED") {
        if (original == NULL) {
            rControlled();
        } else {
            target = original->target;
            disturber = original->disturber;
            destination = original->destination;
            pad = original->pad;
            controller = original->controller;
        }
    }
};

/**
 *  Encapsulates social behavior related attributes (merge with entity?).
 */
struct rSocialised : public rRole {
    /// future: Bitmask, lists Roles which Allies have.
    std::set<OID> allies;
    /// Bitmask, lists Roles which Enemies play.
    std::set<OID> enemies;
    /// Constructor.

    rSocialised() : rRole("SOCIALISED") {
    }
    /// Copy Constructor.

    rSocialised(rSocialised * original) : rRole("SOCIALISED") {
        if (original != NULL) {
            allies = original->allies;
            enemies = original->enemies;
        }
    }

    /// Check wether an object playing a certain role is an ally.

    bool hasAllied(Roles role) {
        return ( allies.find(role) != allies.end());
    }

    /// Add roles which allies of this object play.

    void addAllied(Roles role) {
        allies.insert(role);
    }

    /// Remove objects playing a certain role from ally list.

    void remAllied(Roles role) {
        allies.erase(role);
    }

    /// Check wether an object playing a certain role is an enemy.

    bool hasEnemy(Roles role) {
        return ( enemies.find(role) != enemies.end());
    }

    /// Add roles which enemies of this object play.

    void addEnemy(Roles role) {
        enemies.insert(role);
    }

    /// Remove objects playing a certain role from enemy list.

    void remEnemy(Roles role) {
        enemies.erase(role);
    }
};

/**
 * Object Group - as for now intended for messaging maillist-groups.
 */
struct rGrouping : public rRole {
    /// Group name.
    std::string mName;
    /// Lists registered members of this group.
    std::set<OID> mMembers;

    rGrouping() : rRole("GROUPING"), mName("Unnamed") {
    }

    rGrouping(rGrouping * original) : rRole("GROUPING"), mName("Unnamed") {
        if (original != NULL) {
            mName = original->mName;
            mMembers.clear();
            //mMembers.insert(mMembers.begin(), original->mMembers.begin(), original->mMembers.end());
        }
    }
};


/**
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
class cObject {
    friend class cWorld;
public:

    /// Switches 3D-Texturing on and off.
    static int ENABLE_TEXTURE_3D;

public: // Predefined Roles

    rBase* base;
    rNameable* nameable;
    rSocialised* socialised;
    rTraceable* traceable;
    rCollideable* collideable;
    rDamageable* damageable;
    rControlled* controlled;
    rGrouping* grouping;

public: // Experimental Role "Managing"

    static std::map<std::string, OID> roleids;
    static std::map<OID, std::string> rolenames;
    std::map<OID, rRole*> roles;
    std::set<OID> roleset;

public:

    cObject() {
        if (roleids.empty()) {
            registerRole(rRole::BASE, "BASE");
            registerRole(rRole::TRACEABLE, "TRACEABLE");
            registerRole(rRole::COLLIDEABLE, "COLLIDEABLE");
            registerRole(rRole::DAMAGEABLE, "DAMAGEABLE");
            registerRole(rRole::CONTROLLED, "CONTROLLED");
            registerRole(rRole::SOCIALISED, "SOCIALISED");
            registerRole(rRole::GROUPING, "GROUPING");
        }
        base = new rBase;
        nameable = new rNameable;
        traceable = new rTraceable;
        collideable = NULL;
        damageable = NULL;
        controlled = NULL;
        socialised = NULL;
        grouping = NULL;
        roles[rRole::BASE] = base;
        roles[rRole::NAMEABLE] = nameable;
        roles[rRole::TRACEABLE] = traceable;
    }

    cObject(cObject* original) {
        if (original->base) base = new rBase(original->base);
        if (original->nameable) nameable = new rNameable(original->nameable);
        if (original->traceable) traceable = new rTraceable(original->traceable);
        if (original->collideable) collideable = new rCollideable(original->collideable);
        if (original->damageable) damageable = new rDamageable(original->damageable);
        if (original->controlled) controlled = new rControlled(original->controlled);
        if (original->socialised) socialised = new rSocialised(original->socialised);
        if (original->grouping) grouping = new rGrouping(original->grouping);
        if (original->base) roles[rRole::BASE] = base;
        if (original->nameable) roles[rRole::NAMEABLE] = nameable;
        if (original->traceable) roles[rRole::TRACEABLE] = traceable;
        if (original->collideable) roles[rRole::COLLIDEABLE] = collideable;
        if (original->damageable) roles[rRole::DAMAGEABLE] = damageable;
        if (original->controlled) roles[rRole::CONTROLLED] = controlled;
        if (original->socialised) roles[rRole::SOCIALISED] = socialised;
        if (original->grouping) roles[rRole::GROUPING] = grouping;
    }

    virtual ~cObject() {
        delete this->base;
        delete this->damageable;
        delete this->collideable;
        delete this->controlled;
        delete this->nameable;
        delete this->socialised;
        delete this->traceable;
    }

    static void registerRole(OID id, std::string name) {
        rolenames[id] = name;
        roleids[name] = id;
    }

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

    /// Check wether this Object is playing the given role.

    bool hasRole(OID role) {
        return (roleset.find(role) != roleset.end());
    }

    /// Add a role this objects has to play.

    void addRole(OID role, rRole* roleobj = NULL) {
        roles[role] = roleobj;
        roleset.insert(role);
    }

    /// Remove a role this object wont play anymore.

    void remRole(OID role, bool deleteobj = false) {
        if (deleteobj) delete roles[role];
        roles.erase(role);
        roleset.erase(role);
    }


    /// Called right after object was spawned into the world.

    virtual void onSpawn() {
    }

    /// Called when a message was sent to (a group) this object (is a member of).

    virtual void onMessage(cMessage* message) {
    }

    /// Called right after object has been removed from object list and index.

    virtual void onFrag() {
    }

    /// glMultiplies in the Object's camera matrix.

    virtual void multEyeMatrix() {
    }

    /// Sets this Object's location, orientation and vel. as Audio listener.

    virtual void setAsAudioListener() {
    }

    /// Advance internal timers,animation state and pose, check gamepad.

    virtual void animate(float dt) {
        base->seconds += dt;
    }

    /// Precalculate neccessary transformations - matrices, mountpoints, pos ..

    virtual void transform() {
    }

    /// Render solid non-translucent parts of the object.

    virtual void drawSolid() {
    }

    /// Render translucent object-parts and visual effects.

    virtual void drawEffect() {
    }

    /// Render HUD-contents as seen when looking through the objects-eyes.

    virtual void drawHUD() {
    }


    // Receiving damage:

    /**
     * Deal damage to the object with the damage arriving at or from
     * the given local position ie. relative to the object itself.
     * @localpos input location vector relative to the object.
     * @damage amount of damage to deal to the object.
     * @enactor the object dealing the damage to this object.
     */
    virtual void damageByParticle(float* localpos, float damage, cObject* enactor = NULL) {

    }


    // Physics for Traceable:

    /**
     * Constrain the given world position particle (with radius) to
     * the object boundary hull ie. place the worldpos to the nearest
     * boundary hull position if the worldpos is inside the object's hull.
     * localpos delivers in case of impact the local position relative
     * to the object of the corrected worldpos.
     * @worldpos input location vector in world coordinates.
     * @radius radius of the checked particle in world dimensions.
     * @localpos output adjusted location vector.
     * @return the intrusion depth.
     */
    virtual float constrainParticle(float* worldpos, float radius = 0.0f, float* localpos = NULL, cObject* enactor = NULL) {
        return 0;
    }


    // Related to States:

    // Queries

    /// Search through world-database to find nearest enemy, returns OID.

    virtual OID enemyNearby() {
        return 0;
    }

    /// If object was disturbed by an enemy this may return it's OID.

    virtual OID disturbedBy() {
        return 0;
    }

    // Fuzzy Predicates [0,1]

    /// Has this object reached it's destination position? Fuzzyvalue in [0,1].

    virtual float inDestinationRange() {
        return 0.0f;
    }

    /// Is this object in "hand-to-hand"-combat range? Fuzzyvalue in [0,1].

    virtual float inMeeleRange() {
        return 0.0f;
    }

    /// Is this object in firing range. Fuzzyvalue in [0,1].

    virtual float inWeaponRange() {
        return 0.0f;
    }

    /// Is this object in targeting/visual distance? Fuzzyvalue in [0,1].

    virtual float inTargetRange() {
        return 0.0f;
    }

    /*
    virtual float blockedAhead() {
        return 0.0f;
    }
    
    virtual float blockedLeft() {
        return 0.0f;
    }

    virtual float blockedRight() {
        return 0.0f;
    }

     */

    // Output Actions for Entities

    /// Move towards move-target as fast as possible.

    virtual void do_moveTowards() {
    }

    /// Move to move-target throttling down as target comes near.

    virtual void do_moveNear() {
    }

    /// Aim at aim-target.

    virtual void do_aimAt() {
    }

    /// Fire at aim-target - ie. just fire where object is aiming at right now.

    virtual void do_fireAt() {
    }

    /// Nothing to do for object - possibly idle animation.

    virtual void do_idle() {
    }

    /// Set aim towards given object - ie. set aim-target.

    virtual void do_aimFor(OID target) {
    }

    /// Set movement towards given location. if aim target NULL then same.

    virtual void do_moveFor(float* dest) {
    }
};

#endif
