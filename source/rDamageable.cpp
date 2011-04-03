#include "rDamageable.h"

#include "cObject.h"
#include <GL/glew.h>
#include "psi3d/snippetsgl.h"
#include "psi3d/instfont.h"
DEFINE_glprintf


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

rComponent* rDamageable::clone() {
    return new rDamageable(this);
}

bool rDamageable::damage(int hitzone, float damage, cObject* enactor) {
    hp[hitzone] -= damage;
    if (hp[hitzone] < 0.0f) hp[hitzone] = 0.0f;
    if (hp[BODY] <= 0.0f) {
        alife = false;
    }
    if (enactor != NULL && damage > 0.0001f) {
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

