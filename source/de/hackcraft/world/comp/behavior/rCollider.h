/* 
 * File:     rCollider.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on April 3, 2011, 8:19 PM
 */

#ifndef RCOLLIDER_H
#define	RCOLLIDER_H

#include "de/hackcraft/world/Component.h"

#include "de/hackcraft/psi3d/math3d.h"

/**
 * Component for collision and collision response.
 * Currently only for cylinder collisions.
 */
struct rCollider : public Component {
public: // SYSTEM
    /// Identifier for this component (all uppercase letters without leading "r").
    static std::string cname;
    /// A unique random number (0-9999) to identify this component.
    static unsigned int cid;
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
    rCollider(Entity * obj);
    /// Destructor

    ~rCollider() {
    }

    virtual float constrain(float* worldpos, float radius, float* localpos, Entity * enactor);
    virtual void animate(float spf);
    virtual void drawEffect();
};

#endif	/* RCOLLIDER_H */

