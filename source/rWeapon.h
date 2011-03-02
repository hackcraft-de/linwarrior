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
#include "cObject.h"
#include <list>
#include <vector>

#include "cParticle.h"

#define WEAPONDETAIL 0
#define WEAPONSOUND true
#define EXPLOSIONSOUND true

/**
 * Base-Class of all weapons.
 * Defines "all you need" for designing different kinds of weapons and
 * their visual effects. Note that weapons are themselves responsible for
 * drawing themselves, their bullets, missiles, dust, smoke and other particles.
 */
class rWeapon : public rRole {
public:
    /// Instance counter.
    static int sInstances;
    /// Texture Binds shared between instances.
    static std::map<int,long> sTextures;

public:
    /// Object whielding the weapon (normaly not damaged by weapon).
    cObject* weaponOwner;
    /// Points To Current Vehicle Base Matrix of owner (optional).
    float* weaponBasefv;
    /// Base position and orientation.
    quat weaponOri0;
    vec3 weaponPos0;
    /// Position and orientation relative to base position and orientation.
    quat weaponOri1;
    vec3 weaponPos1;
    /// Mountpoint-index of the weapon.
    int weaponMount;
    /// Current Matrix indicating world pos & orientation (pose) of this weapon.
    float weaponPosef[16];
    /// Relative size and power of the weapon, 1.0 default mechscale weapon.
    float weaponScale;
    /// Remaining reload time until next firing is possible.
    float timeReloading;
    /// Remaining readying time for bringing up the weapon (unused).
    float timeReadying;
    /// Remaining firing time when firing takes more than an instant time.
    float timeFiring;
    /// Remaining shots until reloading another clip (negative == infinitive?).
    short remainingAmmo;
    /// Remaining clips (negative == infinitive?).
    short remainingClips;
    /// Maximum amount of Ammo inside a clip.
    short clipSize;
    /// Maximum amount of clips.
    short depotSize;
    /// OpenAL sound source for this weapon's sounds.
    unsigned soundSource;
    /// Enable drawing of the weapon itself.
    bool drawWeapon;
    /// Particle container with variable meaning.
    std::list<cParticle*> shrapnelParticles;
    /// Particle container with variable meaning.
    std::list<cParticle*> castoffParticles;
    /// Particle container with variable meaning.
    std::list<cParticle*> missileParticles;
    /// Particle container for particles with induce damage.
    std::list<cParticle*> damageParticles;

public:
    rWeapon(cObject* obj = NULL);

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
        return !((timeReloading > +0.0f)
                || (timeReadying > +0.0f)
                || (timeFiring > +0.0f)
                || (remainingAmmo == 0)
                );
    };

    /**
     * Fire Weapon at target, target may be 0.
     */
    virtual void fire(OID target) = 0;

    /**
     * Update weapon animation cycles, particle trajectories and timers.
     */
    virtual void animate(float spf) {
    };

    /**
     * Calculate transformations for painting called between animate and draw*.
     * Calculates current pose according to owners Base and Mount matrices
     * and sets position of sound-source.
     */
    virtual void transform();

    /**
     * Draw the weapon model itself and non-translucent particles.
     */
    virtual void drawSolid() {
    };

    /**
     * Draw translucent effects like weapon flares and particles.
     */
    virtual void drawEffect() {
    };

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

    void transformTo();
};

#include "rWeaponMachinegun.h"
#include "rWeaponPlasmagun.h"
#include "rWeaponSparkgun.h"
#include "rWeaponRaybeam.h"
#include "rWeaponHoming.h"
#include "rWeaponExplosion.h"

#endif	/* _CWEAPON_H */

