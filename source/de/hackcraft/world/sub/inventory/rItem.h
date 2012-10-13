/* 
 * File:    rItem.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on October 13, 2012, 6:52 PM
 */


#ifndef RITEM_H
#define	RITEM_H

class rItem;

#include "de/hackcraft/world/Component.h"

#include <string>

class rItem : public Component {
public:
    rItem();
    virtual ~rItem();
private:
    std::string name;
    std::string description;
};

#endif	/* RITEM_H */

