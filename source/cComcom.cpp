#include "cComcom.h"

#include "cWorld.h"

#include "psi3d/snippetsgl.h"

// for Console and glprintf
#include "psi3d/instfont.h"

DEFINE_Console_printf
DEFINE_glprintf

// FIXME: Forcom and Wepcom rely on mech.
#include "cMech.h"

#include <iostream>
using std::cout;
using std::endl;


// -----------------------------------------------


rComcom::rComcom(cObject* obj) {
    object = obj;
    role = "COMCOM";

    int w = 20;
    int h = 10;
    mConsole = Console_new(w * h, w);
    assert(mConsole != NULL);
    Console* console = (Console*) mConsole;
    //Console_printf(mConsole, "* Welcome to LinWarrior3D\n");
    Console_printf(console, "ComCOM(1) - MENU\n\n");
    Console_printf(console, " [1] Formation\n");
    Console_printf(console, " [2] Engage\n");
    Console_printf(console, " [3] Report\n");
    Console_printf(console, " [4] Objectives\n");
    Console_printf(console, "\n\n > ");
    mLastMessage = 0;
}

void rComcom::animate(float spf) {
    cMessage* message = NULL;//cWorld::instance->recvMessage(0, mLastMessage);
    if (message != NULL) {
        mLastMessage = message->getTimestamp();
    }
}

void rComcom::drawHUD() {
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
        glColor4f(0, 1, 0, 0.9);
        //glColor4f(0.99, 0.99, 0.19, 1);
        Console* console = (Console*) mConsole;
        float w = console->width;
        float h = console->size / console->width;
        glTranslatef(0, 1.0f + (1.0f / h), 0);
        glScalef(1.0f / w, 1.0f / h, 1.0f);
        Console_draw(console);
    }
    glPopMatrix();
}

// -----------------------------------------------

rTarcom::rTarcom(cObject* obj) {
    object = obj;
    role = "TARCOM";

    quat_zero(ori);
    vector_zero(pos);

    near = new std::list<cObject*>();
    far = new std::list<cObject*>();
    enemies = NULL;
    selected = 0;

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

    unsigned long frame = cWorld::instance->getTiming()->getFrame();
    unsigned long key = (((unsigned long)this)>>1) * 174763;
    //cout << "RANGING KEY CODE IS " << key << " !!\n";
    unsigned int m = 3;
    // Find all objects in far range.
    //unsigned char key0 = (key) % m;
    //if (key0 == framekey) {
    if ((key % 23) == (frame % 23)) {
        delete far;
        far = cWorld::instance->filterByRange(object, pos, 0, 100, -1, NULL);
        delete near;
        near = cWorld::instance->filterByRange(object, pos, 0, 50, -1, far);
        delete enemies;
        enemies = cWorld::instance->filterByTags(object, &inc_enemies, false, -1, near);
    }
    /*
    unsigned int n = m / 3;
    unsigned char framekey = frame % m;
    unsigned char key1 = (key + 1 * n) % m;
    unsigned char key2 = (key + 2 * n) % m;
    // Find all objects in near range.
    if (key1 == framekey) {
        delete near;
        near = cWorld::instance->filterByRange(object, pos, 0, 50, -1, far);
    }
    // Find all objects belonging to any enemy party/role.
    if (key2 == framekey) {
        delete enemies;
        enemies = cWorld::instance->filterByTags(object, &inc_enemies, false, -1, near);
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
        cPrimitives::glDisk(16, 1.0f);
        const float r2 = 0.7;
        glColor4f(0.0, 0.6, 0.0, 0.5);
        glScalef(r2, r2, 1);
        cPrimitives::glDisk(16, 1.0f);
        glScalef(1.0 / r2, 1.0 / r2, 1);
        quat ori_;
        quat_cpy(ori_, ori);
        quat_conj(ori_);
        glRotatef(90, 1,0,0);
        SGL::glRotateq(ori_);
        glRotatef(-90, 1,0,0);
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
                if (o->hasTag(cObject::RED)) glColor4f(1, 0, 0, 1);
                else if (o->hasTag(cObject::GREEN)) glColor4f(0, 1, 0, 1);
                else if (o->hasTag(cObject::BLUE)) glColor4f(0, 0, 1, 1);
                else if (o->hasTag(cObject::YELLOW)) glColor4f(0, 1, 0, 1);
                if (o->oid == selected) glColor4f(1,0,1,1);
                float dx = o->traceable->pos[0] - pos[0];
                float dz = o->traceable->pos[2] - pos[2];
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

// -----------------------------------------------

rWepcom::rWepcom(cObject* obj) {
    object = obj;
    role = "WEPCOM";
}

void rWepcom::animate(float spf) {
}

void rWepcom::drawHUD() {
    if (!active) return;

    cMech* mech = (cMech*) object;
    float h = 1.0f / 7.0f * (1 + (mech->weapons.size() + 1) / 2);
    glBegin(GL_QUADS);
    glVertex3f(1, h, 0);
    glVertex3f(0, h, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(1, 0, 0);
    glEnd();
    glPushMatrix();
    {
        glScalef(1.0f / 2.0f, 1.0f / 7.0f, 1.0f);
        glTranslatef(0, 0.5, 0);

        loopi(mech->weapons.size()) {
            glLineWidth(5);
            if (mech->weapons[i]->ready()) glColor4f(0.4f, 1.0f, 0.4f, 0.2f);
            else glColor4f(0.8f, 0.0f, 0.0f, 0.2f);
            cPrimitives::glLineSquare(0.1f);
            glLineWidth(1);
            glColor4f(1.0f, 1.0f, 1.0f, 0.9f);
            cPrimitives::glLineSquare(0.1f);
            glPushMatrix();
            {
                mech->weapons[i]->drawHUD();
            }
            glPopMatrix();
            //glTranslatef(0, 1, 0);
            if (i & 1) glTranslatef(-1, 1, 0);
            else glTranslatef(1, 0, 0);
        }
    }
    glPopMatrix();
}

// -----------------------------------------------

rForcom::rForcom(cObject* obj) {
    object = obj;
    role = "FORCOM";
    quat_zero(ori);
    vector_zero(twr);
}

void rForcom::message(cMessage* message) {
    mMessage = message->getText();
}

void rForcom::animate(float spf) {
}

void rForcom::drawHUD() {
    if (!active) return;

    // Reticle in screen center.
    if (reticle) {
        glPushMatrix();
        {
            glTranslatef(0.5, 0.5, 0);
            glScalef(0.03, 0.04, 1);
            glPointSize(2);
            glBegin(GL_POINTS);
            {
                glColor4f(1, 0, 0, 0.2);
                glVertex3f(0, 0, 0);
            }
            glEnd();
            glBegin(GL_LINES);
            {
                const float a = 0.7;
                // left
                glVertex3f(-1, 0, 0);
                glVertex3f(-a, 0, 0);
                // low
                glVertex3f(0, -1, 0);
                glVertex3f(0, -a, 0);
                // right
                glVertex3f(+1, 0, 0);
                glVertex3f(+a, 0, 0);
                // diagonal
                glVertex3f(-a, 0, 0);
                glVertex3f(0, -a, 0);
                glVertex3f(0, -a, 0);
                glVertex3f(+a, 0, 0);
            }
            glEnd();
        }
        glPopMatrix();
    }

    // Compass-Lines at top of HUD
    if (!true) {
        glPushMatrix();
        {
            glTranslatef(0.5, 0.97, 0);
            glScalef(1.0, 0.02, 1.0);
            glColor4f(0, 1, 0, 0.2);

            float r = atan2(ori[1], ori[3]) / PI_OVER_180;
            if (r > 180) r -= 360;
            const float d = 1.0f / 360.0f;

            glLineWidth(3);
            glBegin(GL_LINES);
            {
                glColor4f(0, 0.3, 1, 0.2);
                float s = 1.2;
                for (int i = -180; i < 180; i += 10) {
                    float t = fmod(r, 10);
                    glVertex3f((i + t) * d, -s, 0);
                    glVertex3f((i + t) * d, +s, 0);
                }
            }
            glEnd();

            glLineWidth(1);
            glBegin(GL_LINES);
            {
                glColor4f(1, 1, 1, 0.6);
                float s = 1.0;
                for (int i = -180; i < 180; i += 10) {
                    float t = fmod(r, 10);
                    glVertex3f((i + t) * d, -s, 0);
                    glVertex3f((i + t) * d, +s, 0);
                }
            }
            glEnd();

        }
        glPopMatrix();
    } // Compass

    // Ornamental HUD-Border-Frame
    if (true) {
        glLineWidth(5);
        glColor4f(1, 1, 0, 0.2);
        glBegin(GL_LINE_STRIP);
        {
            glVertex3f(0.8, 0.1, 0);
            glVertex3f(0.9, 0.2, 0);
            glVertex3f(0.9, 0.8, 0);
            glVertex3f(0.8, 0.9, 0);
        }
        glEnd();
        glBegin(GL_LINE_STRIP);
        {
            glVertex3f(0.2, 0.9, 0);
            glVertex3f(0.1, 0.8, 0);
            glVertex3f(0.1, 0.2, 0);
            glVertex3f(0.2, 0.1, 0);
        }
        glEnd();
        glLineWidth(1);
        glLineStipple(1, 0xFF55);
        glColor4f(1, 1, 1, 0.6);
        glEnable(GL_LINE_STIPPLE);
        glBegin(GL_LINE_STRIP);
        {
            glVertex3f(0.8, 0.1, 0);
            glVertex3f(0.9, 0.2, 0);
            glVertex3f(0.9, 0.8, 0);
            glVertex3f(0.8, 0.9, 0);
        }
        glEnd();
        glBegin(GL_LINE_STRIP);
        {
            glVertex3f(0.2, 0.9, 0);
            glVertex3f(0.1, 0.8, 0);
            glVertex3f(0.1, 0.2, 0);
            glVertex3f(0.2, 0.1, 0);
        }
        glEnd();
        /*
        glBegin(GL_LINE_STRIP);
        {
            glVertex3f(0.2, 0.1, 0);
            glVertex3f(0.8, 0.1, 0);
            glVertex3f(0.9, 0.2, 0);
            glVertex3f(0.9, 0.8, 0);
            glVertex3f(0.8, 0.9, 0);
            glVertex3f(0.2, 0.9, 0);
            glVertex3f(0.1, 0.8, 0);
            glVertex3f(0.1, 0.2, 0);
            glVertex3f(0.2, 0.1, 0);
        }
        glEnd();
         */
        glDisable(GL_LINE_STIPPLE);
    } // Border

    float f = 1.0f / (250 * 0.017453f);

    // Tower angle indicator
    if (true) {
        glLineWidth(3);

        // Tower left/right indication bar.
        glBegin(GL_LINES);
        {
            glColor4f(0, 0.2, 0.7, 0.4);
            glVertex3f(0.5, 0.1, 0);
            glVertex3f(0.5 + twr[1] * f, 0.1, 0);

            glVertex3f(0.5 + twr[1] * f, 0.08, 0);
            glVertex3f(0.5 + twr[1] * f, 0.12, 0);

            glVertex3f(0.5, 0.9, 0);
            glVertex3f(0.5 + twr[1] * f, 0.9, 0);

            glVertex3f(0.5 + twr[1] * f, 0.88, 0);
            glVertex3f(0.5 + twr[1] * f, 0.92, 0);
        }
        glEnd();

        // Tower up/down indication bar.
        glBegin(GL_LINES);
        {
            glColor4f(0, 0.2, 0.7, 0.4);
            glVertex3f(0.1, 0.5, 0);
            glVertex3f(0.1, 0.5 + twr[0] * f, 0);

            glVertex3f(0.08, 0.5 + twr[0] * f, 0);
            glVertex3f(0.12, 0.5 + twr[0] * f, 0);

            glVertex3f(0.9, 0.5, 0);
            glVertex3f(0.9, 0.5 + twr[0] * f, 0);

            glVertex3f(0.88, 0.5 + twr[0] * f, 0);
            glVertex3f(0.92, 0.5 + twr[0] * f, 0);
        }
        glEnd();

    } // Tower angle indicator

    // Message display
    if (true) {
        glPushMatrix();
        {
            std::stringstream s;
            glColor4f(0.99, 0.99, 0.19, 1);
            glTranslatef(0, 1, 0);
            glScalef(1.0f / 60.0f, 1.0f / 20.0f, 1.0f);
            glColor4f(0.09, 0.99, 0.09, 1);
            //glRotatef(1, 0,0,1);
            glTranslatef(0, -0, 0);
            s << mMessage;
            glprintf(s.str().c_str());
            //glprintf("TEST TEST TEST ... TEST TEST TEST\ntest test test");
        }
        glPopMatrix();
    } // Message display
}

// -----------------------------------------------

rNavcom::rNavcom(cObject* obj) {
    object = obj;
    role = "NAVCOM";

    vector_zero(pos);
    quat_zero(ori);

    std::vector<float> p;
    p.reserve(3);
    p.resize(3);

    vector_set(p, -20.0, 0, -50.0);
    mPOIs.push_back(p);

    vector_set(p, 0.0, 0, 0.0);
    mPOIs.push_back(p);

    vector_set(p, 0.0, 0, 90.0);
    mPOIs.push_back(p);

    vector_set(p, 90.0, 0, 90.0);
    mPOIs.push_back(p);

    vector_set(p, 90.0, 0, 0.0);
    mPOIs.push_back(p);

    vector_set(p, 30.0, 0, 0.0);
    mPOIs.push_back(p);

    //std::cout << "POIs: " << mPOIs[0][0] << " " << mPOIs[1][0] << " " << mPOIs[2][0] << std::endl;

    mWaypoint = 0;
    mCyclic = false;
    mRoute.push_back(0);
    mRoute.push_back(1);
    mRoute.push_back(2);
    mRoute.push_back(3);
    mRoute.push_back(4);
    mRoute.push_back(5);
}

void rNavcom::drawPOI(float x, float y, float s) {
    glBegin(GL_LINE_STRIP);
    {
        glVertex3f(x, y - s, 0);
        glVertex3f(x + s, y, 0);
        glVertex3f(x, y + s, 0);
        glVertex3f(x - s, y, 0);
        glVertex3f(x, y - s, 0);
    }
    glEnd();
}

void rNavcom::animate(float spf) {
}

void rNavcom::drawHUD() {
    if (!active) return;

    glBegin(GL_QUADS);
    glVertex3f(1, 1, 0);
    glVertex3f(0, 1, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(1, 0, 0);
    glEnd();
    float x = pos[0];
    float z = pos[2];
    float s = 0.005;

    glPushMatrix();
    {
        glScalef(0.5, 0.5, 1);
        glTranslatef(1, 1, 0);

        // Draw all points of interrest (debug).
        glColor4f(0.6, 0.6, 0.6, 0.7);
        loopiv(mPOIs) {
            float u = s * (mPOIs[i][0] - x);
            float v = s * (mPOIs[i][2] - z);
            drawPOI(+u, -v, 0.04);
        }

        // Draw Route lines with gradient towards current waypoint.
        glBegin(GL_LINE_STRIP);
        loopiv(mRoute) {
            if (i != mWaypoint) glColor4f(0.6, 0.6, 0.6, 0.7);
            else glColor4f(1, 0, 0, 0.7);
            int j = mRoute[i];
            float u = s * (mPOIs[j][0] - x);
            float v = s * (mPOIs[j][2] - z);
            glVertex3f(+u, -v, 0);
        }
        glEnd();

        // Mark next waypoint and check if reached.
        glColor4f(1, 0, 0, 0.3);
        loopiv(mRoute) {
            if (i != mWaypoint)continue;
            int j = mRoute[i];
            float u = s * (mPOIs[j][0] - x);
            float v = s * (mPOIs[j][2] - z);
            drawPOI(+u, -v, 0.07);
            // Waypoint reached? Next waypoint.
            if ((u*u + v*v) < (s*5)*(s*5)) {
                mWaypoint++;
                if (mWaypoint == (int)mRoute.size()) {
                    if (mCyclic) mWaypoint = 0;
                    else  mWaypoint--;
                }
            }
        }

        // Draw arrow direction indicator
        {
            quat ori_;
            quat_cpy(ori_, ori);
            glRotatef(90, 1,0,0);
            SGL::glRotateq(ori_);
            glRotatef(-90, 1,0,0);
            glColor4f(0.9, 0.9, 0.9, 0.9);
            glBegin(GL_LINE_STRIP);
            glVertex3f(-0.07, -0.2, 0);
            glVertex3f(+0.0, +0.0, 0);
            glVertex3f(+0.07, -0.2, 0);
            glEnd();
        }

    }
    glPopMatrix();

    // Overlay information text: time, date, direction, location.
    glPushMatrix();
    {
        std::stringstream s;
        glColor4f(0.99, 0.99, 0.19, 1);
        glTranslatef(0, 1, 0);
        glScalef(1.0f / 20.0f, 1.0f / 10.0f, 1.0f);
        glColor4f(0.09, 0.99, 0.09, 1);
        //glRotatef(1, 0,0,1);
        glTranslatef(0, -0, 0);
        cWorld* w = cWorld::instance;
        int x = int(pos[0]);
        int z = int(pos[2]);
        glprintf("Merc: %06i %06i", x, z);
        glTranslatef(0, -1, 0);
        glprintf("Alt: %3.2f", (pos[1]));
        glTranslatef(0, -1, 0);
        float angle = atan2(ori[1], ori[3]);
        glprintf("MKS: %2i", int(64 - angle * 64 / M_PI) % 64);
        glTranslatef(0, -6, 0);
        s << "Date: " << w->getTiming()->getDate();
        glprintf(s.str().c_str());
        glTranslatef(0, -1, 0);
        s.clear();
        s.str("");
        s << "Time: " << w->getTiming()->getTime();
        glprintf(s.str().c_str());
    }
    glPopMatrix();
}

