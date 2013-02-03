/* 
 * File:     rSpringlink.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on February 3, 2013, 3:52 PM
 */

#ifndef RSPRINGLINK_H
#define	RSPRINGLINK_H

class rSpringlink;

#include "de/hackcraft/world/Component.h"

class rTraceable;

/**
 * Connects two traceables and applies a constraining spring-force
 * to the traceables when the distance between the two traceables changes.
 */
class rSpringlink : public Component {
    
public: // INPUT
    rTraceable* traceable0;
    rTraceable* traceable1;
    float restlength;
    float strength;
    
public:
    rSpringlink();
    virtual ~rSpringlink();

    /** Calculate and apply force according to difference in rest length */
    virtual void animate(float spf);
};

#endif	/* RSPRINGLINK_H */

