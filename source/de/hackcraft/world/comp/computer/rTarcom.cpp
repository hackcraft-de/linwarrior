#include "rTarcom.h"

#include "de/hackcraft/psi3d/GLS.h"
#include "de/hackcraft/psi3d/Primitive.h"
#include "de/hackcraft/psi3d/math3d.h"

#include "de/hackcraft/psi3d/ctrl.h"

#include "de/hackcraft/world/World.h"

#include "de/hackcraft/world/cObject.h"

rTarcom::rTarcom(cObject* obj) {
    object = obj;
    role = "TARCOM";

    quat_zero(ori0);
    vector_zero(pos0);

    near = new std::list<cObject*>();
    far = new std::list<cObject*>();
    enemies = NULL;
    selected = 0;
    nearbyEnemy = 0;

    switching = false;
    switchnext = false;
    switchprev = false;
}

void rTarcom::nextTarget() {
    bool found = false;
    OID last = 0;

    foreach(i, *near) {
        cObject* o = *i;
        if (last == selected) {
            selected = o->oid;
            found = true;
            break;
        }
        last = o->oid;
    }
    if (!found) {
        if (near->empty()) {
            selected = 0;
        } else {
            selected = near->front()->oid;
        }
    }
}

void rTarcom::prevTarget() {
    bool found = false;
    OID last = 0;

    foreach(i, *near) {
        cObject* o = *i;
        if (o->oid == selected) {
            selected = last;
            found = true;
            break;
        }
        last = o->oid;
    }
    if (!found) {
        if (near->empty()) {
            selected = 0;
        } else {
            selected = near->back()->oid;
        }
    }
}

void rTarcom::animate(float spf) {
    if (!active) return;

    if (!switching) {
        if (switchnext) {
            nextTarget();
            switching = true;
        } else if (switchprev) {
            prevTarget();
            switching = true;
        }
    } else {
        switching = (switchnext || switchprev);
    }

    unsigned long frame = World::instance->getTiming()->getFrame();
    unsigned long key = (((unsigned long) this) >> 1) * 174763;
    //cout << "RANGING KEY CODE IS " << key << " !!\n";
    //unsigned int m = 3;
    // Find all objects in far range.
    //unsigned char key0 = (key) % m;
    //if (key0 == framekey) {
    if ((key % 23) == (frame % 23)) {
        delete far;
        far = World::instance->filterByRange(object, pos0, 0, 100, -1, NULL);
        delete near;
        near = World::instance->filterByRange(object, pos0, 0, 55, -1, far);
        delete enemies;
        enemies = World::instance->filterByTags(object, &inc_enemies, false, -1, near);
        // Filter one nearby.
        nearbyEnemy = 0;
        for (std::list<cObject*>::iterator i = enemies->begin(); i != enemies->end(); i++) {
            cObject* o = *i;
            if (!o->anyTags(&exc_enemies)) {
                nearbyEnemy = o->oid;
                break;
            }
        }
    }
    /*
    unsigned int n = m / 3;
    unsigned char framekey = frame % m;
    unsigned char key1 = (key + 1 * n) % m;
    unsigned char key2 = (key + 2 * n) % m;
    // Find all objects in near range.
    if (key1 == framekey) {
        delete near;
        near = World::instance->filterByRange(object, pos, 0, 50, -1, far);
    }
    // Find all objects belonging to any enemy party/role.
    if (key2 == framekey) {
        delete enemies;
        enemies = World::instance->filterByTags(object, &inc_enemies, false, -1, near);
    }
     */
}

void rTarcom::drawHUD() {
    if (!active) return;

    //glColor4f(0,0.1,0,0.2);
    glBegin(GL_QUADS);
    glVertex3f(1, 1, 0);
    glVertex3f(0, 1, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(1, 0, 0);
    glEnd();
    glPushMatrix();
    {
        glScalef(0.5, 0.5, 1);
        glTranslatef(1, 1, 0);
        glColor4f(0.8, 0.8, 0.8, 0.5);
        glBegin(GL_LINE_STRIP);
        glVertex3f(-0.7, +0.7, 0);
        glVertex3f(+0.0, +0.0, 0);
        glVertex3f(+0.7, +0.7, 0);
        glEnd();
        glColor4f(0.0, 0.4, 0.0, 0.5);
        Primitive::glDisk(16, 1.0f);
        const float r2 = 0.7;
        glColor4f(0.0, 0.6, 0.0, 0.5);
        glScalef(r2, r2, 1);
        Primitive::glDisk(16, 1.0f);
        glScalef(1.0 / r2, 1.0 / r2, 1);
        quat ori_;
        quat_cpy(ori_, ori0);
        quat_conj(ori_);
        glRotatef(90, 1, 0, 0);
        GLS::glRotateq(ori_);
        glRotatef(-90, 1, 0, 0);
        glPointSize(3);
        glBegin(GL_POINTS);
        {
            glColor4f(1, 1, 1, 1);
            glVertex3f(0, 0, 0);
        }
        glEnd();

        foreach(i, *far) {
            glBegin(GL_POINTS);
            {
                cObject* o = *i;
                glColor4f(0.5, 0.5, 0.5, 1);
                if (o->hasTag(World::instance->getGroup(FAC_RED))) glColor4f(1, 0, 0, 1);
                else if (o->hasTag(World::instance->getGroup(FAC_GREEN))) glColor4f(0, 1, 0, 1);
                else if (o->hasTag(World::instance->getGroup(FAC_BLUE))) glColor4f(0, 0, 1, 1);
                else if (o->hasTag(World::instance->getGroup(FAC_YELLOW))) glColor4f(0, 1, 0, 1);
                if (o->oid == selected) glColor4f(1, 0, 1, 1);
                float dx = o->pos0[0] - pos0[0];
                float dz = o->pos0[2] - pos0[2];
                float r = sqrtf(dx * dx + dz * dz);
                dx /= r;
                dz /= r;
                float f = 0.01 * r;
                if (f < r2) {
                    glVertex3f(f * dx, -f * dz, 0);
                } else {
                    float r3 = r2 + (1.1 - r2) * log(1 + (f - r2));
                    if (r3 < 1.0) {
                        glVertex3f(r3 * dx, -r3 * dz, 0);
                    }
                }
            }
            glEnd();
        }
    }
    glPopMatrix();
}

bool rTarcom::isAllied(std::set<OID>* tags) {
    std::set<OID> inclusion;
    std::set_intersection(tags->begin(), tags->end(), inc_allies.begin(), inc_allies.end(), std::inserter(inclusion, inclusion.begin()));
    std::set<OID> exclusion;
    std::set_intersection(tags->begin(), tags->end(), exc_allies.begin(), exc_allies.end(), std::inserter(exclusion, exclusion.begin()));
    return (!inclusion.empty() && exclusion.empty());
}

void rTarcom::addAllied(OID tag, bool include) {
    if (include) {
        inc_allies.insert(tag);
    } else {
        exc_allies.insert(tag);
    }
}

void rTarcom::remAllied(OID tag, bool include) {
    if (include) {
        inc_allies.erase(tag);
    } else {
        exc_allies.erase(tag);
    }
}

bool rTarcom::isEnemy(std::set<OID>* tags) {
    std::set<OID> inclusion;
    std::set_intersection(tags->begin(), tags->end(), inc_enemies.begin(), inc_enemies.end(), std::inserter(inclusion, inclusion.begin()));
    std::set<OID> exclusion;
    std::set_intersection(tags->begin(), tags->end(), exc_enemies.begin(), exc_enemies.end(), std::inserter(exclusion, exclusion.begin()));
    return (!inclusion.empty() && exclusion.empty());
}

void rTarcom::addEnemy(OID tag, bool include) {
    if (include) {
        inc_enemies.insert(tag);
    } else {
        exc_enemies.insert(tag);
    }
}

void rTarcom::remEnemy(OID tag, bool include) {
    if (include) {
        inc_enemies.erase(tag);
    } else {
        exc_enemies.erase(tag);
    }
}
