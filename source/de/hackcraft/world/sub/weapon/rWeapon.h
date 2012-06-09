/**
 * File:     rWeapon.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on March 5, 2008, 4:09 PM
 */

#ifndef _RWEAPON_H
#define	_RWEAPON_H

class rWeapon;

#include "de/hackcraft/psi3d/Particle.h"

#include "de/hackcraft/world/Entity.h"
#include "de/hackcraft/world/IModel.h"

#include <list>
#include <vector>

#define WEAPONDETAIL 0
#define WEAPONSOUND true
#define EXPLOSIONSOUND true

/**
 * Base-Class of all weapons.
 * Defines "all you need" for designing different kinds of weapons and
 * their visual effects. Note that weapons are themselves responsible for
 * drawing themselves, their bullets, missiles, dust, smoke and other particles.
 */
class rWeapon : public Component, public IModel {
public: // SYSTEM
    /** Instance counter. */
    static int sInstances;
    /** Texture Binds shared between instances. */
    static std::map<int, long> sTextures;

public: // INPUT
    /** Base position (hook i). */
    quat ori0;
    /** Base orientation (hook i). */
    vec3 pos0;
    /** Position relative to base position and orientation (hook i). */
    quat ori1;
    /** Orientation relative to base position and orientation (hook i). */
    vec3 pos1;
    /** Mountpoint-index of the weapon. */
    int weaponMount;
    /** Target object id for weapons that need targeting. (hook i) */
    OID target;
    /** Set to true to enable triggering (default true). (hook i) */
    bool triggeren;
    /** Triggering firing of weapon (once). (hook i) */
    bool trigger;
    /** Enable drawing of the weapon itself. */
    bool drawWeapon;
    /** Relative size and power of the weapon, 1.0 default mechscale weapon. */
    float weaponScale;
public: // OUTPUT
    /** True when firing was just really triggered last animation cycle. (hook o) */
    bool triggered;
    /** True when firing was just really triggered second last animation cycle. (hook o) */
    bool triggereded;
    /** Current Matrix indicating world pos & orientation (pose) of this weapon. */
    float weaponPosef[16];
    /** Remaining reload time until next firing is possible. */
    float timeReloading;
    /** Remaining readying time for bringing up the weapon (unused). */
    float timeReadying;
    /** Remaining firing time when firing takes more than an instant time. */
    float timeFiring;
    /** Remaining shots until reloading another clip (negative == infinitive?). */
    short remainingAmmo;
    /** Remaining clips (negative == infinitive?). */
    short remainingClips;
    /** Maximum amount of Ammo inside a clip. */
    short clipSize;
    /** Maximum amount of clips. */
    short depotSize;
    /** OpenAL sound source for this weapon's sounds. */
    unsigned soundSource;
protected: // INTERNALS
    /** Particle container with variable meaning. */
    std::list<Particle*> shrapnelParticles;
    /** Particle container with variable meaning. */
    std::list<Particle*> castoffParticles;
    /** Particle container with variable meaning. */
    std::list<Particle*> missileParticles;
    /** Particle container for particles with induce damage. */
    std::list<Particle*> damageParticles;

public:
    rWeapon(Entity* obj = NULL);

    /**
     * Get id - address for now.
     */
    virtual OID getId() { return (OID) this; }
    
    virtual int getPosX() { return (int) weaponPosef[12]; }
    virtual int getPosY() { return (int) weaponPosef[13]; }
    virtual int getPosZ() { return (int) weaponPosef[14]; }
    
    /**
     * Start playback of audio source.
     */
    void playSource();

    /**
     * Start playback of audio source if it isn't playing anymore.
     */
    void playSourceIfNotPlaying();

    /** "Eignungsbeurteilung"
     *
     * Fuzzy-Value [0,1] indicating usability or fittness
     * of this weapon to be fired right now (not considering weapon state).
     * May sum up (depends on Weapon) (future):
     * +Ability/Possibility to hit target (may roll some dice).
     * +Possible damage on target.
     * +Possible damage on other enemy units (owner's enemies).
     * -Possible self-damage.
     * -Possible ally-damage.
     * +In (optimal) Firerange.
     * -Out of (optimal) Firerange.
     */
    virtual float fit(OID target) {
        return 0.5f;
    };

    /**
     * Considering weapon state, true when weapon is
     * at hand, ready-loaded and unlocked.
     */
    virtual bool ready() {
        return triggeren && !((timeReloading > +0.0f)
                || (timeReadying > +0.0f)
                || (timeFiring > +0.0f)
                || (remainingAmmo == 0)
                );
    };

    /**
     * Update weapon animation cycles, trigger, particle trajectories and timers.
     */
    virtual void animate(float spf) = 0;

    /**
     * Calculate transformations for painting called between animate and draw*.
     * Calculates current pose according to owners Base and Mount
     * and sets position of sound-source.
     */
    virtual void transform();

    /**
     * Draw the weapon model itself and non-translucent particles.
     */
    virtual void drawSolid() = 0;

    /**
     * Draw translucent effects like weapon flares and particles.
     */
    virtual void drawEffect() = 0;

    /**
     * Paints an iconic display of the weapon type and
     * state into ortho [0..1,0..1] view.
     */
    virtual void drawHUD() {
    };

    /**
     * Returns amount of damaged objects (currently just one at most).
     */
    int damageByParticle(float* worldpos, float radius, int roles, float damage);

    /**
     * Helper function for transform, call to transform to world coord system.
     */
    void transformTo();
};

#include "rWeaponMachinegun.h"
#include "rWeaponPlasmagun.h"
#include "rWeaponSparkgun.h"
#include "rWeaponRaybeam.h"
#include "rWeaponHoming.h"
#include "rWeaponExplosion.h"

#endif	/* _RWEAPON_H */

