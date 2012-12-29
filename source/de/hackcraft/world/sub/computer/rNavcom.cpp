#include "rNavcom.h"

#include "de/hackcraft/psi3d/GLF.h"
#include "de/hackcraft/psi3d/GLS.h"

#include "de/hackcraft/world/World.h"

std::string rNavcom::cname = "NAVCOM";
unsigned int rNavcom::cid = 4660;

rNavcom::rNavcom(Entity* obj) {
    object = obj;

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

    //std::cout << "POIs: " << mPOIs[0][0] << " " << mPOIs[1][0] << " " << mPOIs[2][0] << "\n";

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
    GL::glBegin(GL_LINE_STRIP);
    {
        GL::glVertex3f(x, y - s, 0);
        GL::glVertex3f(x + s, y, 0);
        GL::glVertex3f(x, y + s, 0);
        GL::glVertex3f(x - s, y, 0);
        GL::glVertex3f(x, y - s, 0);
    }
    GL::glEnd();
}

void rNavcom::animate(float spf) {
}

void rNavcom::drawHUD() {
    if (!active) return;

    GL::glBegin(GL_QUADS);
    GL::glVertex3f(1, 1, 0);
    GL::glVertex3f(0, 1, 0);
    GL::glVertex3f(0, 0, 0);
    GL::glVertex3f(1, 0, 0);
    GL::glEnd();
    
    GL::glPushMatrix();
    {
        float w = 1;
        float h = 10;
        GL::glTranslatef(0, 1.0f + (1.0f / h), 0);
        GL::glScalef(1.0f / w, 1.0f / h, 1.0f);
        
        for (int i = 0; i < h; i++) {
            GL::glTranslatef(0, -1, 0);
            GL::glBegin(GL_LINES);
            GL::glColor4f(0.99, 0.99, 0.99, 0.02 + 0.1 * (h - i) / h);
            GL::glVertex3f(0,0,0);
            GL::glColor4f(0.99, 0.99, 0.99, 0.0);
            GL::glVertex3f(w,0,0);
            GL::glEnd();
        }
        
    }
    GL::glPopMatrix();
    
    float x = pos0[0];
    float z = pos0[2];
    float s = 0.005;

    GL::glPushMatrix();
    {
        GL::glScalef(0.5, 0.5, 1);
        GL::glTranslatef(1, 1, 0);

        // Draw all points of interrest (debug).
        GL::glColor4f(0.6, 0.6, 0.6, 0.7);

        for (std::vector<float>& poi: mPOIs) {
            float u = s * (poi[0] - x);
            float v = s * (poi[2] - z);
            drawPOI(+u, -v, 0.04);
        }

        // Draw Route lines with gradient towards current waypoint.
        GL::glBegin(GL_LINE_STRIP);

        int i = 0;
        for (int route: mRoute) {
            if (i++ != mWaypoint) GL::glColor4f(0.6, 0.6, 0.6, 0.7);
            else GL::glColor4f(1, 0, 0, 0.7);
            float u = s * (mPOIs[route][0] - x);
            float v = s * (mPOIs[route][2] - z);
            GL::glVertex3f(+u, -v, 0);
        }
        GL::glEnd();

        // Mark next waypoint and check if reached.
        GL::glColor4f(1, 0, 0, 0.3);

        i = 0;
        for (int route: mRoute) {
            if (i++ != mWaypoint)continue;
            float u = s * (mPOIs[route][0] - x);
            float v = s * (mPOIs[route][2] - z);
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
            GL::glRotatef(90, 1, 0, 0);
            GLS::glRotateq(ori_);
            GL::glRotatef(-90, 1, 0, 0);
            GL::glColor4f(0.9, 0.9, 0.9, 0.9);
            GL::glBegin(GL_LINE_STRIP);
            GL::glVertex3f(-0.07, -0.2, 0);
            GL::glVertex3f(+0.0, +0.0, 0);
            GL::glVertex3f(+0.07, -0.2, 0);
            GL::glEnd();
        }

    }
    GL::glPopMatrix();

    // Overlay information text: time, date, direction, location.
    GL::glPushMatrix();
    {
        World* world = World::getInstance();

        GL::glColor4f(0.99, 0.99, 0.19, 1);
        GL::glTranslatef(0, 1, 0);
        GL::glScalef(1.0f / 20.0f, 1.0f / 10.0f, 1.0f);
        GL::glColor4f(0.09, 0.99, 0.09, 1);

        GL::glTranslatef(0, -0, 0);
        int x = int(pos0[0]);
        int z = int(pos0[2]);
        GLF::glprintf("Merc: %06i %06i", x, z);

        GL::glTranslatef(0, -1, 0);
        GLF::glprintf("Alt: %3.2f", (pos0[1]));

        GL::glTranslatef(0, -1, 0);
        float angle = atan2(ori0[1], ori0[3]);
        GLF::glprintf("MKS: %2i", int(64 - angle * 64 / M_PI) % 64);
        
        GL::glTranslatef(0, -6, 0);
        GLF::glprintf("Date: %s", world->getTiming()->getDate().c_str());

        GL::glTranslatef(0, -1, 0);
        GLF::glprintf("Time: %s", world->getTiming()->getTime().c_str());
    }
    GL::glPopMatrix();
}

