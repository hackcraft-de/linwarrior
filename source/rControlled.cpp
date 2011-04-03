#include "rControlled.h"

#include "psi3d/macros.h"

#include "cPad.h"
#include "cController.h"

rControlled::rControlled(cObject* obj) : target(0), pad(new cPad()), controller(new cController(obj, true)) {
    role = "CONTROLLED";
    object = obj;
    vector_set(destination, float_NAN, float_NAN, float_NAN);
}

rControlled::rControlled(rControlled * original) {
    role = "CONTROLLED";
    if (original == NULL) {
        rControlled();
    } else {
        object = original->object;
        target = original->target;
        vector_cpy(destination, original->destination);
        pad = original->pad;
        controller = original->controller;
    }
}

rComponent* rControlled::clone() {
    return new rControlled(this);
}

void rControlled::animate(float spf) {
    if (!active) return;
    //cout << mPad->toString();

    if (controller->controllerEnabled) {
        pad->reset();
        controller->controlledDevice = object;
        controller->process();
    }
}

