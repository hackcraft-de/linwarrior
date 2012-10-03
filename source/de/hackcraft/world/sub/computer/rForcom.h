/* 
 * File:     rForcom.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on September 15, 2011, 7:49 PM
 */

#ifndef RFORCOM_H
#define	RFORCOM_H

struct rForcom;

#include "de/hackcraft/world/Component.h"

#include "de/hackcraft/world/sub/computer/IComputer.h"

#include "de/hackcraft/psi3d/math3d.h"

/** Forward Computer
 * Forward-looking information overlay.
 */
struct rForcom : public Component, public IComputer {
public: // SYSTEM
    /** Identifier for this component (all uppercase letters without leading "r"). */
    static std::string cname;
    /** A unique random number (0-9999) to identify this component. */
    static unsigned int cid;
public:
    std::string mMessage;
    quat ori;
    vec3 twr;
    bool reticle;
public:
    rForcom(Entity* obj = NULL); // FIXME: Must be a cMech.
    //virtual void message(Message * message);
    virtual void animate(float spf);
    virtual void drawHUD();
};


#endif	/* RFORCOM_H */

