/* 
 * File:     rWepcom.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on September 15, 2011, 7:45 PM
 */

#ifndef RWEPCOM_H
#define	RWEPCOM_H

#include "de/hackcraft/world/Component.h"
#include "de/hackcraft/world/IComputer.h"

#include "rWeapon.h"

/** Weapon Computer
 * currently just draws weapon status.
 */
struct rWepcom : public Component, public IComputer {
public: // SYSTEM
    /** Identifier for this component (all uppercase letters without leading "r"). */
    static std::string cname;
    /** A unique random number (0-9999) to identify this component. */
    static unsigned int cid;
public:
    rWepcom(Entity* obj = NULL); // FIXME: Must be a cMech.
    /** Get id - address for now. */
    virtual OID getId() { return (OID) this; }
    virtual void animate(float spf);
    virtual void drawHUD();
    void addControlledWeapon(rWeapon* weapon);
    void fire();
private:
    /** Controlled/displayed weapons. */
    std::vector<rWeapon*> weapons;
    /** Currently selected weapon for firing. */
    int currentWeapon;
    /** Switch to next weapon after firing when true (default). */
    bool cycleWeapon;
    /** Fire a single weapon only at a time when true (default) */
    bool singleWeapon;
};

#endif	/* RWEPCOM_H */

