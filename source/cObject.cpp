#include "cObject.h"

#include "psi3d/macros.h"

#include <cassert>

#include <vector>
using std::vector;

#include <iostream>
using std::cout;
using std::endl;

#include "psi3d/snippetsgl.h"
#include "psi3d/instfont.h"
DEFINE_glprintf


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

#include "psi3d/snippetsgl.h"

rNameable::rNameable(cObject* obj) : name("Unnamed"), description("Undescribed"), designation(0) {
    role = "NAMEABLE";
    object = obj;
    vector_zero(pos0);
    vector_zero(pos1);
    quat_zero(ori0);
    quat_zero(ori1);
    color[0] = color[1] = color[2] = color[3] = 1.0f;
    effect = true;
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

void rNameable::drawEffect() {
    if (!effect) return;
    
    glPushMatrix();
    {
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        {
            SGL::glUseProgram_fgplaintexture();
            glColor4fv(color);
            
            float s = 0.75;
            //glTranslatef(pos[0], pos[1] + 0.5f*s, pos[2]);
            glTranslatef(pos0[0], pos0[1], pos0[2]);
            SGL::glRotateq(ori0);
            glTranslatef(pos1[0], pos1[1], pos1[2]);
            bool billboard = true;
            if (!billboard) SGL::glRotateq(ori1);
            glScalef(s, s, s);
            if (billboard) {
                float n[16];
                SGL::glGetTransposeInverseRotationMatrix(n);
                glMultMatrixf(n);
            }
            int l = name.length();
            glTranslatef(-l * 0.5f, 0, 0);
            glprintf(name.c_str());
        }
        glPopAttrib();
    }
    glPopMatrix();
}


// DAMAGEABLE


rDamageable::rDamageable(cObject* obj) {
    role = "DAMAGEABLE";
    object = obj;
    alife = true;
    loopi(MAX_PARTS) hp[i] = 100.0f;
    disturber = 0;
}

rDamageable::rDamageable(rDamageable * original) {
    role = "DAMAGEABLE";
    if (original == NULL) {
        rDamageable();
    } else {
        object = original->object;
        alife = original->alife;
        loopi(MAX_PARTS) hp[i] = original->hp[i];
        disturber = original->disturber;
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
    if (enactor != NULL && damage > 0.001f) {
        disturber = enactor->oid;
    }
    return alife;
}

void rDamageable::drawHUD() {
    if (!active) return;

    //glColor4f(0,0.1,0,0.2);
    glBegin(GL_QUADS);
    glVertex3f(1, 1, 0);
    glVertex3f(0, 1, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(1, 0, 0);
    glEnd();
    glColor4f(0.3, 0.3, 0.3, 0.3);
    for (int i = 1; i <= 5; i += 4) {
        if (i == 1) glColor4f(1, 1, 1, 0.9);
        glLineWidth(i);
        glPushMatrix();
        {
            //glLoadIdentity();
            glTranslatef(0.0f, 0.4f, 0.0f);
            glScalef(0.333f, 0.3f, 1.0f);
            //glTranslatef(0.8, 0.1, 0);
            //glScalef(0.06, 0.08, 1.0);
            // Left Arm
            int left = rDamageable::LEFT;
            if (i != 1) glColor4f(1 - object->damageable->hp[left]*0.01, object->damageable->hp[left]*0.01, 0.4, 0.2);
            cPrimitives::glLineSquare(0.1f);
            // Torsor&Head
            glTranslatef(1, 0, 0);
            glScalef(1, 1.5, 1);
            int body = rDamageable::BODY;
            if (i != 1) glColor4f(1 - object->damageable->hp[body]*0.01, object->damageable->hp[body]*0.01, 0.4, 0.2);
            cPrimitives::glLineSquare(0.1f);
            // Right Arm&Shoulder
            glTranslatef(1, 0, 0);
            glScalef(1, 1.0f / 1.5f, 1);
            int right = rDamageable::RIGHT;
            if (i != 1) glColor4f(1 - object->damageable->hp[right]*0.01, object->damageable->hp[right]*0.01, 0.4, 0.2);
            cPrimitives::glLineSquare(0.1f);
            // Legs
            glTranslatef(-1.6, -1, 0);
            glScalef(2.2, 1, 1);
            int legs = rDamageable::LEGS;
            if (i != 1) glColor4f(1 - object->damageable->hp[legs]*0.01, object->damageable->hp[legs]*0.01, 0.4, 0.2);
            cPrimitives::glLineSquare(0.1f);
        }
        glPopMatrix();
    }

    glPushMatrix();
    {
        glColor4f(0.09, 0.99, 0.09, 1);
        glScalef(1.0f / 20.0f, 1.0f / 10.0f, 1.0f);
        glTranslatef(0, 1, 0);
        glprintf("hackcraft.de");
        glTranslatef(0, 9, 0);
        int left = rDamageable::LEFT;
        int body = rDamageable::BODY;
        int right = rDamageable::RIGHT;
        int legs = rDamageable::LEGS;
        glprintf("L %3.0f  T %3.0f  R %3.0f\n       B %3.0f", object->damageable->hp[left], object->damageable->hp[body], object->damageable->hp[right], object->damageable->hp[legs]);
    }
    glPopMatrix();
}


// CONTROLLED

#include "cPad.h"
#include "cController.h"

rControlled::rControlled(cObject* obj) : target(0), pad(new cPad()), controller(new cController(obj, true)) {
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
