/* 
 * File:     Particle.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on December 22, 2009, 4:58 PM
 */

#ifndef _CPARTICLE_H
#define	_CPARTICLE_H

typedef unsigned long long OBID;

// for vector ops
#include "de/hackcraft/psi3d/math3d.h"
// for float_NAN
#include "de/hackcraft/psi3d/macros.h"

#include <list>
#include <vector>
#include <cmath>
#include <iostream>
#include <cstdio>

//#undef sqrtf

/**
 * A Particle can be anything from bullets over flares to smoke.
 * It has a position and a velocity. Besides it has a lifetime
 * countdown (fuel), a time in existence timer and a spawn countdown
 * which is for spawning particle offspring. A particle can have a
 * trail consisting of other particles like smoke or other
 * recursive systems (in this context I hereby coin the term Particle-Tree ;).
 * Note: There are ways to produces trees (plants) using particle systems
 * but that's another story.
 */
struct Particle {
    /** Particle orientation quaternion - may use as euler. */
    quat ori;
    /** Particle position vector. */
    vec3 pos;
    /** Particle old position vector. */
    vec3 old;
    /** Particle velocity vector. */
    vec3 vel;
    /** Particle force accumulation vector. */
    vec3 fce;
    /** Particle mass. */
    float mass;
    /** The inverse of the particle mass is often neccessary. */
    float mass_inv;
    /** Set when having contact according to contact-depth and material. */
    float friction;
    /** CW coefficient times area in m2 for calculating air-drag. */
    float cwm2;
    /** Remaining fuel in seconds. */
    float fuel;
    /** Time since start. */
    float timer;
    /** Countdown til spawning next Smoke-Particle. */
    float spawn;
    /** Object-ID of Particle target - attraction object or evasion. */
    OBID target;
    /** Abstract dimension of Particle by sphere radius. */
    float radius;
    /** A sound-source-id just in case this particle emits sound. */
    unsigned int sound;
    /** A texture-id when a particle has an individual texture. */
    unsigned int texture;
    /** User-type specifier or flags, program defines behavior and shape. */
    int type;
    /** Specific user-data - eg billboard text, color curves, object, etc. */
    void* data;
    /** Recursive sub Particles - or trail of particles. */
    std::list<Particle*> trail;

    Particle() :
    mass(1.0f), mass_inv(1.0f), friction(0.0f), cwm2(0.0f), fuel(1), timer(0),
    spawn(-1), target(0), radius(0), sound(-1), texture(-1), type(0), data(NULL) {
        quat_zero(ori);
        vector_zero(pos);
        vector_zero(old);
        vector_zero(vel);
        vector_zero(fce);
    }

    /** Copy Constructor. */

    Particle(Particle * original) {
        if (original == NULL) {
            throw "Particle: Null instance for copy when calling copy constructor.";
        }
        quat_cpy(ori, original->ori);
        vector_cpy(pos, original->pos);
        vector_cpy(old, original->old);
        vector_cpy(vel, original->vel);
        vector_cpy(fce, original->fce);
        mass = original->mass;
        mass_inv = original->mass_inv;
        friction = original->friction;
        cwm2 = original->cwm2;
        fuel = original->fuel;
        timer = original->timer;
        spawn = original->spawn;
        target = original->target;
        radius = original->radius;
        sound = original->sound;
        texture = original->texture;
        type = original->type;
        data = original->data;


        for (auto i = original->trail.begin(); i != original->trail.end(); i++) {
            trail.push_back(new Particle(*i));
        }
        //for (Particle* i: original->trail) {
            //trail.push_back(new Particle(*i));
        //}
    }

    /** Apply gravity to the particle's force, eg with float g[] = {0,-9.8f,0}. */

    inline void applyGravityForce(float* gravity_m_per_s) {
        if (!finitef(mass_inv)) return;
        fce[0] += mass * gravity_m_per_s[0];
        fce[1] += mass * gravity_m_per_s[1];
        fce[2] += mass * gravity_m_per_s[2];
    }

    /** Apply fake friction based on velocity and mass. */

    inline void applyFrictionForce(float dt) {
        if (friction == 0.0f) return;
        float c_step = friction / dt; //(1.0f - friction);
        vector_muladd(fce, fce, vel, -c_step * mass);
    }

    /** Apply billboard airdrag based on front surface area and cw-value. */

    inline void applyAirdragForce(float density_kg_per_m3 = 1.204f) {
        if (cwm2 == 0.0f) return;
        const float p = density_kg_per_m3;
        float vx2 = vel[0] * vel[0];
        float vy2 = vel[1] * vel[1];
        float vz2 = vel[2] * vel[2];
        float velocity = sqrtf(vx2 + vy2 + vz2);
        float velocity_inv = (velocity != 0) ? (1.0f / velocity) : (0.0f);
        float pstau = p * 0.5f * velocity * velocity;
        float F = cwm2 * pstau;
        vector_muladd(fce, fce, vel, -F * velocity_inv);
    }

    /** Apply spring force to both particles and return force. */

    inline float applySpringForce(Particle* other, float strength, float rest_length) {
        float* c = this->pos;
        float* p = other->pos;
        float cp[3] = {
            (p[0] - c[0]),
            (p[1] - c[1]),
            (p[2] - c[2])
        };
        float actual_length = sqrtf(cp[0] * cp[0] + cp[1] * cp[1] + cp[2] * cp[2]);

        float force = strength * (rest_length - actual_length) * -0.5f;

        float force3[] = {cp[0] * force, cp[1] * force, cp[2] * force};

        c[0] += force3[0] * this->mass_inv;
        c[1] += force3[1] * this->mass_inv;
        c[2] += force3[2] * this->mass_inv;
        p[0] -= force3[0] * other->mass_inv;
        p[1] -= force3[1] * other->mass_inv;
        p[2] -= force3[2] * other->mass_inv;

        return force;
    }

    /**
     * Euler integration works by integrating force to velocity
     * and then further into position which is stored in old position
     * beforehand. It is fast and not bad but poses problems when
     * the particle position needs to be adjusted due to collisions
     * because current velocity needs to be adjusted, too.
     * It may be ok for elastic particles but for non-elastic particles
     * verlet integration is simpler.
     * @param dt the elapsed time.
     * @param damping pseudo damping factor within [0,1], typically very small.
     */
    inline void stepEuler(float dt, float damping = 0.01f) {
        // Calling static method for calculating step.
        stepEuler(pos, old, vel, fce, mass_inv, dt, damping);
    }

    /**
     * Euler integration works by integrating force to velocity
     * and then further into position which is stored in old position
     * beforehand. It is fast and not bad but poses problems when
     * the particle position needs to be adjusted due to collisions
     * because current velocity needs to be adjusted, too.
     * It may be ok for elastic particles but for non-elastic particles
     * verlet integration is simpler.
     * @param pos current and then derived particle position (3fv).
     * @param old memory for the next previous particle position (3fv).
     * @param vel current particle velocity (3fv).
     * @param fce current applied force (3fv).
     * @param dt the elapsed time.
     * @param damping pseudo damping factor within [0,1], typically very small.
     */
    static inline void stepEuler(float* pos, float* old, float* vel, float* fce, float mass_inv, float dt, float damping = 0.01f) {
        vel[0] *= (1.0f - damping);
        vel[1] *= (1.0f - damping);
        vel[2] *= (1.0f - damping);
        vel[0] += fce[0] * mass_inv;
        vel[1] += fce[1] * mass_inv;
        vel[2] += fce[2] * mass_inv;
        fce[0] = fce[1] = fce[2] = 0.0f;
        old[0] = pos[0];
        old[1] = pos[1];
        old[2] = pos[2];
        pos[0] += dt * vel[0];
        pos[1] += dt * vel[1];
        pos[2] += dt * vel[2];
    }

    /**
     * Verlet integration derives the new position from
     * the old position the current position and force applied.
     * Velocity is thus implicitly used and is derived.
     * Non-elastic collision correction is a snap - just adjust the position.
     * @param dt_inv inverse of the elapsed time.
     * @param dt2 square of the elapsed time.
     * @param damping pseudo damping factor within [0,1], typically very small.
     */
    inline void stepVerlet(float dt_inv, float dt2, float damping = 0.01f, float velalpha = 1.0f) {
        // Calling static method for calculating step.
        stepVerlet(pos, old, vel, fce, mass_inv, dt_inv, dt2, damping, velalpha);
    }

    /**
     * Verlet integration derives the new position from
     * the old position the current position and force applied.
     * Velocity is thus implicitly used and is derived.
     * Non-elastic collision correction is a snap - just adjust the position.
     * @param pos current particle position (3fv).
     * @param old previous particle position (3fv).
     * @param vel memory for derived velocity (3fv).
     * @param fce current applied force (3fv).
     * @param dt_inv inverse of the elapsed time.
     * @param dt2 square of the elapsed time.
     * @param damping pseudo damping factor within [0,1], typically very small.
     */
    static inline void stepVerlet(float* pos, float* old, float* vel, float* fce, float mass_inv, float dt_inv, float dt2, float damping = 0.01f, float velalpha = 1.0f) {
        // Velocity is just a function of position with verlet (here with averaging, velalpha == 1 no averaging).
        vel[0] += velalpha * ((pos[0] - old[0]) * dt_inv - vel[0]);
        vel[1] += velalpha * ((pos[1] - old[1]) * dt_inv - vel[1]);
        vel[2] += velalpha * ((pos[2] - old[2]) * dt_inv - vel[2]);
        // Temporarily we need the acceleration resulting from previous force.
        float acc[] = {
            fce[0] * mass_inv,
            fce[1] * mass_inv,
            fce[2] * mass_inv
        };
        // Restart accumulating force for next verlet step.
        fce[0] = fce[1] = fce[2] = 0.0f;
        // Calculate next position.
        float f = damping;
        float next[] = {
            (2.0f - f) * pos[0] - (1.0f - f) * old[0] + acc[0] * dt2,
            (2.0f - f) * pos[1] - (1.0f - f) * old[1] + acc[1] * dt2,
            (2.0f - f) * pos[2] - (1.0f - f) * old[2] + acc[2] * dt2
        };
        old[0] = pos[0];
        old[1] = pos[1];
        old[2] = pos[2];
        pos[0] = next[0];
        pos[1] = next[1];
        pos[2] = next[2];
    }

    static inline float constrainParticleByStick(float* particle3fv, float* center3fv, float radius, float* projection3fv, float adjustment = 1.0f, float* centerprojection3fv = NULL) {
        // Vector from center to particle.
        float cp[] = {
            particle3fv[0] - center3fv[0],
            particle3fv[1] - center3fv[1],
            particle3fv[2] - center3fv[2]
        };

        float cp_len = sqrt_(cp[0] * cp[0] + cp[1] * cp[1] + cp[2] * cp[2]);
        float depth = (radius - cp_len);
        float norm_depth = (depth / cp_len) * adjustment;

        projection3fv[0] = particle3fv[0] + cp[0] * norm_depth;
        projection3fv[1] = particle3fv[1] + cp[1] * norm_depth;
        projection3fv[2] = particle3fv[2] + cp[2] * norm_depth;

        // Is the other end of the stick loose (!= NULL) or fixed (== NULL)?
        if (centerprojection3fv != NULL) {
            centerprojection3fv[0] = center3fv[0] - cp[0] * norm_depth;
            centerprojection3fv[1] = center3fv[1] - cp[1] * norm_depth;
            centerprojection3fv[2] = center3fv[2] - cp[2] * norm_depth;
        }

        return depth;
    }

    static inline float constraintParticleBySphere(float* particle3fv, float* center3fv, float radius, float* projection3fv) {
        float r2 = radius * radius;
        // Vector from center to particle.
        float cp[] = {
            particle3fv[0] - center3fv[0],
            particle3fv[1] - center3fv[1],
            particle3fv[2] - center3fv[2]
        };

        float cp_len2 = cp[0] * cp[0] + cp[1] * cp[1] + cp[2] * cp[2];
        if (cp_len2 > r2) return 0;

        float cp_len = sqrt_(cp_len2);
        float depth = (radius - cp_len);
        float norm_offset = (depth / cp_len);

        projection3fv[0] = particle3fv[0] + cp[0] * norm_offset;
        projection3fv[1] = particle3fv[1] + cp[1] * norm_offset;
        projection3fv[2] = particle3fv[2] + cp[2] * norm_offset;

        return depth;
    }

    static inline float constraintParticleByCone(float* particle3fv, float* base3fv, float radius, float height, float* projection3fv) {
        float r2 = radius * radius;
        float dist[] = {particle3fv[0] - base3fv[0], 0, particle3fv[2] - base3fv[2]};

        // r2dist < r2 inside
        float r2dist = dist[0] * dist[0] + dist[2] * dist[2];
        if (r2dist > r2) return 0;

        // bottom < 0 inside
        float bottom = base3fv[1] - particle3fv[1];
        if (bottom > 0) return 0;

        // top < 0 inside
        float top = particle3fv[1] - (base3fv[1] + height);
        if (top > 0) return 0;

        // Distance from cone center pole.
        float rdist = sqrt_(r2dist);

        // Normalised Tangent vector of base circle below particle.
        float inv_len = 1.0f / rdist;
        float tangent[] = {-dist[2] * inv_len, 0, dist[0] * inv_len};
        //std::cout << "tangent: " << tangent[0] << " " << tangent[1] << " " << tangent[2] << "\n";

        // Tangent point.
        float tp[] = {radius * dist[0] * inv_len, base3fv[1], radius * dist[2] * inv_len};
        //std::cout << "tp: " << tp[0] << " " << tp[1] << " " << tp[2] << "\n";

        // Normalised Vector from tangent point to cone top tip.
        //float inv_up = 1.0f / sqrtf(radius*radius + height*height);
        float inv_up = 1.0f / sqrtf(tp[0] * tp[0] + tp[2] * tp[2] + height * height);
        float up[] = {-tp[0] * inv_up, height * inv_up, -tp[2] * inv_up};
        //std::cout << "up: " << up[0] << " " << up[1] << " " << up[2] << "\n";

        // Normalised projection normal vector.
        float norm[3];
        vector_cross(norm, tangent, up);
        //std::cout << "norm: " << norm[0] << " " << norm[1] << " " << norm[2] << "\n";

        // Plane equation d.
        float d = norm[1] * height;
        //std::cout << "Invariant d: " << d << "\n";

        // Distance of Particle to plane.
        float delta = norm[0] * particle3fv[0] + norm[1] * particle3fv[1] + norm[2] * particle3fv[2] - d;
        //std::cout << "Cone Distance: " << delta << "\n";

        if (delta > 0) {
            projection3fv[0] = particle3fv[0] - delta * norm[0];
            projection3fv[1] = particle3fv[1] - delta * norm[1];
            projection3fv[2] = particle3fv[2] - delta * norm[2];
            return delta;
        }

        return 0.0f;
    }

    static inline float constraintParticleByCylinder(float* particle3fv, float* base3fv, float radius, float height, float* projection3fv) {
        float r2 = radius * radius;
        float dist[] = {particle3fv[0] - base3fv[0], 0, particle3fv[2] - base3fv[2]};

        // r2dist < r2 inside
        float r2dist = dist[0] * dist[0] + dist[2] * dist[2];
        if (r2dist > r2) return 0;

        // bottom < 0 inside
        float bottom = base3fv[1] - particle3fv[1];
        if (bottom > 0) return 0;

        // top < 0 inside
        float top = particle3fv[1] - (base3fv[1] + height);
        if (top > 0) return 0;

        float rdist = sqrt_(r2dist);
        float side = rdist - radius;

        bool top_or_bottom_not_side = top > side || bottom > side;
        bool top_not_bottom = top > bottom;

        if (top_or_bottom_not_side) {
            if (top_not_bottom) {
                projection3fv[0] = particle3fv[0];
                projection3fv[1] = base3fv[1] + height;
                projection3fv[2] = particle3fv[2];
                return -top;
            } else {
                projection3fv[0] = particle3fv[0];
                projection3fv[1] = base3fv[1];
                projection3fv[2] = particle3fv[2];
                return -bottom;
            }
        } else {
            float proj = (radius / rdist);
            projection3fv[0] = base3fv[0] + dist[0] * proj;
            projection3fv[1] = particle3fv[1];
            projection3fv[2] = base3fv[2] + dist[2] * proj;
            return -side;
        }
    }

    static inline float constraintParticleByBox(float* particle3fv, float* min3fv, float* max3fv, float* projection3fv) {
        float dminmax[6];
        float* dmin = &dminmax[0];
        float* dmax = &dminmax[3];
        vector_sub(dmin, min3fv, particle3fv);
        vector_sub(dmax, particle3fv, max3fv);
        float maxval = -1000000;
        int maxidx = 0;

        for (int i = 0; i < 6; i++) {
            //cout << dminmax[i] << "\n";
            if (dminmax[i] > 0) return 0;
            if (dminmax[i] > maxval) {
                maxval = dminmax[i];
                maxidx = i;
            }
        }

        //cout << maxval << " " << maxidx << "\n";
        vector_cpy(projection3fv, particle3fv);

        if (maxidx < 3) {
            projection3fv[maxidx] = min3fv[maxidx];
        } else {
            projection3fv[maxidx - 3] = max3fv[maxidx - 3];
        }

        return -maxval;
    }

public:

    /** Utility function to compute nearest rotation direction and intensity. */

    static void rotationTo(float* result2f, float* own_pos, float* tgt_pos, float* base_ori, float*tower_ori = NULL) {
        float dir_global[3];
        vector_sub(dir_global, tgt_pos, own_pos);
        quat ori_inv;
        quat_cpy(ori_inv, base_ori);
        if (tower_ori) quat_mul(ori_inv, ori_inv, tower_ori);
        quat_conj(ori_inv);
        quat dir_local;
        quat_apply(dir_local, ori_inv, dir_global);
        vector_norm(dir_local, dir_local);
        result2f[0] = dir_local[0];
        if (dir_local[2] > 0) result2f[0] += copysign(1.0f, result2f[0]);
        result2f[1] = dir_local[1];
    }

private:

    static inline float sqrt_(float value) {
        //static long long a = 1;
        //static long long b = 1;
        if (value >= 1.0f && value < 9.0) {
            //a++;
            //cout << " " << a << " " << b << " " << "\n";
            float guess = 1.0f + (value - 1.0f) * 0.45f;
            return 0.5f * (guess + value / guess);
        } else {
            //b++;
            //cout << " " << a << " " << b << " " << "\n";
            return sqrtf(value); //exp(0.5f * log(value));
        }
    }
};


#endif	/* _CPARTICLE_H */

