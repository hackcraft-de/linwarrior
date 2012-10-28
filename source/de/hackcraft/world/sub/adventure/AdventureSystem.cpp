#include "AdventureSystem.h"

#include "de/hackcraft/log/Logger.h"

#include "de/hackcraft/world/World.h"

#include "de/hackcraft/world/sub/adventure/rInventory.h"
#include "de/hackcraft/world/sub/adventure/rItem.h"
#include "de/hackcraft/world/sub/adventure/rInventoryItem.h"


Logger* AdventureSystem::logger = Logger::getLogger("de.hackcraft.world.sub.adventure.AdventureSystem");

AdventureSystem* AdventureSystem::instance = NULL;


AdventureSystem* AdventureSystem::getInstance() {
    return instance;
}


AdventureSystem::AdventureSystem() {
    instance = this;
}


AdventureSystem::~AdventureSystem() {
}


void AdventureSystem::add(rInventory* inventory) {
    inventories.push_back(inventory);
    inventoriesIndex[inventory->getId()] = inventory;
}


void AdventureSystem::add(rInventoryItem* inventoryItem) {
    inventoryItems.push_back(inventoryItem);
    inventoryItemsIndex[inventoryItem->getId()] = inventoryItem;
}


void AdventureSystem::add(rItem* item) {
    items.push_back(item);
    itemsIndex[item->getId()] = item;
}

