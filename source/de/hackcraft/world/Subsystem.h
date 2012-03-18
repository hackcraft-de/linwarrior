/* 
 * File:     Subsystem.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on February 19, 2012, 5:41 PM
 */

#ifndef SUBSYSTEM_H
#define	SUBSYSTEM_H

class Subsystem {
public:
    /// Advance simulation time for one frame.
    virtual void advanceTime(int deltamsec) {};
    /// Re-build spatial clustering of objects.
    virtual void clusterObjects() {};
    /// Deliver overdue messages to objects.
    virtual void dispatchMessages() {};
    /// Let all objects process input, adjust pose and calculate physics.
    virtual void animateObjects() {};
    /// Let all objects calculate transformation matrices etc.
    virtual void transformObjects() {};
    /// Setup structures for rendering
    virtual void setupView(float* pos, float* ori) {};
    /// Draws background (skybox).
    virtual bool drawBack() { return false; };
    /// Draw all Object's solid surfaces (calls their drawSolid method).
    virtual void drawSolid() {};
    /// Draw all Object's effects (calls their drawEffect method).
    virtual void drawEffect() {};
};

#endif	/* SUBSYSTEM_H */

