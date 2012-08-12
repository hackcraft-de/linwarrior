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
    cameras.push_back(camera);
    camerasIndex[camera->getId()] = camera;
}


void CameraSystem::animateObjects() {
    
    float spf = World::getInstance()->getTiming()->getSPF();
    
    for(Component* component : cameras) {
        
        component->prebind();
        component->animate(spf);
        component->postbind();
    }
}


