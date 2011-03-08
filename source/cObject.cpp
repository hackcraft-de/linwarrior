#include "cObject.h"

#include "psi3d/macros.h"

#include <cassert>

#include <vector>
using std::vector;

#include <iostream>
using std::cout;
using std::endl;


// OBJECT

std::map<std::string, rRole*> cObject::roleprotos;
std::map<std::string, OID> cObject::roleoffsets;
//std::map<std::string, OID> cObject::roleids;
//std::map<OID, rRole*> cObject::roletypes;



// MISC FUNCTIONAL CODE

unsigned char* warningTexture(const char *fname, int *w, int* h, int* bpp) {
    *w = 8;
    *h = 8;
    *bpp = 3;
    unsigned char* bmp = new unsigned char[(*w)*(*h)*(*bpp)];
    int i, j;
    for (j = 0; j < *h; j++) {
        for (i = 0; i < *w; i++) {
            bmp[(j * (*w) + i) * 3 + 0] = ((i+j)&1) * 255;
            bmp[(j * (*w) + i) * 3 + 1] = ((i+j)&1) * 255;
            bmp[(j * (*w) + i) * 3 + 2] = 255;
        }
    }
    return bmp;
}

unsigned char* loadTGA(const char *fname, int *w, int* h, int* bpp) {
    typedef unsigned char BYTE;

    struct {
        BYTE idlength;
        BYTE colormaptype;
        BYTE imagetype;
        BYTE colormapspec[5];
        BYTE xorigin[2];
        BYTE yorigin[2];
        BYTE width[2];
        BYTE height[2];
        BYTE depth;
        BYTE descriptor;
    } tgahead;

    FILE *file;

    file = fopen(fname, "rb");
    if (file == NULL) {
        cout << "WARNING: Could not open tga file.\n";
        return warningTexture(fname, w, h, bpp);
    };

    size_t r = fread(&tgahead, sizeof (tgahead), 1, file);

    if (r != 1) {
        fclose(file);
        cout << "WARNING: Could not read tga header.\n";
        return warningTexture(fname, w, h, bpp);
    }

    if (tgahead.imagetype != 2) {
        fclose(file);
        cout << "WARNING: Only uncompressed true color tga images supported.\n";
        return warningTexture(fname, w, h, bpp);
    };

    if (tgahead.depth != 24 && tgahead.depth != 32) {
        fclose(file);
        cout << "WARNING: Only 24 or 32 bit per pixel tga images supported.\n";
        return warningTexture(fname, w, h, bpp);
    }

    for (int j = 0; j < tgahead.idlength; j++) fgetc(file);

#ifdef __BIG_ENDIAN__
    *w = (tgahead.width[0] << 8L)+(tgahead.width[1]);
    *h = (tgahead.height[0] << 8L)+(tgahead.height[1]);
#else
    *w = (tgahead.width[1] << 8L)+(tgahead.width[0]);
    *h = (tgahead.height[1] << 8L)+(tgahead.height[0]);
#endif
    *bpp = tgahead.depth >> 3;

    unsigned long size = (*w) * (*h) * (*bpp);
    unsigned char* bmp = new unsigned char[size];
    assert(bmp != NULL);
    r = fread(bmp, size, 1, file);
    if (r != 1) {
        fclose(file);
        cout << "WARNING: Could not read tga bitmap data.\n";
        return warningTexture(fname, w, h, bpp);
    }

    fclose(file);
    return bmp;
}


int saveTGA(const char *fname, int w, int h, int bpp, unsigned char* image) {
    typedef unsigned char BYTE;

    struct {
        BYTE idlength;
        BYTE colormaptype;
        BYTE imagetype;
        BYTE colormapspec[5];
        BYTE xorigin[2];
        BYTE yorigin[2];
        BYTE width[2];
        BYTE height[2];
        BYTE depth;
        BYTE descriptor;
    } tgahead;

    FILE *file;

    file = fopen(fname, "wb");
    if (file == NULL) {
        return -1;
    };

    tgahead.idlength = 0;
    tgahead.colormaptype = 0;
    tgahead.colormapspec[0] = 0;
    tgahead.colormapspec[1] = 0;
    tgahead.colormapspec[2] = 0;
    tgahead.colormapspec[3] = 0;
    tgahead.colormapspec[4] = 0;
    
    // Only uncompressed true color tga images supported.
    tgahead.imagetype = 2;

    tgahead.depth = bpp << 3;
    if (tgahead.depth != 24 && tgahead.depth != 32) {
        throw "Only 24 or 32 bit per pixel tga images supported.";
        return -1;
    }

    tgahead.xorigin[0] = 0;
    tgahead.xorigin[1] = 0;
    tgahead.yorigin[0] = 0;
    tgahead.yorigin[1] = 0;

#ifdef __BIG_ENDIAN__
    tgahead.width[0] = (w & 0xFF00) >> 8;
    tgahead.width[1] = (w & 0x00FF);
    tgahead.height[0] = (h & 0xFF00) >> 8;
    tgahead.height[1] = (h & 0x00FF);
#else
    tgahead.width[1] = (w & 0xFF00) >> 8;
    tgahead.width[0] = (w & 0x00FF);
    tgahead.height[1] = (h & 0xFF00) >> 8;
    tgahead.height[0] = (h & 0x00FF);
#endif

    size_t r = fwrite(&tgahead, sizeof (tgahead), 1, file);
    if (r != 1) throw "Could not write tga header.";

    unsigned long size = (w) * (h) * (bpp);
    if (image == NULL) {
        throw "Can't write NULL image.";
        return -1;
    }
    r = fwrite(image, size, 1, file);
    if (r != 1) throw "Could not write tga bitmap data.";

    fclose(file);
    return 0;
}


// NAMEABLE

rNameable::rNameable(cObject* obj) : name("Unnamed"), description("Undescribed"), designation(0) {
    role = "NAMEABLE";
    object = obj;
}

rNameable::rNameable(rNameable * original) : name("Unnamed"), description("Undescribed"), designation(0) {
    role = "NAMEABLE";
    if (original != NULL) {
        object = original->object;
        name = original->name;
        description = original->description;
        designation = original->designation;
    }
}

rRole* rNameable::clone() {
    return new rNameable(this);
}


// DAMAGEABLE

rDamageable::rDamageable(cObject* obj) {
    role = "DAMAGEABLE";
    object = obj;
    alife = true;
    loopi(MAX_PARTS) hp[i] = 100.0f;
}

rDamageable::rDamageable(rDamageable * original) {
    role = "DAMAGEABLE";
    if (original == NULL) {
        rDamageable();
    } else {
        object = original->object;
        alife = original->alife;
        loopi(MAX_PARTS) hp[i] = original->hp[i];
    }
}

rRole* rDamageable::clone() {
    return new rDamageable(this);
}

bool rDamageable::damage(int hitzone, float damage, cObject* enactor) {
    hp[hitzone] -= damage;
    if (hp[hitzone] < 0.0f) hp[hitzone] = 0.0f;
    if (hp[BODY] <= 0.0f) {
        alife = false;
    }
    return alife;
}


// CONTROLLED

#include "cPad.h"
#include "cController.h"

rControlled::rControlled(cObject* obj) : target(0), disturber(0), pad(new cPad()), controller(new cController(obj, true)) {
    role = "CONTROLLED";
    object = obj;
    vector_set(destination, float_NAN, float_NAN, float_NAN);
}

rControlled::rControlled(rControlled * original) {
    role = "CONTROLLED";
    if (original == NULL) {
        rControlled();
    } else {
        object = original->object;
        target = original->target;
        disturber = original->disturber;
        vector_cpy(destination, original->destination);
        pad = original->pad;
        controller = original->controller;
    }
}

rRole* rControlled::clone() {
    return new rControlled(this);
}

void rControlled::animate(float spf) {
    if (!active) return;
    //cout << mPad->toString();

    if (controller->controllerEnabled) {
        pad->reset();
        controller->controlledDevice = object;
        controller->process();
    }
}


// SOCIALISED

rSocialised::rSocialised(cObject* obj) {
    role = "SOCIALISED";
    object = obj;
}

rSocialised::rSocialised(rSocialised * original) {
    role = "SOCIALISED";
    if (original != NULL) {
        object = original->object;
        inc_allies = original->inc_allies;
        inc_enemies = original->inc_enemies;
        exc_allies = original->exc_allies;
        exc_enemies = original->exc_enemies;
    }
}

rRole* rSocialised::clone() {
    return new rSocialised(this);
}

bool rSocialised::isAllied(std::set<OID>* tags) {
    std::set<OID> inclusion;
    std::set_intersection(tags->begin(), tags->end(), inc_allies.begin(), inc_allies.end(), std::inserter(inclusion, inclusion.begin()));
    std::set<OID> exclusion;
    std::set_intersection(tags->begin(), tags->end(), exc_allies.begin(), exc_allies.end(), std::inserter(exclusion, exclusion.begin()));
    return (!inclusion.empty() && exclusion.empty());
}

void rSocialised::addAllied(OID tag, bool include) {
    if (include) {
        inc_allies.insert(tag);
    } else {
        exc_allies.insert(tag);
    }
}

void rSocialised::remAllied(OID tag, bool include) {
    if (include) {
        inc_allies.erase(tag);
    } else {
        exc_allies.erase(tag);
    }
}

bool rSocialised::isEnemy(std::set<OID>* tags) {
    std::set<OID> inclusion;
    std::set_intersection(tags->begin(), tags->end(), inc_enemies.begin(), inc_enemies.end(), std::inserter(inclusion, inclusion.begin()));
    std::set<OID> exclusion;
    std::set_intersection(tags->begin(), tags->end(), exc_enemies.begin(), exc_enemies.end(), std::inserter(exclusion, exclusion.begin()));
    return (!inclusion.empty() && exclusion.empty());
}

void rSocialised::addEnemy(OID tag, bool include) {
    if (include) {
        inc_enemies.insert(tag);
    } else {
        exc_enemies.insert(tag);
    }
}

void rSocialised::remEnemy(OID tag, bool include) {
    if (include) {
        inc_enemies.erase(tag);
    } else {
        exc_enemies.erase(tag);
    }
}


// TRACEABLE

#include "cWorld.h"

rTraceable::rTraceable(cObject* obj) : cParticle() {
    role = "TRACEABLE";
    object = obj;
    grounded = true;
    jetthrottle = 0.0f;
    throttle = 0.0f;
}

rTraceable::rTraceable(rTraceable * original) : cParticle(original) {
    role = "TRACEABLE";
    object = original->object;
    grounded = original->grounded;
    jetthrottle = original->jetthrottle;
    throttle = original->throttle;
}

rRole* rTraceable::clone() {
    return new rTraceable(this);
}

void rTraceable::accumulate(float spf) {
    // Accumulate steering forces
    {
        float jetuwd =  130000.0f * jetthrottle;
        float jetfwd =  -80000.0f * throttle * jetthrottle;
        float whlfwd = -600000.0f * throttle;
        if (friction <= 0.01f) whlfwd = 0.0f;
        float force[] = {0, jetuwd, jetfwd + whlfwd};
        quat_apply(force, ori, force);
        vector_add(fce, fce, force);
    }

    // Accumulate environmental forces
    applyGravityForce(cWorld::instance->getGravity());
    applyFrictionForce(spf);
    applyAirdragForce(cWorld::instance->getAirdensity());
}

void rTraceable::integrate(float spf) {
    // Integrate position and estimate current velocity...
    stepVerlet(1.0f / spf, spf*spf, 0.001f, 0.99f);
}

void rTraceable::collide(float spf) {
    // Constraint position...
    float depth = 0;
    {
        float center[3];
        vector_cpy(center, pos);
        center[1] += radius;
        depth = cWorld::instance->constrainParticle(object, center, radius);
        if (depth > 0.0f) {
            // There was a collision.
            // Set current position to corrected position.
            vector_cpy(pos, center);
            pos[1] -= radius;

            float delta[3];
            vector_set3i(delta, pos[i] - old[i]);
            float dist = vector_mag(delta);
            bool infinite = !finitef(pos[0]) || !finitef(pos[1]) || !finitef(pos[2]);
            if (infinite) {
                // The new position is not feasible.
                // (Re-)Set position to a position higher than the old position.
                old[1] += 0.3f;
                vector_cpy(pos, old);
                vector_set(vel, 0,0,0);
            } else if (dist > 1.3f) {
                // Way too fast.
                // Reset position.
                vector_cpy(old, pos);
            } else if (dist > 1.0f) {
                // Just too fast.
                // Rescale (normalize) movement.
                vector_scale(delta, delta, 1.0f / dist);
                vector_add(pos, old, delta);
                vector_cpy(vel, delta);
            }
        }
    }
    // Assume ground contact if there was a collision.
    float onground = (depth > 0.0f) ? 1.0f : 0.0f;
    // Average groundedness to decide on friction.
    grounded += 0.1 * (onground - grounded);
    // Set friction for next frame.
    friction = ((grounded > 0.1)?1.0f:0.0f) * cWorld::instance->getGndfriction();
}

void rTraceable::animate(float spf) {
    accumulate(spf);
    integrate(spf);
    collide(spf);
}



// GROUPING

rGrouping::rGrouping(cObject* obj) : name("Unnamed") {
    role = "GROUPING";
    object = obj;
}

rGrouping::rGrouping(rGrouping * original) : name("Unnamed") {
    role = "GROUPING";
    if (original != NULL) {
        object = original->object;
        name = original->name;
        members.clear();
        //mMembers.insert(mMembers.begin(), original->mMembers.begin(), original->mMembers.end());
    }
}

rRole* rGrouping::clone() {
    return new rGrouping(this);
}
