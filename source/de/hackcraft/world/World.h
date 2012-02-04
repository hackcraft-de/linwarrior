/**
 * File:     World.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on March 28, 2008, 10:23 PM (1999)
 */

#ifndef _CWORLD_H
#define _CWORLD_H


class World;

#include "de/hackcraft/psi3d/macros.h"

#include "de/hackcraft/world/Background.h"
#include "de/hackcraft/world/OID.h"
#include "de/hackcraft/world/Timing.h"
#include "de/hackcraft/world/Message.h"

#include "de/hackcraft/world/Entity.h"

#include <list>
#include <set>
#include <map>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>

class Background;
class Entity;
class Mission;

/**
 * The Game-Object-Managing World-Database-Class.
 * Cares about spawning, indexing, searching, messaging, updating, rendering.
 */
class World {
public:
    /// The current World instance (singleton) other objects may
    /// only affect that current instance.
    static World* instance;

    /// Mission Subsystem (world setup and objective-controller).
    Mission* mission;

    /// Background Subsystem (sky, env-lighting and fog).
    Background* background;
    
private:
    /// Not yet dispatched messages (because they are not yet overdue).
    std::priority_queue<Message*, std::vector<Message*>, Message> mMessages;

    /// Messages already delivered to group participants.
    std::list<Message*> mDispatchedMessages;

    // Not Yet, may be merged with Background:
    // Overlays weather effects like rain, snow or dust.
    //cWeather mWeather;

    /// Gravity acceleration vector [m/s²]. space = (0,0,0), earth = (0,-9.8,0).
    vec3 mGravity;

    /// Air density in kg per cubic meter (default 1.204 kg/m3 Air at 20°C).
    float mAirdensity;

    /// Velocity "damping" when on ground (0.14 ok).
    float mGndfriction;

    /// Object-ID (OID) to Object-Reference (non-fragged).
    std::map<OID, Entity*> mIndex;

    /// Contains non-fragged objects.
    std::list<Entity*> mObjects;

    /// Contains fragged objects.
    std::list<Entity*> mCorpses;

    /// Allows searching the world in a structured manner.
    std::unordered_map<OID, std::list<Entity*> > mGeomap;

    /// Non-Positional (NaN) and some oversize objects go here for clustering.
    std::list<Entity*> mUncluster;

    /// Render only objects that far away.
    float mViewdistance;

    /*
     * Key-Value-Pairs for world state and progress indication.
     * Interpretation is not fixed and may vary, samples:
     * "ImportantThingHappened" = "true" [boolean value]
     * "CurrentChapter" = "1" [integer value]
     * "ObjectOfInterest" = "20280809120000001" [OID]
     * "JustAFlagOrKeyitem" = "" [flag]
     */
    //std::map<std::string, std::string> mValues;

    /// Timing, simulation stepping, deltacycles, date, fps, etc.
    Timing mTiming;
    
    /// Groups objects together for searching, messaging, tagging, ...
    struct Group {
        /// Group id used for reference (later: use for tags)
        OID gid;
        /// Group id of parent group if not zero.
        //OID parent;
        /// Name of the group (use qualified names like: "world/enemies/squads/alpha" or "state/dead")
        std::string name;
        /// Objects in this group.
        std::list<OID> members;
    };
    
    /// Groups of objects
    std::map<OID,Group*> mGroupIndex;

public: // Constructor, Object-Management and Drawing:

    World();

public: // Accessors

    /// Returns the Object ID valid for this instant's time & deltacycle.
    OID getOID();

    Timing* getTiming();

    Entity* getObject(OID oid);

    float* getGravity();

    float getGndfriction();

    float getAirdensity();

    void setViewdistance(float viewdistance);

    float getViewdistance();
    
public: // Grouping

    /// Finds and returns a group or creates a new one.
    OID getGroup(std::string name);

    /// Adds a new member to a existing group or creates a new one, returns gid.
    void addToGroup(OID gid, Entity* member);

public: // Messaging

    /**
     * Sent messages are currently collected with an attached timestamp.
     * @param delay time offset for sending see getOID() for calculation.
     * @param sender a value of 0 currently defines a broadcast.
     * @param recvid is the receiver the message is sent to - 0 null/def receiver.
     * @param text the message subject/text to send, will be copied to string.
     * @param blob binary data, you give away ownership may be deleted by world.
     */
    void sendMessage(OID delay, OID sender /* = 0 */, OID recvid /* = 0 */, std::string type, std::string text, void* blob = NULL);

public: // Spawning, Fragging, Garbage Collection

    /**
     * Pastes object into active world (mObjects & mIndex),
     * assigns serial OID,
     * increments delta cycle,
     * finally calls object->onSpawn().
     */
    void spawnObject(Entity *object);

    /**
     * Removes object from active world (mObjects & mIndex),
     * adds object to Corpse-List,
     * adds rRole::FRAGGED role to object,
     * finally object->onFragged() is called.
     * Note that FRAGGED != DEAD objects:
     * Dead objects are still part of the world
     * but fragged objects are removed from the perceptible world.
     * Fragged == removed and scheduled for deletion.
     */
    void fragObject(Entity *object);

    /**
     * Deletes fragged objects from Corpse-List.
     * Possibly called very rarely within a Mission at transitional moments.
     * This is like garbage collection on demand.
     */
    void bagFragged();

public: // Simulation Step - Call every frame in order to update the world/mission and draw a frame.

    /// Advance simulation time for one frame.
    void advanceTime(int deltamsec);
    /// Re-build spatial clustering of objects.
    void clusterObjects();
    /// Deliver overdue messages to objects.
    void dispatchMessages();
    /// Let all objects process input, adjust pose and calculate physics.
    void animateObjects();
    /// Let all objects calculate transformation matrices etc.
    void transformObjects();
    /// Draws background (skybox) by calling mBackground drawing method.
    void drawBack();
    /// Draw all Object's solid surfaces (calls their drawSolid method).
    void drawSolid(Entity* camera, std::list<Entity*>* objects = NULL);
    /// Draw all Object's effects (calls their drawEffect method).
    void drawEffect(Entity* camera, std::list<Entity*>* objects = NULL);

public: // World-Filtering, World-Scanning, World-Sense for objects.

    /**
     * Hash key for location.
     *
     * @param x east west coord
     * @param z north south coord
     * @return Hash key for location.
     */
    OID getGeokey(long x, long z);

    /**
     * Find objects within 2d area using location hashkeys.
     * 
     * @param min2f First corner of square area (smaller values).
     * @param max2f Second corner of square area (larger values).
     * @param addunclustered Return global unclustered objects, too.
     * @return List of found objects.
     */
    std::list<Entity*>* getGeoInterval(float* min2f, float* max2f, bool addunclustered = false);

    /**
     * Debug function to print out an object-list.
     * With a parameter of NULL/no parameter at all prints all objects within the world.
     * Warning: Printing all objects could lead to massive load on the console.
     */
    std::string getNames(std::list<Entity*>* objects = NULL);

    /**
     * Filters the given objectlist (or all mObjects if NULL) by tags (ORed Bitmask) (excluding ex Object).
     * There is the option to match all tags (true=AND) or to select any given tag (false=OR).
     * The returned list is fresh allocated - caller delete responsibility.
     */
    std::list<Entity*>* filterByTags(Entity* ex, std::set<OID>* rolemask, bool all, int maxamount, std::list<Entity*>* objects = NULL);

    /** Returns a List of objects which are within minimum and maximum range (excluding ex Object).
     * The returned list is fresh allocated - caller delete responsibility.
     */
    std::list<Entity*>* filterByRange(Entity* ex, float* origin, float minrange, float maxrange, int maxamount, std::list<Entity*>* objects = NULL);

    /**
     * Returns a List of objects which have the specified name.
     * The returned list is fresh allocated - caller delete responsibility.
     */
    std::list<Entity*>* filterByName(Entity* ex, char* name, int maxamount, std::list<Entity*>* objectlist = NULL);

    /**
     * Returns a List of objects whose bounding volume (approximately) intersects with the cylinder
     * described by the volume of a certain diameter around a line.
     * The returned list is fresh allocated - caller delete responsibility.
     */
    std::list<Entity*>* filterByBeam(Entity* ex, float* pointa, float* pointb, float radius, int maxamount, std::list<Entity*>* objects = NULL);

    /**
     *
     * @param worldpos The particle position to be constrained given in world coordinates.
     * @param radius The size of the particle in terms of a radius.
     * @return Zero if there was no collision, else the maximum impact depth.
     */
    float constrainParticle(Entity* ex, float* worldpos, float radius);
};

#endif
