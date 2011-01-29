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
 * Base-Class of all roles.
 * An instance of it may be used to flag a role as being "set" (future).
 * A role can be understood as a set of object attributes reflecting
 * a certain part or aspect of the object.
 * Did you know that a Role of an Object can be sub-classed individually
 * to replace the parent role in an Object?
 */
struct rRole {
    std::string role;

    rRole(std::string role) {
        this->role = role;
    }

    virtual rRole* clone() {
        return new rRole(role);
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

    virtual rRole* clone() {
        return new rNameable(this);
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

    virtual rRole* clone() {
        return new rTraceable(this);
    }
};


/**
 * Encapsulates attributes related to body damage and armor state.
 */
struct rDamageable : public rRole {
    /// Marks object as still alife for internal state reference.
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
    /// Constructor.

    rDamageable() : rRole("DAMAGEABLE") {
        alife = true;
        loopi(MAX_PARTS) hp[i] = 100.0f;
    }
    /// Copy Constructor.

    rDamageable(rDamageable * original) : rRole("DAMAGEABLE") {
        if (original == NULL) {
            rDamageable();
        } else {
            alife = original->alife;
            loopi(MAX_PARTS) hp[i] = original->hp[i];
        }
    }

    virtual rRole* clone() {
        return new rDamageable(this);
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
    vec3 destination;
    /// Human, AI/Remote Pad-Control over the Object.
    cPad* pad;
    /// Command Stack controlls the pad when enabled.
    cController* controller;
    /// Constructor.

    rControlled() : rRole("CONTROLLED"), target(0), disturber(0), pad(NULL), controller(NULL) {
        vector_set(destination, float_NAN, float_NAN, float_NAN);
    }
    /// Copy Constructor.

    rControlled(rControlled * original) : rRole("CONTROLLED") {
        if (original == NULL) {
            rControlled();
        } else {
            target = original->target;
            disturber = original->disturber;
            vector_cpy(destination, original->destination);
            pad = original->pad;
            controller = original->controller;
        }
    }

    virtual rRole* clone() {
        return new rControlled(this);
    }
};

/**
 *  Encapsulates social behavior related attributes (merge with entity?).
 */
struct rSocialised : public rRole {
    /// Set lists Tags of allies.
    std::set<OID> inc_allies;
    /// Set lists Tags of explicitly excluded allied (like dead ones).
    std::set<OID> exc_allies;
    /// Set lists Tags of enemies.
    std::set<OID> inc_enemies;
    /// Set lists Tags of explicitly excluded enemies (like dead ones).
    std::set<OID> exc_enemies;
    
    /// Constructor.
    rSocialised() : rRole("SOCIALISED") {
    }

    /// Copy Constructor.
    rSocialised(rSocialised * original) : rRole("SOCIALISED") {
        if (original != NULL) {
            inc_allies = original->inc_allies;
            inc_enemies = original->inc_enemies;
            exc_allies = original->exc_allies;
            exc_enemies = original->exc_enemies;
        }
    }

    virtual rRole* clone() {
        return new rSocialised(this);
    }

    /// Check wether an tag-set is identified as an ally.
    bool isAllied(std::set<OID>* tags) {
        std::set<OID> inclusion;
        std::set_intersection(tags->begin(), tags->end(), inc_allies.begin(), inc_allies.end(), std::inserter(inclusion, inclusion.begin()));
        std::set<OID> exclusion;
        std::set_intersection(tags->begin(), tags->end(), exc_allies.begin(), exc_allies.end(), std::inserter(exclusion, exclusion.begin()));
        return (!inclusion.empty() && exclusion.empty());
    }

    /// Add tags which allies of this object have (not).
    void addAllied(OID tag, bool include = false) {
        if (include) {
            inc_allies.insert(tag);
        } else {
            exc_allies.insert(tag);
        }
    }

    /// Remove tags which alies of this object have (not).
    void remAllied(OID tag, bool include = false) {
        if (include) {
            inc_allies.erase(tag);
        } else {
            exc_allies.erase(tag);
        }
    }

    /// Check wether an tag-set is identified as an enemy.
    bool isEnemy(std::set<OID>* tags) {
        std::set<OID> inclusion;
        std::set_intersection(tags->begin(), tags->end(), inc_enemies.begin(), inc_enemies.end(), std::inserter(inclusion, inclusion.begin()));
        std::set<OID> exclusion;
        std::set_intersection(tags->begin(), tags->end(), exc_enemies.begin(), exc_enemies.end(), std::inserter(exclusion, exclusion.begin()));
        return (!inclusion.empty() && exclusion.empty());
    }

    /// Add roles which enemies of this object play.
    void addEnemy(OID tag, bool include = true) {
        if (include) {
            inc_enemies.insert(tag);
        } else {
            exc_enemies.insert(tag);
        }
    }

    /// Remove objects playing a certain role from enemy list.
    void remEnemy(OID tag, bool include = false) {
        if (include) {
            inc_enemies.erase(tag);
        } else {
            exc_enemies.erase(tag);
        }
    }
};

/**
 * Object Group - as for now intended for messaging maillist-groups.
 */
struct rGrouping : public rRole {
    /// Group name.
    std::string name;
    /// Lists registered members of this group.
    std::set<OID> members;

    rGrouping() : rRole("GROUPING"), name("Unnamed") {
    }

    rGrouping(rGrouping * original) : rRole("GROUPING"), name("Unnamed") {
        if (original != NULL) {
            name = original->name;
            members.clear();
            //mMembers.insert(mMembers.begin(), original->mMembers.begin(), original->mMembers.end());
        }
    }

    virtual rRole* clone() {
        return new rGrouping(this);
    }
};

#define FIELDOFS(attribute)     (((OID)&attribute) - ((OID)this))
#define ROLEPTR(attribute)      ((rRole* cObject::*) &attribute)
#define GETROLE(offset)         ((rRole*) *((OID*)( ((OID)this) + offset )))

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

public: // Basic Object attributes for managing.

    /// Unique Object ID, simulation date:time:deltacycle of spawn.
    OID oid;
    /// Age in seconds since spawn, updated by world before animate() call.
    double seconds;
    /// Internal qualified name of the object.
    std::string name;

public: // Predefined Roles

    rNameable* nameable;
    rSocialised* socialised;
    rTraceable* traceable;
    rDamageable* damageable;
    rControlled* controlled;
    rGrouping* grouping;

public: // Experimental Role "Managing"

    static std::map<std::string, rRole*> roleprotos;
    static std::map<std::string, OID> roleoffsets;
    //static std::map<std::string, OID> roleids;
    //static std::map<OID, rRole*> roletypes;
    //std::map<OID, rRole*> roles;
    //std::set<OID> roleset;

public: // Object Tags

    /// Tags (IDs, Social-Roles, Parties, States...) this object has.
    std::set<OID> tags;
    
    enum DefaultTags {
        // Roles indicating belonging to certain Social-Parties
        RED,
        BLUE,
        GREEN,
        YELLOW,

        // Roles indicating State
        WOUNDED, //  <= 75% hp
        SERIOUS, // <= 50% hp
        CRITICAL, // <= 25% hp
        DEAD, // <= 0% hp, note that the later include the former.

        // Special Roles
        HUMANPLAYER,

        MAX_PARTIES
    };

public:

    cObject() {
        oid = 0;
        seconds = 0;
        name = "";
        if (roleprotos.empty()) {
            registerRole(new rNameable, FIELDOFS(nameable), ROLEPTR(cObject::nameable));
            registerRole(new rSocialised, FIELDOFS(socialised), ROLEPTR(cObject::socialised));
            registerRole(new rTraceable, FIELDOFS(traceable), ROLEPTR(cObject::traceable));
            registerRole(new rDamageable, FIELDOFS(damageable), ROLEPTR(cObject::damageable));
            registerRole(new rControlled, FIELDOFS(controlled), ROLEPTR(cObject::controlled));
            registerRole(new rGrouping, FIELDOFS(grouping), ROLEPTR(cObject::grouping));
        }
        nameable = new rNameable;
        socialised = NULL;
        traceable = new rTraceable;
        damageable = NULL;
        controlled = NULL;
        grouping = NULL;
        //rRole* r = GETROLE(FIELDOFS(nameable));
        //std::cout << "TEST: " << r << " vs " << nameable << "\n";
    }

    cObject(cObject* original) {
        if (original->nameable) nameable = new rNameable(original->nameable);
        if (original->traceable) traceable = new rTraceable(original->traceable);
        if (original->damageable) damageable = new rDamageable(original->damageable);
        if (original->controlled) controlled = new rControlled(original->controlled);
        if (original->socialised) socialised = new rSocialised(original->socialised);
        if (original->grouping) grouping = new rGrouping(original->grouping);
    }

    virtual ~cObject() {
        delete this->nameable;
        delete this->traceable;
        delete this->damageable;
        delete this->controlled;
        delete this->socialised;
        delete this->grouping;
    }

    static void registerRole(rRole* prototype, OID attribute_offset, rRole* cObject::* ptr = NULL) {
        roleprotos[prototype->role] = prototype;
        roleoffsets[prototype->role] = attribute_offset;
        std::cout << "NEW ROLE " << prototype->role << " @ " << attribute_offset << " # " << ptr << "\n";
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
    */

    /// Check wether this Object has at least one of the given tags.
    bool anyTags(std::set<OID>* tagset) {
        std::set<OID> result;
        std::set_intersection(tags.begin(), tags.end(), tagset->begin(), tagset->end(), std::inserter(result, result.begin()));
        return (!result.empty());
    }

    /// Check wether this Object has all the given tags.
    bool allTags(std::set<OID>* tagset) {
        std::set<OID> result;
        std::set_intersection(tags.begin(), tags.end(), tagset->begin(), tagset->end(), std::inserter(result, result.begin()));
        return (result.size() == tagset->size());
    }

    /// Check wether this Object has the given tag.
    bool hasTag(OID tag) {
        return (tags.find(tag) != tags.end());
    }

    /// Add a tag to this object.
    void addTag(OID tag) {
        tags.insert(tag);
    }

    /// Remove a tag from this object.
    void remTag(OID tag) {
        tags.erase(tag);
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
