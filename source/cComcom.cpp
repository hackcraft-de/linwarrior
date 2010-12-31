#include "cComcom.h"

#include "cWorld.h"

#include "psi3d/snippetsgl.h"
#include "psi3d/instfont.h"

#include <iostream>
using std::cout;
using std::endl;

DEFINE_Console_printf
DEFINE_glprintf

// -----------------------------------------------

cComcom::cComcom(cObject* device) {
    mDevice = device;
    int w = 20;
    int h = 10;
    mConsole = Console_new(w * h, w);
    assert(mConsole != NULL);
    //Console_printf(mConsole, "* Welcome to LinWarrior3D\n");
    Console_printf(mConsole, "ComCOM(1) - MENU\n\n");
    Console_printf(mConsole, " [1] Formation\n");
    Console_printf(mConsole, " [2] Engage\n");
    Console_printf(mConsole, " [3] Report\n");
    Console_printf(mConsole, " [4] Objectives\n");
    Console_printf(mConsole, "\n\n > ");
    mLastMessage = 0;
}

void cComcom::process(float spf) {
    cMessage* message = NULL;//cWorld::instance->recvMessage(0, mLastMessage);
    if (message != NULL) {
        mLastMessage = message->getTimestamp();
    }
}

void cComcom::drawHUD() {
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
        float w = mConsole->width;
        float h = mConsole->size / mConsole->width;
        glTranslatef(0, 1.0f + (1.0f / h), 0);
        glScalef(1.0f / w, 1.0f / h, 1.0f);
        Console_draw(mConsole);
    }
    glPopMatrix();
}

// -----------------------------------------------

cTarcom::cTarcom(cObject* device) {
    mDevice = device;
    near = NULL;
    selected = 0;
}

OID cTarcom::getSelected() {
    return selected;
}

void cTarcom::nextTarget() {
    bool found = false;
    OID last = 0;
    foreach(i, *near) {
        cObject* o = *i;
        if (last == selected) {
            selected = o->base->oid;
            found = true;
            break;
        }
        last = o->base->oid;
    }
    if (!found) {
        if (near->empty()) {
            selected = 0;
        } else {
            selected = near->front()->base->oid;
        }
    }
}

void cTarcom::prevTarget() {
    bool found = false;
    OID last = 0;
    foreach(i, *near) {
        cObject* o = *i;
        if (o->base->oid == selected) {
            selected = last;
            found = true;
            break;
        }
        last = o->base->oid;
    }
    if (!found) {
        if (near->empty()) {
            selected = 0;
        } else {
            selected = near->back()->base->oid;
        }
    }
}

void cTarcom::process(float spf) {
    delete near;
    near = cWorld::instance->filterByRange(mDevice, mDevice->traceable->pos.data(), 0, 150, -1, NULL);
}

void cTarcom::drawHUD() {
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
        float ori[4];
        quat_cpy(ori, mDevice->traceable->ori);
        quat_conj(ori);
        glRotatef(90, 1,0,0);
        SGL::glRotateq(ori);
        glRotatef(-90, 1,0,0);
        glPointSize(3);
        glBegin(GL_POINTS);
        {
            glColor4f(1, 1, 1, 1);
            glVertex3f(0, 0, 0);
        }
        glEnd();

        foreach(i, *near) {
            glBegin(GL_POINTS);
            {
                cObject* o = *i;
                if (o->hasRole(rRole::RED)) glColor4f(1, 0, 0, 1);
                else if (o->hasRole(rRole::GREEN)) glColor4f(0, 1, 0, 1);
                else if (o->hasRole(rRole::BLUE)) glColor4f(0, 0, 1, 1);
                else if (o->hasRole(rRole::YELLOW)) glColor4f(0, 1, 0, 1);
                else glColor4f(0.5, 0.5, 0.5, 1);
                if (o->base->oid == selected) glColor4f(1,0,1,1);
                float dx = o->traceable->pos[0] - mDevice->traceable->pos[0];
                float dz = o->traceable->pos[2] - mDevice->traceable->pos[2];
                float r = sqrt(dx * dx + dz * dz);
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

// -----------------------------------------------

cSyscom::cSyscom(cObject* device) {
    mDevice = device;
}

void cSyscom::process(float spf) {
}

void cSyscom::drawHUD() {
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
            if (i != 1) glColor4f(1 - mDevice->damageable->hp[left]*0.01, mDevice->damageable->hp[left]*0.01, 0.4, 0.2);
            cPrimitives::glLineSquare(0.1f);
            // Torsor&Head
            glTranslatef(1, 0, 0);
            glScalef(1, 1.5, 1);
            int body = rDamageable::BODY;
            if (i != 1) glColor4f(1 - mDevice->damageable->hp[body]*0.01, mDevice->damageable->hp[body]*0.01, 0.4, 0.2);
            cPrimitives::glLineSquare(0.1f);
            // Right Arm&Shoulder
            glTranslatef(1, 0, 0);
            glScalef(1, 1.0f / 1.5f, 1);
            int right = rDamageable::RIGHT;
            if (i != 1) glColor4f(1 - mDevice->damageable->hp[right]*0.01, mDevice->damageable->hp[right]*0.01, 0.4, 0.2);
            cPrimitives::glLineSquare(0.1f);
            // Legs
            glTranslatef(-1.6, -1, 0);
            glScalef(2.2, 1, 1);
            int legs = rDamageable::LEGS;
            if (i != 1) glColor4f(1 - mDevice->damageable->hp[legs]*0.01, mDevice->damageable->hp[legs]*0.01, 0.4, 0.2);
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
        glprintf("L %3.0f  T %3.0f  R %3.0f\n       B %3.0f", mDevice->damageable->hp[left], mDevice->damageable->hp[body], mDevice->damageable->hp[right], mDevice->damageable->hp[legs]);
    }
    glPopMatrix();
}

// -----------------------------------------------

cWepcom::cWepcom(cObject* device) {
    mDevice = device;
}

void cWepcom::process(float spf) {
}

void cWepcom::drawHUD() {
    cMech* mech = (cMech*) mDevice;
    float h = 1.0f / 7.0f * (1 + (mech->misc->weapons.size() + 1) / 2);
    glBegin(GL_QUADS);
    glVertex3f(1, h, 0);
    glVertex3f(0, h, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(1, 0, 0);
    glEnd();
    glPushMatrix();
    {
        glScalef(1.0 / 2.0, 1.0 / 7.0, 1.0);
        glTranslatef(0, 0.5, 0);

        loopi(mech->misc->weapons.size()) {
            glLineWidth(5);
            if (mech->misc->weapons[i]->ready()) glColor4f(0.4f, 1.0f, 0.4f, 0.2f);
            else glColor4f(0.8f, 0.0f, 0.0f, 0.2f);
            cPrimitives::glLineSquare(0.1f);
            glLineWidth(1);
            glColor4f(1.0f, 1.0f, 1.0f, 0.9f);
            cPrimitives::glLineSquare(0.1f);
            glPushMatrix();
            {
                mech->misc->weapons[i]->drawHUD();
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

cForcom::cForcom(cMech* device) {
    mDevice = device;
}

void cForcom::addMessage(std::string msg) {
    mMessage = msg;
}

void cForcom::process(float spf) {
}

void cForcom::drawHUD() {
    // Reticle in screen center.
    if (true && abs(mDevice->misc->camerastate) == 1) {
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

            float r = atan2(mDevice->traceable->ori[1], mDevice->traceable->ori[3]) / PI_OVER_180;
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
            glVertex3f(0.5 + mDevice->misc->twr[1] * f, 0.1, 0);

            glVertex3f(0.5 + mDevice->misc->twr[1] * f, 0.08, 0);
            glVertex3f(0.5 + mDevice->misc->twr[1] * f, 0.12, 0);

            glVertex3f(0.5, 0.9, 0);
            glVertex3f(0.5 + mDevice->misc->twr[1] * f, 0.9, 0);

            glVertex3f(0.5 + mDevice->misc->twr[1] * f, 0.88, 0);
            glVertex3f(0.5 + mDevice->misc->twr[1] * f, 0.92, 0);
        }
        glEnd();

        // Tower up/down indication bar.
        glBegin(GL_LINES);
        {
            glColor4f(0, 0.2, 0.7, 0.4);
            glVertex3f(0.1, 0.5, 0);
            glVertex3f(0.1, 0.5 + mDevice->misc->twr[0] * f, 0);

            glVertex3f(0.08, 0.5 + mDevice->misc->twr[0] * f, 0);
            glVertex3f(0.12, 0.5 + mDevice->misc->twr[0] * f, 0);

            glVertex3f(0.9, 0.5, 0);
            glVertex3f(0.9, 0.5 + mDevice->misc->twr[0] * f, 0);

            glVertex3f(0.88, 0.5 + mDevice->misc->twr[0] * f, 0);
            glVertex3f(0.92, 0.5 + mDevice->misc->twr[0] * f, 0);
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

cNavcom::cNavcom(cObject* device) {
    mDevice = device;

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

void cNavcom::drawPOI(float x, float y, float s) {
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

void cNavcom::process(float spf) {
}

void cNavcom::drawHUD() {
    glBegin(GL_QUADS);
    glVertex3f(1, 1, 0);
    glVertex3f(0, 1, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(1, 0, 0);
    glEnd();
    float x = mDevice->traceable->pos[0];
    float z = mDevice->traceable->pos[2];
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
            float ori[4];
            quat_cpy(ori, mDevice->traceable->ori);
            glRotatef(90, 1,0,0);
            SGL::glRotateq(ori);
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
        int x = int(mDevice->traceable->pos[0]);
        int z = int(mDevice->traceable->pos[2]);
        glprintf("Merc: %06i %06i", x, z);
        glTranslatef(0, -1, 0);
        glprintf("Alt: %3.2f", (mDevice->traceable->pos[1]));
        glTranslatef(0, -1, 0);
        float angle = atan2(mDevice->traceable->ori[1], mDevice->traceable->ori[3]);
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