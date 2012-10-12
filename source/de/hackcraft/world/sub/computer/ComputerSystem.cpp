#include "ComputerSystem.h"

#include "de/hackcraft/log/Logger.h"

#include "de/hackcraft/world/World.h"

#include "de/hackcraft/world/sub/computer/rComcom.h"
#include "de/hackcraft/world/sub/computer/rController.h"
#include "de/hackcraft/world/sub/computer/rForcom.h"
#include "de/hackcraft/world/sub/computer/rNavcom.h"


Logger* ComputerSystem::logger = Logger::getLogger("de.hackcraft.world.sub.computer.ComputerSystem");

ComputerSystem* ComputerSystem::instance = NULL;


ComputerSystem* ComputerSystem::getInstance() {
    return instance;
}


ComputerSystem::ComputerSystem() {
    instance = this;
}


ComputerSystem::~ComputerSystem() {
}


void ComputerSystem::add(rComcom* comcom) {
    comcoms.push_back(comcom);
    comcomsIndex[comcom->getId()] = comcom;
}


void ComputerSystem::add(rController* controller) {
    controllers.push_back(controller);
    controllersIndex[controller->getId()] = controller;
}


void ComputerSystem::add(rForcom* forcom) {
    forcoms.push_back(forcom);
    forcomsIndex[forcom->getId()] = forcom;
}


void ComputerSystem::add(rNavcom* navcom) {
    navcoms.push_back(navcom);
    navcomsIndex[navcom->getId()] = navcom;
}


void ComputerSystem::animateObjects() {
    
    float spf = World::getInstance()->getTiming()->getSPF();
    
    for(rComcom* component : comcoms) {
        component->prebind();
        component->animate(spf);
        component->postbind();
    }
    
    for(rController* component : controllers) {
        component->prebind();
        component->animate(spf);
        component->postbind();
    }
    
    for(rForcom* component : forcoms) {
        component->prebind();
        component->animate(spf);
        component->postbind();
    }
    
    for(rNavcom* component : navcoms) {
        component->prebind();
        component->animate(spf);
        component->postbind();
    }
}

