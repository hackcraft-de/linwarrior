/* 
 * File:     cComcom.h
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


/** Computer Component
 *  Placeholder base class for computerised components.
 *  Computers have to implement animate and drawHUD methods to be useful.
 *  The message method isn't mandatory.
 */
struct rComputer : rRole {
public:
    virtual void message(cMessage* message) {};
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
    quat ori;
    vec3 pos;
    std::list<cObject*>* near;
    OID selected;
    bool switchnext;
    bool switchprev;
    bool switching;
public:
    rTarcom(cObject* obj = NULL);
    OID getSelected();
    void nextTarget();
    void prevTarget();
    virtual void animate(float spf);
    virtual void drawHUD();
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
    virtual void message(cMessage* message);
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

