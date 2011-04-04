#include "rTraceable.h"

#include "cWorld.h"

rTraceable::rTraceable(cObject* obj) : cParticle() {
    role = "TRACEABLE";
    object = obj;
    grounded = true;
    jetthrottle = 0.0f;
    throttle = 0.0f;
}

rTraceable::rTraceable(rTraceable * original) : cParticle(original) {
    role = "TRACEABLE";
    object = original->object;
    grounded = original->grounded;
    jetthrottle = original->jetthrottle;
    throttle = original->throttle;
}

rComponent* rTraceable::clone() {
    return new rTraceable(this);
}

void rTraceable::accumulate(float spf) {
    // Accumulate steering forces
    {
        float jetuwd = 130000.0f * jetthrottle;
        float jetfwd = -80000.0f * throttle * jetthrottle;
        float whlfwd = -600000.0f * throttle;
        if (friction <= 0.01f) whlfwd = 0.0f;
        float force[] = {0, jetuwd, jetfwd + whlfwd};
        quat_apply(force, ori, force);
        vector_add(fce, fce, force);
    }

    // Accumulate environmental forces
    applyGravityForce(cWorld::instance->getGravity());
    applyFrictionForce(spf);
    applyAirdragForce(cWorld::instance->getAirdensity());
}

void rTraceable::integrate(float spf) {
    // Integrate position and estimate current velocity...
    stepVerlet(1.0f / spf, spf*spf, 0.001f, 0.99f);
}

void rTraceable::collide(float spf) {
    // Constraint position...
    float depth = 0;
    {
        float center[3];
        vector_cpy(center, pos);
        center[1] += radius;
        depth = cWorld::instance->constrainParticle(object, center, radius);
        if (depth > 0.0f) {
            // There was a collision.
            // Set current position to corrected position.
            vector_cpy(pos, center);
            pos[1] -= radius;

            float delta[3];
            vector_set3i(delta, pos[i] - old[i]);
            float dist = vector_mag(delta);
            bool infinite = !finitef(pos[0]) || !finitef(pos[1]) || !finitef(pos[2]);
            if (infinite) {
                // The new position is not feasible.
                // (Re-)Set position to a position higher than the old position.
                old[1] += 0.3f;
                vector_cpy(pos, old);
                vector_set(vel, 0, 0, 0);
            } else if (dist > 1.3f) {
                // Way too fast.
                // Reset position.
                vector_cpy(old, pos);
            } else if (dist > 1.0f) {
                // Just too fast.
                // Rescale (normalize) movement.
                vector_scale(delta, delta, 1.0f / dist);
                vector_add(pos, old, delta);
                vector_cpy(vel, delta);
            }
        }
    }
    // Assume ground contact if there was a collision.
    float onground = (depth > 0.0f) ? 1.0f : 0.0f;
    // Average groundedness to decide on friction.
    grounded += 0.1 * (onground - grounded);
    // Set friction for next frame.
    friction = ((grounded > 0.1) ? 1.0f : 0.0f) * cWorld::instance->getGndfriction();
}

void rTraceable::animate(float spf) {
    accumulate(spf);
    integrate(spf);
    collide(spf);
}

