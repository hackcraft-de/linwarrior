#include "rNavcom.h"

#include "de/hackcraft/psi3d/GLS.h"
#include "de/hackcraft/psi3d/GLF.h"

#include "de/hackcraft/world/cWorld.h"


rNavcom::rNavcom(cObject* obj) {
    object = obj;
    role = "NAVCOM";

    vector_zero(pos0);
    quat_zero(ori0);

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
    float x = pos0[0];
    float z = pos0[2];
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
            if ((u * u + v * v) < (s * 5)*(s * 5)) {
                mWaypoint++;
                if (mWaypoint == (int) mRoute.size()) {
                    if (mCyclic) mWaypoint = 0;
                    else mWaypoint--;
                }
            }
        }

        // Draw arrow direction indicator
        {
            quat ori_;
            quat_cpy(ori_, ori0);
            glRotatef(90, 1, 0, 0);
            GLS::glRotateq(ori_);
            glRotatef(-90, 1, 0, 0);
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
        cWorld* world = cWorld::instance;

        glColor4f(0.99, 0.99, 0.19, 1);
        glTranslatef(0, 1, 0);
        glScalef(1.0f / 20.0f, 1.0f / 10.0f, 1.0f);
        glColor4f(0.09, 0.99, 0.09, 1);

        glTranslatef(0, -0, 0);
        int x = int(pos0[0]);
        int z = int(pos0[2]);
        GLF::glprintf("Merc: %06i %06i", x, z);

        glTranslatef(0, -1, 0);
        GLF::glprintf("Alt: %3.2f", (pos0[1]));

        glTranslatef(0, -1, 0);
        float angle = atan2(ori0[1], ori0[3]);
        GLF::glprintf("MKS: %2i", int(64 - angle * 64 / M_PI) % 64);
        
        glTranslatef(0, -6, 0);
        GLF::glprintf("Date: %s", world->getTiming()->getDate().c_str());

        glTranslatef(0, -1, 0);
        GLF::glprintf("Time: %s", world->getTiming()->getTime().c_str());
    }
    glPopMatrix();
}

