/* 
 * File:     IComputer.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on September 15, 2011, 7:40 PM
 */

#ifndef ICOMPUTER_H
#define	ICOMPUTER_H

/** 
 * Class-Interface for computerized components.
 * Computers have to implement animate and drawHUD methods to be useful.
 */
class IComputer {
public:
    virtual void animate(float spf) = 0;
    virtual void drawHUD() = 0;
};

#endif	/* ICOMPUTER_H */

