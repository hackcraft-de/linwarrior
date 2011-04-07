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

class cWorld;

struct rGrouping;

#include "rGrouping.h"

#include "OID.h"
#include "cMessage.h"
#include "cPad.h"

#include "psi3d/math3d.h"

#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <string>
#include <iostream>

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
    /// Position for clustering, targeting and placing.
    vec3 pos;
    /// Radius for clustering and visibility.
    float radius;
    /// Basic object orientation.
    quat ori;
    /// Age in seconds since spawn, updated by world before animate() call.
    double seconds;
    /// Internal qualified name of the object.
    std::string name;
    /// Input to the object through common input device if not null.
    cPad* pad;

public: // FIXME: Predefined Components, to be removed from cObject

    rGrouping* grouping;

public: // Experimental Component "Managing"

    static std::map<std::string, rComponent*> roleprotos;
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
        pos[0] = pos[1] = pos[2] = 0.0f;
        ori[0] = ori[1] = ori[2] = 0.0f;
        ori[3] = 1.0f;
        seconds = 0;
        name = "";
        if (roleprotos.empty()) {
            //registerRole(new rNameable, FIELDOFS(nameable), ROLEPTR(cObject::nameable));
            //registerRole(new rTraceable, FIELDOFS(traceable), ROLEPTR(cObject::traceable));
            //registerRole(new rDamageable, FIELDOFS(damageable), ROLEPTR(cObject::damageable));
            //registerRole(new rControlled, FIELDOFS(controlled), ROLEPTR(cObject::controlled));
            //registerRole(new rGrouping, FIELDOFS(grouping), ROLEPTR(cObject::grouping));
        }
        grouping = NULL;
        pad = NULL;
        //rRole* r = GETROLE(FIELDOFS(nameable));
        //std::cout << "TEST: " << r << " vs " << nameable << "\n";
    }

    cObject(cObject* original) {
        if (original->grouping) grouping = new rGrouping(original->grouping);
    }

    virtual ~cObject() {
        delete this->grouping;
    }

    static void registerRole(rComponent* prototype, OID attribute_offset, rComponent* cObject::* ptr = NULL) {
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

    /// Called right after object has been removed from object list and index.

    virtual void onFrag() {
    }

    /// glMultiplies in the Object's camera matrix.

    virtual void multEyeMatrix() {
    }

    /// Sets this Object's location, orientation and vel. as Audio listener.

    virtual void setAsAudioListener() {
    }

    /// Called for each message sent to this object.

    virtual void message(cMessage* message) {
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
