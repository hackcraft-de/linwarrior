#include "rSpringlink.h"

#include "de/hackcraft/world/sub/physics/rTraceable.h"


rSpringlink::rSpringlink() {
}


rSpringlink::~rSpringlink() {
}


void rSpringlink::animate(float spf) {

    // Eventually for relaxation (with multiple iterations) with a value below 1.
    float adjustment = 1.0f;

    // Use spring force instead of a hard constraint?
    bool spring = (strength != 0);

    if (spring) {
        traceable0->applySpringForce(traceable1, adjustment * strength, restlength);
    } else {

        float* p0 = traceable0->pos;
        float* p1 = traceable1->pos;

        Particle::constrainParticleByStick(p0, p1, restlength, p0, adjustment, p1);
    }
}

