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
    
    trigger = false;
}


void rWepcom::animate(float spf) {

    if (!active) return;
    
    if (trigger) {
        trigger = false;
        fire();
    }
}


void rWepcom::drawHUD() {
    
    if (!active) return;

    float h = 1.0f / 7.0f * (1 + (weapons.size() + 1) / 2);
    GL::glBegin(GL_QUADS);
    GL::glVertex3f(1, h, 0);
    GL::glVertex3f(0, h, 0);
    GL::glVertex3f(0, 0, 0);
    GL::glVertex3f(1, 0, 0);
    GL::glEnd();
    GL::glPushMatrix();
    {
        GL::glScalef(1.0f / 2.0f, 1.0f / 7.0f, 1.0f);
        GL::glTranslatef(0, 0.5, 0);

        loopi(weapons.size()) {
            GL::glLineWidth(5);
            if (weapons[i]->ready()) GL::glColor4f(0.4f, 1.0f, 0.4f, 0.2f);
            else GL::glColor4f(0.8f, 0.0f, 0.0f, 0.2f);
            Primitive::glLineSquare(0.1f);
            GL::glLineWidth(1);
            GL::glColor4f(1.0f, 1.0f, 1.0f, 0.9f);
            Primitive::glLineSquare(0.1f);
            GL::glPushMatrix();
            {
                weapons[i]->drawHUD();
            }
            GL::glPopMatrix();
            //GL::glTranslatef(0, 1, 0);
            if (i & 1) GL::glTranslatef(-1, 1, 0);
            else GL::glTranslatef(1, 0, 0);
        }
    }
    GL::glPopMatrix();
}


void rWepcom::addControlledWeapon(rWeapon* weapon) {
    weapons.push_back(weapon);
}


void rWepcom::fire() {

    if (singleWeapon) {
        if (weapons.empty()) return;
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

