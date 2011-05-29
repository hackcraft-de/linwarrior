#include "rGrouping.h"

// GROUPING

rGrouping::rGrouping(cObject* obj) : name("Unnamed") {
    role = "GROUPING";
    object = obj;
}

rGrouping::rGrouping(rGrouping * original) : name("Unnamed") {
    role = "GROUPING";
    if (original != NULL) {
        object = original->object;
        name = original->name;
        members.clear();
        //mMembers.insert(mMembers.begin(), original->mMembers.begin(), original->mMembers.end());
    }
}

rComponent* rGrouping::clone() {
    return new rGrouping(this);
}

