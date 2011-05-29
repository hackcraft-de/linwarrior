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

#include "de/hackcraft/comp/rGrouping.h"

#include "de/hackcraft/util/OID.h"
#include "de/hackcraft/util/Pad.h"
#include "de/hackcraft/util/Message.h"

#include "de/hackcraft/psi3d/math3d.h"

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
    vec3 pos0;
    /// Radius for clustering and visibility.
    float radius;
    /// Basic object orientation.
    quat ori0;
    /// Age in seconds since spawn, updated by world before animate() call.
    double seconds;
    /// Internal qualified name of the object.
    std::string name;
    /// Input to the object through common input device if not null.
    Pad* pad;

public: // Components

    std::vector<rComponent*> components;

public: // FIXME: Predefined Components, to be removed from cObject

    rGrouping* grouping;

public: // Experimental Component "Managing"

    //static std::map<std::string, rComponent*> roleprotos;
    //static std::map<std::string, OID> roleoffsets;
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
        pos0[0] = pos0[1] = pos0[2] = 0.0f;
        ori0[0] = ori0[1] = ori0[2] = 0.0f;
        ori0[3] = 1.0f;
        seconds = 0;
        name = "";
        //if (roleprotos.empty()) {
            //registerRole(new rNameable, FIELDOFS(nameable), ROLEPTR(cObject::nameable));
            //registerRole(new rTraceable, FIELDOFS(traceable), ROLEPTR(cObject::traceable));
            //registerRole(new rDamageable, FIELDOFS(damageable), ROLEPTR(cObject::damageable));
            //registerRole(new rControlled, FIELDOFS(controlled), ROLEPTR(cObject::controlled));
            //registerRole(new rGrouping, FIELDOFS(grouping), ROLEPTR(cObject::grouping));
        //}
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

    /*
    static void registerRole(rComponent* prototype, OID attribute_offset, rComponent* cObject::* ptr = NULL) {
        roleprotos[prototype->role] = prototype;
        roleoffsets[prototype->role] = attribute_offset;
        std::cout << "NEW ROLE " << prototype->role << " @ " << attribute_offset << " # " << ptr << "\n";
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

    virtual void spawn() {
        for (auto i = components.begin(); i != components.end(); i++) {
            (*i)->spawn();
        }
    }

    /// Called right after object has been removed from object list and index.

    virtual void frag() {
        for (auto i = components.begin(); i != components.end(); i++) {
            (*i)->frag();
        }
    }

    /// Called to glMultiply in the Object's camera matrix.

    virtual void camera() {
        for (auto i = components.begin(); i != components.end(); i++) {
            (*i)->camera();
        }
    }

    /// Called to set Object's location, orientation and vel. as audio listener.

    virtual void listener() {
        for (auto i = components.begin(); i != components.end(); i++) {
            (*i)->listener();
        }
    }

    /// Called for each message sent to this object.

    virtual void message(Message* message) {
        for (auto i = components.begin(); i != components.end(); i++) {
            (*i)->message(message);
        }
    }

    /// Called to advance internal timers,animation state and pose, check gamepad.

    virtual void animate(float dt) {
        for (auto i = components.begin(); i != components.end(); i++) {
            (*i)->animate(dt);
        }
    }

    /// Deprecated, use animate? Called to precalculate neccessary transformations - matrices, mountpoints, pos ..

    virtual void transform() {
        for (auto i = components.begin(); i != components.end(); i++) {
            (*i)->transform();
        }
    }

    /// Called to render solid non-translucent parts of the object.

    virtual void drawSolid() {
        for (auto i = components.begin(); i != components.end(); i++) {
            (*i)->drawSolid();
        }
    }

    /// Called to render translucent object-parts and visual effects.

    virtual void drawEffect() {
        for (auto i = components.begin(); i != components.end(); i++) {
            (*i)->drawEffect();
        }
    }

    /// Called to render HUD-contents as seen when looking through the objects-eyes.

    virtual void drawHUD() {
    }

    /**
     * Called to deal damage to the object with the damage arriving at or
     * from the given local position ie. relative to the object itself.
     * @localpos input location vector relative to the object.
     * @damage amount of damage to deal to the object.
     * @enactor the object dealing the damage to this object.
     */
    virtual void damage(float* localpos, float damage, cObject* enactor = NULL) {
        for (auto i = components.begin(); i != components.end(); i++) {
            (*i)->damage(localpos, damage, enactor);
        }
    }

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
    virtual float constrain(float* worldpos, float radius = 0.0f, float* localpos = NULL, cObject* enactor = NULL) {
        double maxdepth = 0.0f;
        for (auto i = components.begin(); i != components.end(); i++) {
            double depth = (*i)->constrain(worldpos, radius, localpos, enactor);
            maxdepth = fmax(maxdepth, depth);
        }
        return float(maxdepth);
    }

};

#endif
