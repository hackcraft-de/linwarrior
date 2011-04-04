/* 
 * File:     rComponent.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on April 3, 2011, 6:13 PM
 */

#ifndef RCOMPONENT_H
#define	RCOMPONENT_H

#include <cassert>
#include <string>
#include <set>
#include <map>

class cObject;
class cMessage;

/**
 * Base-Class of all components.
 * A components can be understood as a set of attributes reflecting
 * a certain part, component or aspect of the whole object.
 * Input to and output from components should rely soley on attributes
 * or signal variables for i/o impules.
 */
struct rComponent {
    /// Parent object of which this component is a part of.
    cObject* object;
    /// Typename/Role of this component (all uppercase letters without leading "r").
    std::string role;
    /// Indicates an active enabled component (or disabled if false).
    bool active;

    rComponent(rComponent* original = NULL) {
        if (original != NULL) {
            object = original->object;
            active = original->active;
            role = original->role;
        } else {
            object = NULL;
            active = true;
            role = "";
        }
    }

    virtual rComponent * clone() {
        return new rComponent(this);
    }

    virtual float constrainParticle(float* worldpos, float radius, float* localpos, cObject * enactor) {
        return 0.0f;
    };

    virtual void message(cMessage * message) {
    };

    virtual void animate(float spf) {
    };

    virtual void transform() {
    };

    virtual void drawSolid() {
    };

    virtual void drawEffect() {
    };

    virtual void drawHUD() {
    };
};

/*
#include "rNameable.h"
#include "rTraceable.h"
#include "rDamageable.h"
#include "cController.h"
#include "rGrouping.h"
#include "rRigged.h"
#include "rCamera.h"
#include "rCollider.h"
#include "rMobile.h"
 */

#endif	/* RCOMPONENT_H */

