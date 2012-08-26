#include "rInputsource.h"

#include "de/hackcraft/io/Pad.h"

#include <math.h>

// Gamepad Mapping

#define MECH_CHASSIS_LR_AXIS  AX_LR1
#define MECH_TURRET_LR_AXIS   AX_LR2
#define MECH_TURRET_UD_AXIS   AX_UD2
#define MECH_THROTTLE_AXIS    AX_UD1
#define MECH_FIRE_BUTTON1     BT_R1
#define MECH_FIRE_BUTTON2     BT_R2
#define MECH_JET_BUTTON1      BT_PD
#define MECH_JET_BUTTON2      BT_L2
#define MECH_CAMERA_BUTTON    BT_PU
#define MECH_NEXT_BUTTON      BT_PR
#define MECH_PREV_BUTTON      BT_PL


rInputsource::rInputsource(Entity* obj) {
    
    object = obj;
    
    fire = false;
    
    nextTarget = false;
    prevTarget = false;
    
    nextCamera = false;
    
    chassisUD = 0.0f;
    chassisLR = 0.0f;
    
    turretUD = 0.0f;
    turretLR = 0.0f;
    
    driveEnable = 0.0f;
    jetEnable = 0.0f;
    
    pad = NULL;
}


rInputsource::~rInputsource() {
}


void rInputsource::setPad(Pad* pad) {
    this->pad = pad;
}


Pad* rInputsource::getPad() {
    return pad;
}


void rInputsource::animate(float spf) {
    
    if (!active) return;
    
    if (pad == NULL) return;

    fire = (pad->getButton(Pad::MECH_FIRE_BUTTON1) || pad->getButton(Pad::MECH_FIRE_BUTTON2));
    
    nextTarget = pad->getButton(Pad::MECH_NEXT_BUTTON);
    prevTarget = pad->getButton(Pad::MECH_PREV_BUTTON);
    
    nextCamera = pad->getButton(Pad::MECH_CAMERA_BUTTON);
    
    turretLR = pad->getAxis(Pad::MECH_TURRET_LR_AXIS);
    turretUD = pad->getAxis(Pad::MECH_TURRET_UD_AXIS);
    
    chassisLR = pad->getAxis(Pad::MECH_CHASSIS_LR_AXIS);
    
    driveEnable = pad->getAxis(Pad::MECH_THROTTLE_AXIS);
    jetEnable = fmax(pad->getButton(Pad::MECH_JET_BUTTON1), pad->getButton(Pad::MECH_JET_BUTTON2));
    
    pad->reset();
}

