/* 
 * File:     rTraceable.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on April 3, 2011, 8:56 PM
 */

#ifndef RTRACEABLE_H
#define	RTRACEABLE_H

#include "rComponent.h"
#include "cParticle.h"

/**
 * Encapsulates physical state as far as movement and position is concerned.
 */
struct rTraceable : public rComponent, public cParticle {
    /// Averaged amount of groundedness [0,1] result from collision checks.
    float grounded;
    /// Jetdrive throttle setting hook usually [0,1].
    float jetthrottle;
    /// Grounddrive throttle setting hook usually [0,1].
    float throttle;

    /// Constructor.
    rTraceable(cObject* obj = NULL);
    /// Copy Constructor.
    rTraceable(rTraceable * original);
    /// Clone this.
    virtual rComponent * clone();
    /// Accumulate steering and environmental forces.
    void accumulate(float spf);
    /// Integrate position and derive velocity given forces and old state.
    void integrate(float spf);
    /// Adjust position to nullify collisions.
    void collide(float spf);
    /// Accumulate, integrate and collide.
    virtual void animate(float spf);
};

#endif	/* RTRACEABLE_H */

