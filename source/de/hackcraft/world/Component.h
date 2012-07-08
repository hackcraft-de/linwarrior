/* 
 * File:     Component.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on April 3, 2011, 6:13 PM
 */

#ifndef RCOMPONENT_H
#define	RCOMPONENT_H

struct Component;

#include "de/hackcraft/lang/OID.h"

#include "de/hackcraft/util/Binding.h"

#include "de/hackcraft/world/Subsystem.h"

#include <string>
#include <vector>

class Entity;

/**
 * Base-Class of all components.
 * A components can be understood as a set of attributes reflecting
 * a certain part, component or aspect of the whole object.
 * Input to and output from components should rely soley on attributes
 * or signal variables for i/o impules.
 */
struct Component {
    /** Identifier for this component (all uppercase letters without leading "r"). */
    static std::string cname;
    /** A unique random number (0-9999) to identify this component. */
    static unsigned int cid;

    /** Parent object of which this component is a part of. */
    Entity* object;
    /** Indicates an active enabled component (or disabled if false). */
    bool active;
    /** Bindings to be executed before animation. */
    std::vector<Binding*> prebinds;
    /** Bindings to be executed after animation (if any). */
    std::vector<Binding*> postbinds;
    

    Component(Component* original = NULL) {
        if (original != NULL) {
            object = original->object;
            active = original->active;
        } else {
            object = NULL;
            active = true;
        }
    }

    virtual Component * clone() {
        return NULL;
    }

    void addBinding(void* dst, void* src, unsigned int size) {
        prebinds.push_back(new Binding(dst, src, size));
    }
    
    virtual OID getId() {
        return (OID) this;
    }
    
    virtual void spawn() {
    }

    virtual void frag() {
    }

    virtual void camera() {
    }

    virtual void listener() {
    }

    virtual float constrain(float* worldpos, float radius, float* localpos = NULL, Entity * enactor = NULL) {
        return 0.0f;
    }

    virtual bool damage(float* localpos, float damage, Entity * enactor = NULL) {
        return true;
    }

    void prebind() {
        for (Binding* binding : prebinds) {
            binding->read();
        }
    }

    virtual void animate(float spf) {
    }

    void postbind() {
        for (Binding* binding : postbinds) {
            binding->read();
        }
    }

    virtual void transform() {
    }

    virtual void drawSolid() {
    }

    virtual void drawEffect() {
    }

    virtual void drawHUD() {
    }
};

#endif	/* RCOMPONENT_H */

