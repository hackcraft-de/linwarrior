/* 
 * File:     rNameable.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on April 3, 2011, 9:00 PM
 */

#ifndef RBILLBOARD_H
#define	RBILLBOARD_H

#include "de/hackcraft/world/Component.h"
#include "de/hackcraft/world/IModel.h"

#include "de/hackcraft/psi3d/math3d.h"

/**
 *  Encapsulates on-screen descriptive texts about an object.
 */
struct rBillboard : public Component, public IModel {
public: // SYSTEM
    /// Identifier for this component (all uppercase letters without leading "r").
    static std::string cname;
    /// A unique random number (0-9999) to identify this component.
    static unsigned int cid;
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
    rBillboard(Entity* obj = NULL);
    /// Copy Constructor.
    rBillboard(rBillboard * original);
    /// Clone this.
    virtual Component * clone();

    virtual int getPosX() { return (int) pos0[0]; }
    virtual int getPosY() { return (int) pos0[1]; }
    virtual int getPosZ() { return (int) pos0[2]; }
    virtual void animate(float spf) {};
    virtual void transform() {};
    virtual void drawSolid() {};
    virtual void drawEffect();
};

#endif	/* RNAMEABLE_H */

