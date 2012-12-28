#include "rTrigger.h"

#include "de/hackcraft/world/Entity.h"


rTrigger::rTrigger(Entity* obj) {
    object = obj;
    //id = object->oid;
    id = (OID) this;
    
    pos0[0] = pos0[1] = pos0[2] = 0;
}

/*
rTrigger::rTrigger() {
}

rTrigger::rTrigger(const rTrigger& orig) {
}

rTrigger::~rTrigger() {
}
*/
