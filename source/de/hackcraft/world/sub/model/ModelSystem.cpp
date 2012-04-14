#include "ModelSystem.h"

ModelSystem* ModelSystem::instance = NULL;

ModelSystem::ModelSystem() {
    instance = this;
}

ModelSystem::ModelSystem(const ModelSystem& orig) {
}

ModelSystem::~ModelSystem() {
}

ModelSystem* ModelSystem::getInstance() {
    return instance;
}
