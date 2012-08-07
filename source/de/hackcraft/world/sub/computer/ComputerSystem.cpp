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
    comcoms[comcom->getId()] = comcom;
}


void ComputerSystem::add(rController* controller) {
    controllers[controller->getId()] = controller;
}


void ComputerSystem::add(rForcom* forcom) {
    forcoms[forcom->getId()] = forcom;
}


void ComputerSystem::add(rNavcom* navcom) {
    navcoms[navcom->getId()] = navcom;
}


void ComputerSystem::animateObjects() {
    
    float spf = World::getInstance()->getTiming()->getSPF();
    
    for(std::pair<OID,rComcom*> p : comcoms) {
        Component* component = p.second;
        
        component->prebind();
        component->animate(spf);
        component->postbind();
    }
    
    for(std::pair<OID,rController*> p : controllers) {
        Component* component = p.second;
        
        component->prebind();
        component->animate(spf);
        component->postbind();
    }
    
    for(std::pair<OID,rForcom*> p : forcoms) {
        Component* component = p.second;
        
        component->prebind();
        component->animate(spf);
        component->postbind();
    }
    
    for(std::pair<OID,rNavcom*> p : navcoms) {
        Component* component = p.second;
        
        component->prebind();
        component->animate(spf);
        component->postbind();
    }
}


