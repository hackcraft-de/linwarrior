#include "rWepcom.h"

// FIXME: Wepcom rely on mech.
#include "de/hackcraft/world/object/cMech.h"

// FIXME? Wepcom rely on weapon.
#include "de/hackcraft/world/comp/weapon/rWeapon.h"

#include "de/hackcraft/psi3d/Primitive.h"


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
            Primitive::glLineSquare(0.1f);
            glLineWidth(1);
            glColor4f(1.0f, 1.0f, 1.0f, 0.9f);
            Primitive::glLineSquare(0.1f);
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

