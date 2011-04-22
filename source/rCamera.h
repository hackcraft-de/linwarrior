/* 
 * File:     rCamera.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on April 3, 2011, 8:33 PM
 */

#ifndef RCAMERA_H
#define	RCAMERA_H

#include "rComponent.h"

#include "psi3d/math3d.h"

/**
 * Camera component.
 */
struct rCamera : public rComponent {
public: // INPUT
    /// Base position (hook i).
    quat ori0;
    /// Base orientation (hook i).
    vec3 pos0;
    /// Position relative to base position and orientation (hook i).
    quat ori1;
    /// Orientation relative to base position and orientation (hook i).
    vec3 pos1;
    /// Switch camera state one step when greater zero. (hook i)
    float cameraswitch;
    /// Strenght of camera shaking [0,1], scaled internally. (hook i)
    float camerashake;
public: // OUTPUT
    /// True when camerastate is in first perspective mode. (hook o)
    bool firstperson;
protected: // INTERNALS
    /// # of camera modes: ISO C++ forbids const int with initialization >:(
    enum {
        MAX_CAMERAMODES = 6
    };
    /// Current Camera mode, negative number is indicating transition.
    int camerastate;
public:
    /// Constructor
    rCamera(cObject * obj);
    /// Mult in camera matrix.
    virtual void camera();

    virtual void animate(float spf);
};

#endif	/* RCAMERA_H */

