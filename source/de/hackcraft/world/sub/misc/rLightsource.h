/* 
 * File:    rLightsource.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on August 19, 2012, 12:14 PM
 */


#ifndef RLIGHTSOURCE_H
#define	RLIGHTSOURCE_H

class rLightsource;

#include "de/hackcraft/world/Component.h"

class Entity;

class rLightsource : public Component {
public:
    rLightsource(Entity* obj);
    virtual ~rLightsource();
private:

};

#endif	/* RLIGHTSOURCE_H */

