/* 
 * File:     rCollider.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on April 3, 2011, 8:19 PM
 */

#ifndef RCOLLIDER_H
#define	RCOLLIDER_H

#include "rComponent.h"
#include "de/hackcraft/psi3d/math3d.h"

/**
 * Component for collision and collision response.
 * Currently only for cylinder collisions.
 */
struct rCollider : public rComponent {
public:
    /// Yet unused.
    enum CollisionType {
        CT_NONE, CT_CYLINDER, CT_SPHERE, CT_BOX
    };
public: // INPUT
    /// Base orientation (hook i).
    vec3 pos0;
    /// Base position (hook i).
    quat ori0;
    /// Radius for cylinder or sphere.
    float radius;
    /// Ratio of radius to height (height = ratio * radius) if not zero.
    float ratio;
    /// Height of cylinder if ratio not zero.
    float height;
public: // OUTPUT
protected: // INTERNALS
public:
    /// Constructor
    rCollider(cObject * obj);
    /// Destructor

    ~rCollider() {
    }

    virtual float constrain(float* worldpos, float radius, float* localpos, cObject * enactor);
    virtual void animate(float spf);
    virtual void drawEffect();
};

#endif	/* RCOLLIDER_H */

