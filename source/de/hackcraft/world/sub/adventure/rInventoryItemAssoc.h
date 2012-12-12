/* 
 * File:    rInventoryItemAssoc.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on October 13, 2012, 10:55 PM
 */


#ifndef RINVENTORYITEMASSOC_H
#define	RINVENTORYITEMASSOC_H

class rInventoryItemAssoc;

#include "de/hackcraft/world/Component.h"

class rItem;

class rInventoryItemAssoc : public Component {
public:
    rInventoryItemAssoc();
    virtual ~rInventoryItemAssoc();
private:
    int amount;
    rItem* item;
};

#endif	/* RINVENTORYITEM_H */

