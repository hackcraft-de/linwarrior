#include "CameraSystem.h"

#include "de/hackcraft/log/Logger.h"

#include "de/hackcraft/world/World.h"

#include "de/hackcraft/world/sub/camera/rCamera.h"


Logger* CameraSystem::logger = Logger::getLogger("de.hackcraft.world.sub.camera.CameraSystem");

CameraSystem* CameraSystem::instance = NULL;


CameraSystem* CameraSystem::getInstance() {
    return instance;
}


CameraSystem::CameraSystem() {
    instance = this;
}


CameraSystem::~CameraSystem() {
}


void CameraSystem::add(rCamera* camera) {
    cameras[camera->getId()] = camera;
}


void CameraSystem::animateObjects() {
    
    float spf = World::getInstance()->getTiming()->getSPF();
    
    for(std::pair<OID,rCamera*> p : cameras) {
        Component* component = p.second;
        
        component->prebind();
        component->animate(spf);
        component->postbind();
    }
}


