/* 
 * File:     rNameable.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on April 3, 2011, 9:00 PM
 */

#ifndef RNAMEABLE_H
#define	RNAMEABLE_H

#include "de/hackcraft/world/Component.h"

#include "de/hackcraft/psi3d/math3d.h"

/**
 *  Encapsulates onscreen descriptive texts about an object.
 */
struct rNameable : public Component {
public: // INPUT
    /// Base position for rendering. (hook i)
    vec3 pos0;
    /// Base orientation for rendering. (hook i)
    quat ori0;
    /// Extended position for rendering. (hook i)
    vec3 pos1;
    /// Extended orientation for rendering. (hook i)
    quat ori1;
    /// Color for rendering. (hook i)
    vec4 color;
    /// Enable effect rendering. (hook i)
    bool effect;
    /// Name or title of object or the document.
    std::string name;
    /// String describing the object or the document.
    std::string description;
    /// { 0, .., 25 } for { a, ..., z }.
    unsigned int designation;
public: // OUTPUT
public: // INTERNALS
public:
    /// Constructor.
    rNameable(Entity* obj = NULL);
    /// Copy Constructor.
    rNameable(rNameable * original);
    /// Clone this.
    virtual Component * clone();

    virtual void drawEffect();
};

#endif	/* RNAMEABLE_H */

