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
public:
    rWepcom(cObject* obj = NULL); // FIXME: Must be a cMech.
    virtual void animate(float spf);
    virtual void drawHUD();
};

#endif	/* RWEPCOM_H */

