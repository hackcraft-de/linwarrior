/* 
 * File:     rTarcom.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on September 15, 2011, 7:35 PM
 */

#ifndef RTARCOM_H
#define	RTARCOM_H

struct rTarcom;

#include "rComputer.h"

#include "de/hackcraft/psi3d/math3d.h"

#include "de/hackcraft/world/OID.h"

#include <list>
#include <set>

/** Targeting Computer,
 * includes Radar, target-selection and -display.
 */
struct rTarcom : public rComputer {
public: // INPUT
    /// Sensor orientation. (hook i)
    quat ori0;
    /// Sensor position. (hook i)
    vec3 pos0;
    /// Switch to next target. (hook i)
    bool switchnext;
    /// Switch to previous target. (hook i)
    bool switchprev;
    /// Set lists Tags of allies.
    std::set<OID> inc_allies;
    /// Set lists Tags of explicitly excluded allied (like dead ones).
    std::set<OID> exc_allies;
    /// Set lists Tags of enemies.
    std::set<OID> inc_enemies;
    /// Set lists Tags of explicitly excluded enemies (like dead ones).
    std::set<OID> exc_enemies;
public: // OUTPUT
    /// List of objects in far range.
    std::list<cObject*>* far;
    /// List of objects in range.
    std::list<cObject*>* near;
    /// List of (possible) enemy objects in range (only inc_enemies).
    std::list<cObject*>* enemies;
    /// ID of selected object (hook o)
    OID selected;
    /// Switching to next/prev target in progress.
    bool switching;
    /// Just any near enemy. (hook o)
    OID nearbyEnemy;
protected: // INTERNALS
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

#endif	/* RTARCOM_H */

