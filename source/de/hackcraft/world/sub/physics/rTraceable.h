/* 
 * File:     rTraceable.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on April 3, 2011, 8:56 PM
 */

#ifndef RTRACEABLE_H
#define	RTRACEABLE_H

#include "de/hackcraft/world/Component.h"

#include "de/hackcraft/psi3d/Particle.h"

/**
 * Encapsulates physical state as far as movement and position is concerned.
 */
class rTraceable : public Component, public Particle {
public: // SYSTEM
    /** Identifier for this component. */
    static std::string cname;
    /** Just a unique random number (0-9999) to identify this component. */
    static unsigned int cid;
public: // INPUT
    /** Jetdrive throttle setting hook usually [0,1]. */
    float jetthrottle;
    /** Grounddrive throttle setting hook usually [0,1]. */
    float throttle;
public: // OUTPUT
    /** Averaged amount of groundedness [0,1] result from collision checks. */
    float grounded;
protected: // INTERNALS
public:
    /** Constructor. */
    rTraceable(Entity* obj = NULL);
    /** Copy Constructor. */
    rTraceable(rTraceable * original);
    /** Clone this. */
    virtual Component * clone();
    /** Accumulate steering and environmental forces. */
    void accumulate(float spf);
    /** Integrate position and derive velocity given forces and old state. */
    void integrate(float spf);
    /** Adjust position to nullify collisions. */
    void collide(float spf);
    /** Accumulate, integrate and collide. */
    virtual void animate(float spf);
};

#endif	/* RTRACEABLE_H */

