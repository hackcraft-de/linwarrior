/* 
 * File:     IComputer.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on September 15, 2011, 7:40 PM
 */

#ifndef RCOMPUTER_H
#define	RCOMPUTER_H

#include "de/hackcraft/world/Component.h"

/** Computer Component
 *  Placeholder base class for computerized components.
 *  Computers have to implement animate and drawHUD methods to be useful.
 *  The message method isn't mandatory.
 */
class IComputer {
public:
    virtual void animate(float spf) = 0;
    virtual void drawHUD() = 0;
};

#endif	/* RCOMPUTER_H */

