#include "rWepcom.h"

#include "de/hackcraft/psi3d/Primitive.h"

#include "de/hackcraft/world/sub/weapon/rWeapon.h"

std::string rWepcom::cname = "WEPCOM";
unsigned int rWepcom::cid = 4280;

rWepcom::rWepcom(Entity* obj) {
    object = obj;
    
    currentWeapon = 0;
    cycleWeapon = true;
    singleWeapon = true;
}

void rWepcom::animate(float spf) {
}

void rWepcom::drawHUD() {
    if (!active) return;

    float h = 1.0f / 7.0f * (1 + (weapons.size() + 1) / 2);
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

        loopi(weapons.size()) {
            glLineWidth(5);
            if (weapons[i]->ready()) glColor4f(0.4f, 1.0f, 0.4f, 0.2f);
            else glColor4f(0.8f, 0.0f, 0.0f, 0.2f);
            Primitive::glLineSquare(0.1f);
            glLineWidth(1);
            glColor4f(1.0f, 1.0f, 1.0f, 0.9f);
            Primitive::glLineSquare(0.1f);
            glPushMatrix();
            {
                weapons[i]->drawHUD();
            }
            glPopMatrix();
            //glTranslatef(0, 1, 0);
            if (i & 1) glTranslatef(-1, 1, 0);
            else glTranslatef(1, 0, 0);
        }
    }
    glPopMatrix();
}


void rWepcom::addControlledWeapon(rWeapon* weapon) {
    weapons.push_back(weapon);
}


void rWepcom::fire() {

    if (singleWeapon) {
        if (weapons.size() == 0) return;
        currentWeapon %= weapons.size();
        weapons[currentWeapon]->trigger = true;
        currentWeapon = cycleWeapon ? (currentWeapon + 1) : currentWeapon;
        currentWeapon %= weapons.size();
    } else {
        int n = weapons.size();
        for (int i = 0; i < n; i++) {
                weapons[i]->trigger = true;
        }
    }
}

