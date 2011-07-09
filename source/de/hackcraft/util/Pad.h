/**
 * File:     Pad.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on March 28, 2008, 10:23 PM
 *
 * Provides Uniform GamePad-Input in a Common Fashion.
 * Possibly-Mapped-Input-Sources:
 * - Keyboard(+Mouse), Real-GamePad, AI-Controll, Remote-Controll (&Proxies).
 * - Input recording and playback for demoloops.
 */

#if !defined(PAD_INC)
#define PAD_INC

#include <cstring>
#include <sstream>

/**
 * Emulation of a hardwired "universal" gamepad.
 * It's Structure is like that of available Gamepads.
 * A player and its data may be associated with the pad.
 */
class Pad {
public:
    /**
     * Identifiers for single (analouge) Axes:
     * UD1, LR1: Primary stick.
     * UD2, LR2: Secondary stick.
     */
    enum Axes {
        AX_LR1, AX_UD1, AX_UD2, AX_LR2,
        MAX_AXES
    };

    /**
     * Identifiers for single Buttons with groupings:
     * HU, HD, HL, HR: CoolieHat or digital axes as of yesterdays.
     * PU, PD, PL, PR: Primary action buttons (up,dn,lt,rt).
     * L1, L2: Left hand side buttons (top and bottom) [selections/camera/trigger].
     * R1, R2: Right hand side buttons (top and bottom) [selections/camera/trigger].
     * SELECT, START: Center buttons.
     * J1B, J2B: Buttons of the analouge sticks (pressing them down).
     */
    enum Buttons {
        BT_PL, BT_PU, BT_PD, BT_PR, // 4 Primary action buttons.
        BT_L1, BT_L2, BT_R1, BT_R2, // 2 + 2 Shoulder buttons.
        BT_SELECT, BT_START, // 2 Central controll buttons.
        BT_J1B, BT_J2B, // 2 Analouge stick push down buttons (or L3 and R3).
        BT_HU, BT_HD, BT_HL, BT_HR, // 4 HUD directional buttons.
        MAX_BUTTONS
    };

private:
    char axes[MAX_AXES];
    unsigned short buttons[MAX_BUTTONS];
    unsigned short buttons_[MAX_BUTTONS];
    void* playerdata;

public:
    /**
     * A new instance of a pad will be initialised to all zeros.
     */
    Pad() {
        //buttons = 0;
        for (int i = 0; i < MAX_BUTTONS; i++) {
            buttons[i] = 0;
        }
        reset();
    }

    /**
     * Methods for setting and resetting axis and button value.
     * Use the enum-constants of "Axes" and "Buttons" to access
     * to individual axes and buttons.
     *
     * Ranges in float for Axis are [-1,+1], whereas for int it is [-127,+127].
     */
    void reset() {
        //axes[0] = axes[1] = axes[2] = axes[3] = 0;
        //buttons_ = buttons;
        //buttons = 0;
        for (int i = 0; i < MAX_AXES; i++) {
            axes[i] = 0;
        }
        for (int i = 0; i < MAX_BUTTONS; i++) {
            buttons_[i] = buttons[i];
            buttons[i] = 0;
        }
    }

    /**
     * Set axis indicated by enum-constant to a value in [-127,+127] range.
     * The value is clamped to that interval.
     */
    void setAxis(Axes axis, int value) {
        this->axes[axis] = (value < -127) ? -127 : (value > +127) ? +127 : value;
    }

    /**
     * Set axis indicated by an enum-constant to a value in [-1.0,+1.0] range.
     * The value is clamped to that [-1.0,+1.0] interval and then mapped to
     * a [-127,+127] interval for internal storrage.
     */
    void setAxis(Axes axis, float value) {
        this->axes[axis] = (char) (127.0f * ((value < -1.0f) ? -1.0f : (value > +1.0f) ? +1.0f : value));
    }

    /**
     * Get the value of an axis indicated by an enum-constant.
     * The returned value is in [-1.0,+1.0] range representing
     * an internal range of [-127,+127].
     */
    float getAxis(Axes axis) {
        return (((float) this->axes[axis]) / 127.0f);
    }

    /**
     * Set a button indicated by an enum-constant to either un-/pressed.
     * True (unequal zero) is equal to pressed.
     */
    void setButton(Buttons button, bool enable) {
        //if (enable) this->buttons |= (1UL << button);
        //else this->buttons &= ~(1UL << button);
        buttons[button] = enable ? 1 : 0;
    }

    /**
     * Get the state of a button indicated by an enum-constant.
     * A value of 1 (int) is equal to a pressed button - zero otherwise.
     * Hint: Use the button state as a enabling factor in calculations.
     */
    int getButton(Buttons button) {
        //if ((this->buttons & (1UL << button)) != 0) return 1;
        //return 0;
        return buttons[button];
    }

    /**
     * Get the event-state of a button indicated by an enum-constant.
     * A value of 1 (int) is equal to a just-pressed or released button
     * after reset was called.
     */
    int getButtonEvent(Buttons button) {
        //if ((this->buttons & (1UL << button)) != (this->buttons_ & (1UL << button))) return 1;
        //return 0;
        return ((buttons[button] != buttons_[button]) ? 1 : 0);
    }

    /**
     * Puts the current pad configuration into a string and returns it.
     */
    std::string toString() {
        std::string div = "\n";
        std::stringstream s;
        s << "Pad {" << div
                << "J1(" << getAxis(AX_UD1) << ", " << getAxis(AX_LR1) << ")" << div
                << "J2(" << getAxis(AX_UD2) << ", " << getAxis(AX_LR2) << ")" << div
                << "LUDR-BT(" << getButton(BT_PL) << ", " << getButton(BT_PU) << ", " << getButton(BT_PD) << ", " << getButton(BT_PR) << ")" << div
                << "L1/R1(" << getButton(BT_L1) << ", " << getButton(BT_R1) << ")" << div
                << "L2/R2(" << getButton(BT_L2) << ", " << getButton(BT_R2) << ")" << div
                << "Sel/St(" << getButton(BT_SELECT) << ", " << getButton(BT_START) << ")" << div
                << "J1B/J2B(" << getButton(BT_J1B) << ", " << getButton(BT_J2B) << ")" << div
                << "UDLR-HUD(" << getButton(BT_HU) << ", " << getButton(BT_HD) << ", " << getButton(BT_HL) << ", " << getButton(BT_HR) << div
                << "}\n";
        return s.str();
    }

    /**
     * Input recording.
     * Takes a pointer to a buffer,
     * puts in the current pad configuration,
     * increments the buffer pointer by the size of the configuration
     * and returns that incremented pointer to the buffer
     * so that it can be used for the next call.
     */
    /*
    char* serialise(char *buffer) {
        int size = 8;
        memcpy(buffer, this, size);
        return (buffer + size);
    }
    */

    /**
     * Input playback for demo replay
     * Just like input recording but reads a pad
     * configuration from the buffer instead.
     */
    /*
    char* deserialise(char* buffer) {
        int size = 8;
        memcpy(this, buffer, size);
        return (buffer + size);
    }
    */
};

#endif
