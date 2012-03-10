/* 
 * File:     rWepcom.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on September 15, 2011, 7:45 PM
 */

#ifndef RWEPCOM_H
#define	RWEPCOM_H

#include "rComputer.h"

/** Weapon Computer
 * currently just draws weapon status.
 */
struct rWepcom : public rComputer {
public: // SYSTEM
    /// Identifier for this component (all uppercase letters without leading "r").
    static std::string cname;
    /// A unique random number (0-9999) to identify this component.
    static unsigned int cid;
public:
    rWepcom(Entity* obj = NULL); // FIXME: Must be a cMech.
    virtual void animate(float spf);
    virtual void drawHUD();
};

#endif	/* RWEPCOM_H */

