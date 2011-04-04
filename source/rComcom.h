/* 
 * File:     rComcom.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on August 4, 2009, 8:44 PM
 */

#ifndef _CCOMCOM_H
#define	_CCOMCOM_H

struct rComputer;
struct rComcom;
struct rTarcom;
struct rWepcom;
struct rForcom;
struct rNavcom;

#include "cObject.h"

#include <list>

/** Computer Component
 *  Placeholder base class for computerised components.
 *  Computers have to implement animate and drawHUD methods to be useful.
 *  The message method isn't mandatory.
 */
struct rComputer : rComponent {
public:

    virtual void message(cMessage * message) {
    };
    virtual void animate(float spf) = 0;
    virtual void drawHUD() = 0;
};

/** Communication Computer
 *  encapsulates sending and receiving of messages
 *  and stores these messages.
 *  Orders to comrades may enhance strategic advance.
 */
struct rComcom : public rComputer {
protected:
    void* mConsole;
    OID mLastMessage;
public:
    rComcom(cObject* obj = NULL);
    virtual void animate(float spf);
    virtual void drawHUD();
};

/** Targeting Computer,
 * includes Radar, target-selection and -display.
 */
struct rTarcom : public rComputer {
public:
    /// Sensor orientation. (hook i)
    quat ori;
    /// Sensor position. (hook i)
    vec3 pos;
    /// List of objects in far range.
    std::list<cObject*>* far;
    /// List of objects in range.
    std::list<cObject*>* near;
    /// List of (possible) enemy objects in range (only inc_enemies).
    std::list<cObject*>* enemies;
    /// ID of selected object (hook o)
    OID selected;
    /// Switch to next target. (hook i)
    bool switchnext;
    /// Switch to previous target. (hook i)
    bool switchprev;
    /// Switching to next/prev target in progress.
    bool switching;
    /// Set lists Tags of allies.
    std::set<OID> inc_allies;
    /// Set lists Tags of explicitly excluded allied (like dead ones).
    std::set<OID> exc_allies;
    /// Set lists Tags of enemies.
    std::set<OID> inc_enemies;
    /// Set lists Tags of explicitly excluded enemies (like dead ones).
    std::set<OID> exc_enemies;
    /// Just any near enemy. (hook o)
    OID nearbyEnemy;
public:
    /// Constructor.
    rTarcom(cObject* obj = NULL);
protected:
    /// Switch to next target.
    void nextTarget();
    /// Switch to previous target.
    void prevTarget();
public:
    virtual void animate(float spf);
    virtual void drawHUD();
public:
    /// Check wether an tag-set is identified as an ally.
    bool isAllied(std::set<OID>* tags);
    /// Add tags which allies of this object have (not).
    void addAllied(OID tag, bool include = false);
    /// Remove tags which alies of this object have (not).
    void remAllied(OID tag, bool include = false);
    /// Check wether an tag-set is identified as an enemy.
    bool isEnemy(std::set<OID>* tags);
    /// Add roles which enemies of this object play.
    void addEnemy(OID tag, bool include = true);
    /// Remove objects playing a certain role from enemy list.
    void remEnemy(OID tag, bool include = false);
};

/** Weapon Computer
 * currently just draws weapon status.
 */
struct rWepcom : public rComputer {
public:
    rWepcom(cObject* obj = NULL); // FIXME: Must be a cMech.
    virtual void animate(float spf);
    virtual void drawHUD();
};

/** Forward Computer
 * Forward-looking information overlay.
 */
struct rForcom : public rComputer {
public:
    std::string mMessage;
    quat ori;
    vec3 twr;
    bool reticle;
public:
    rForcom(cObject* obj = NULL); // FIXME: Must be a cMech.
    virtual void message(cMessage * message);
    virtual void animate(float spf);
    virtual void drawHUD();
};

/** Navigation Computer,
 * stores mission critical POints of Interest (POIs),
 * routes, map data and encapsulates path-finding.
 */
struct rNavcom : public rComputer {
public:
    quat ori;
    vec3 pos;
    int mWaypoint;
    bool mCyclic;
    std::vector< int > mRoute;
    std::vector< std::vector<float> > mPOIs;
public:
    rNavcom(cObject* obj = NULL);
    void drawPOI(float x, float y, float s);
    virtual void animate(float spf);
    virtual void drawHUD();
};


#endif	/* _CCOMCOM_H */

