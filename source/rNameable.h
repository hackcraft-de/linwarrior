/* 
 * File:   rNameable.h
 * Author: benben
 *
 * Created on April 3, 2011, 9:00 PM
 */

#ifndef RNAMEABLE_H
#define	RNAMEABLE_H

#include "rComponent.h"
#include "psi3d/math3d.h"

/**
 *  Encapsulates onscreen descriptive texts about an object.
 */
struct rNameable : public rComponent {
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

    /// Constructor.
    rNameable(cObject* obj = NULL);
    /// Copy Constructor.
    rNameable(rNameable * original);
    /// Clone this.
    virtual rComponent* clone();

    virtual void drawEffect();
};

#endif	/* RNAMEABLE_H */

