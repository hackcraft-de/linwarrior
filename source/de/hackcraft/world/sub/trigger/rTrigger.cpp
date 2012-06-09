#include "rTrigger.h"

#include "de/hackcraft/world/Entity.h"


rTrigger::rTrigger(Entity* obj) {
    object = obj;
    //id = object->oid;
    id = (OID) this;
}

/*
rTrigger::rTrigger() {
}

rTrigger::rTrigger(const rTrigger& orig) {
}

rTrigger::~rTrigger() {
}
*/
