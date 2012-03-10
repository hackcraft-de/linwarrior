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
    /// Advance simulation time for one frame.
    void advanceTime(int deltamsec) {};
    /// Re-build spatial clustering of objects.
    void clusterObjects() {};
    /// Deliver overdue messages to objects.
    void dispatchMessages() {};
    /// Let all objects process input, adjust pose and calculate physics.
    void animateObjects() {};
    /// Let all objects calculate transformation matrices etc.
    void transformObjects() {};
    /// Setup structures for rendering
    void setupView(float* pos, float* ori);
    /// Draws background (skybox) by calling mBackground drawing method.
    void drawBack() {};
    /// Draw all Object's solid surfaces (calls their drawSolid method).
    void drawSolid() {};
    /// Draw all Object's effects (calls their drawEffect method).
    void drawEffect() {};
};

#endif	/* SUBSYSTEM_H */

