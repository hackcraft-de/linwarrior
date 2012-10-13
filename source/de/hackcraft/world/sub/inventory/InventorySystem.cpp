#include "InventorySystem.h"

#include "de/hackcraft/log/Logger.h"

#include "de/hackcraft/world/World.h"

#include "de/hackcraft/world/sub/inventory/rInventory.h"
#include "de/hackcraft/world/sub/inventory/rItem.h"
#include "de/hackcraft/world/sub/inventory/rInventoryItem.h"


Logger* InventorySystem::logger = Logger::getLogger("de.hackcraft.world.sub.inventory.InventorySystem");

InventorySystem* InventorySystem::instance = NULL;


InventorySystem* InventorySystem::getInstance() {
    return instance;
}


InventorySystem::InventorySystem() {
    instance = this;
}


InventorySystem::~InventorySystem() {
}


void InventorySystem::add(rInventory* inventory) {
    inventories.push_back(inventory);
    inventoriesIndex[inventory->getId()] = inventory;
}


void InventorySystem::add(rInventoryItem* inventoryItem) {
    inventoryItems.push_back(inventoryItem);
    inventoryItemsIndex[inventoryItem->getId()] = inventoryItem;
}


void InventorySystem::add(rItem* item) {
    items.push_back(item);
    itemsIndex[item->getId()] = item;
}

