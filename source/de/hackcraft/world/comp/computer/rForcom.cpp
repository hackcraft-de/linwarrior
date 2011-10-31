#include "rForcom.h"

#include "de/hackcraft/psi3d/GLS.h"
#include "de/hackcraft/psi3d/GLF.h"

#include "de/hackcraft/world/Message.h"

std::string rForcom::cname = "FORCOM";
unsigned int rForcom::cid = 1032;

rForcom::rForcom(Entity* obj) {
    object = obj;
    
    quat_zero(ori);
    vector_zero(twr);
}

void rForcom::message(Message* message) {
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
            glColor4f(0.99, 0.99, 0.19, 1);
            glTranslatef(0, 1, 0);
            glScalef(1.0f / 60.0f, 1.0f / 20.0f, 1.0f);
            glColor4f(0.09, 0.99, 0.09, 1);
            glTranslatef(0, -0, 0);
            GLF::glprint(mMessage.c_str());
            //GLF::glprintf("TEST TEST TEST ... TEST TEST TEST\ntest test test");
        }
        glPopMatrix();
    } // Message display
}

