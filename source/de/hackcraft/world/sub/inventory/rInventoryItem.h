/* 
 * File:    rInventoryItem.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on October 13, 2012, 10:55 PM
 */


#ifndef RINVENTORYITEM_H
#define	RINVENTORYITEM_H

class rInventoryItem;

#include "de/hackcraft/world/Component.h"

class rItem;

class rInventoryItem : public Component {
public:
    rInventoryItem();
    virtual ~rInventoryItem();
private:
    int amount;
    rItem* item;
};

#endif	/* RINVENTORYITEM_H */

