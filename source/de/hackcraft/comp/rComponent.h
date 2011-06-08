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
#include <string.h>
//#include <set>
//#include <map>
#include <vector>

class cObject;
class Message;

/**
 * Base-Class of all components.
 * A components can be understood as a set of attributes reflecting
 * a certain part, component or aspect of the whole object.
 * Input to and output from components should rely soley on attributes
 * or signal variables for i/o impules.
 */
struct rComponent {
    /// Represents a binding of a variable src to a variable dst of a given size.
    struct Binding {
        void* dst;
        void* src;
        unsigned int size;
        Binding(void* dst, void* src, unsigned int size) {
            this->dst = dst;
            this->src = src;
            this->size = size;
        }
        /// Store from destination to source.
        void write() {
            memcpy(src, dst, size);
        }
        /// Load from source to destination.
        void read() {
            memcpy(dst, src, size);
        }
    };

    /// Parent object of which this component is a part of.
    cObject* object;
    /// Typename/Role of this component (all uppercase letters without leading "r").
    std::string role;
    /// Indicates an active enabled component (or disabled if false).
    bool active;
    /// Bindings to be executed before animation.
    std::vector<Binding*> prebinds;
    /// Bindings to be executed after animation (if any).
    std::vector<Binding*> postbinds;
    

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

    void addBinding(void* dst, void* src, unsigned int size) {
        prebinds.push_back(new Binding(dst, src, size));
    }

    virtual void spawn() {
    }

    virtual void frag() {
    }

    virtual void camera() {
    }

    virtual void listener() {
    }

    virtual float constrain(float* worldpos, float radius, float* localpos = NULL, cObject * enactor = NULL) {
        return 0.0f;
    }

    virtual bool damage(float* localpos, float damage, cObject * enactor = NULL) {
        return true;
    }

    virtual void message(Message * message) {
    }

    void prebind() {
        for (auto i = prebinds.begin(); i != prebinds.end(); i++) {
            (*i)->read();
        }
    }

    virtual void animate(float spf) {
    }

    void postbind() {
        for (auto i = prebinds.begin(); i != prebinds.end(); i++) {
            (*i)->read();
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

