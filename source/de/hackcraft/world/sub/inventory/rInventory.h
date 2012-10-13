/* 
 * File:    rInventory.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on October 13, 2012, 6:50 PM
 */


#ifndef RINVENTORY_H
#define	RINVENTORY_H

class rInventory;

#include "de/hackcraft/world/Component.h"

//#include "de/hackcraft/world/sub/inventory/rInventoryItem.h"

#include <vector>

class rInventoryItem;

class rInventory : public Component {
public:
    rInventory();
    virtual ~rInventory();
private:
    enum InventoryType {
        INVENTORY_WALLET, // Monetary items with special treatment?
        INVENTORY_EQUIPPED,
        INVENTORY_QUICKBAR,
        INVENTORY_BAG,
        INVENTORY_KEYITEMS,
        INVENTORY_STORAGE,
        INVENTORY_CHEST,
        INVENTORY_SALE,
        INVENTORY_BUY
    };
    InventoryType type;
    std::vector<rInventoryItem*> inventoryItems;
};

#endif	/* RINVENTORY_H */

