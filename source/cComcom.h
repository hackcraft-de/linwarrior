/* 
 * File:     cComcom.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on August 4, 2009, 8:44 PM
 */

#ifndef _CCOMCOM_H
#define	_CCOMCOM_H

class cComputer;
class cComcom;
class cTarcom;
class cWepcom;
class cSyscom;
class cForcom;
class cNavcom;

#include "cMech.h"

// for Console
#include "psi3d/instfont.h"

class cComputer {
public:
    virtual void drawHUD() = 0;
    virtual void process(float spf) = 0;
};

/** Communication Computer
 *  encapsulates sending and receiving of messages
 *  and stores these messages.
 *  Orders to comrades may enhance strategic advance.
 */
class cComcom : public cComputer {
protected:
    cObject* mDevice;
    Console* mConsole;
    OID mLastMessage;
public:
    cComcom(cObject* device);
    void process(float spf);
    void drawHUD();
};

/** Targeting Computer,
 * includes Radar, target-selection and -display.
 */
class cTarcom : public cComputer {
public:
    cObject* mDevice;
    std::list<cObject*>* near;
    OID selected;
public:
    cTarcom(cObject* device);
    OID getSelected();
    void nextTarget();
    void prevTarget();
    void process(float spf);
    void drawHUD();
};

/** System Monitoring Computer
 *  includes damage indicators.
 */
class cSyscom : public cComputer {
public:
    cObject* mDevice;
public:
    cSyscom(cObject* device);
    void process(float spf);
    void drawHUD();
};

/** Weapon Computer
 * currently just draws weapon status.
 */
class cWepcom : public cComputer {
public:
    cObject* mDevice;
public:
    cWepcom(cObject* device);
    void process(float spf);
    void drawHUD();
};

/** Forward Computer
 * Forward-looking information overlay.
 */
class cForcom : public cComputer {
public:
    cMech* mDevice;
    std::string mMessage;
public:
    cForcom(cMech* device);
    void addMessage(std::string msg);
    void process(float spf);
    void drawHUD();
};

// Navigation Computer,
// stores mission critical POints of Interest (POIs),
// routes, map data and encapsulates path-finding.

class cNavcom : public cComputer {
public:
    cObject* mDevice;
    int mWaypoint;
    bool mCyclic;
    std::vector< int > mRoute;
    std::vector< std::vector<float> > mPOIs;
public:
    cNavcom(cObject* device);
    void drawPOI(float x, float y, float s);
    void process(float spf);
    void drawHUD();
};


#endif	/* _CCOMCOM_H */

