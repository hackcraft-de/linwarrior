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
#include "psi3d/math3d.h"

/**
 * Component for collision and collision response.
 * Currently only for cylinder collisions.
 */
struct rCollider : public rComponent {
    quat ori;
    vec3 pos;
    /// Radius for cylinder or sphere.
    float radius;
    /// Ratio of radius to height (height = ratio * radius) if not zero.
    float ratio;
    /// Height of cylinder if ratio not zero.
    float height;

    /// Constructor
    rCollider(cObject * obj);
    /// Destructor

    ~rCollider() {
    }

    virtual float constrainParticle(float* worldpos, float radius, float* localpos, cObject * enactor);
    virtual void animate(float spf);
    virtual void drawEffect();
};

#endif	/* RCOLLIDER_H */

