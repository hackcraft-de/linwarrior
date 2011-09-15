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

#include "rComputer.h"

#include "de/hackcraft/psi3d/math3d.h"

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
    virtual void message(Message * message);
    virtual void animate(float spf);
    virtual void drawHUD();
};


#endif	/* RFORCOM_H */

