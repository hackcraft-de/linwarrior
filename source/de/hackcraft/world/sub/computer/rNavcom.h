/* 
 * File:     rNavcom.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on September 15, 2011, 7:51 PM
 */

#ifndef RNAVCOM_H
#define	RNAVCOM_H

struct rNavcom;

#include "de/hackcraft/world/Component.h"
#include "de/hackcraft/world/IComputer.h"

#include "de/hackcraft/psi3d/math3d.h"

/** Navigation Computer,
 * stores mission critical POints of Interest (POIs),
 * routes, map data and encapsulates path-finding.
 */
struct rNavcom : public Component, public IComputer {
public: // SYSTEM
    /** Identifier for this component (all uppercase letters without leading "r"). */
    static std::string cname;
    /** A unique random number (0-9999) to identify this component. */
    static unsigned int cid;
public:
    quat ori0;
    vec3 pos0;
    int mWaypoint;
    bool mCyclic;
    std::vector< int > mRoute;
    std::vector< std::vector<float> > mPOIs;
public:
    rNavcom(Entity* obj = NULL);
    void drawPOI(float x, float y, float s);
    virtual void animate(float spf);
    virtual void drawHUD();
};

#endif	/* RNAVCOM_H */

