#include "rBillboard.h"

#include "de/hackcraft/psi3d/GLS.h"
#include "de/hackcraft/psi3d/GLF.h"

#include <GL/glew.h>

std::string rBillboard::cname = "NAMEABLE";
unsigned int rBillboard::cid = 8730;

rBillboard::rBillboard(Entity* obj) : name("Unnamed"), description("Undescribed"), designation(0) {
    object = obj;
    vector_zero(pos0);
    vector_zero(pos1);
    quat_zero(ori0);
    quat_zero(ori1);
    color[0] = color[1] = color[2] = color[3] = 1.0f;
    effect = true;
}

rBillboard::rBillboard(rBillboard * original) : name("Unnamed"), description("Undescribed"), designation(0) {
    if (original != NULL) {
        object = original->object;
        name = original->name;
        description = original->description;
        designation = original->designation;
    }
}

Component* rBillboard::clone() {
    return new rBillboard(this);
}

void rBillboard::drawEffect() {
    if (!effect) return;

    glPushMatrix();
    {
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        {
            GLS::glUseProgram_fgplaintexture();
            glColor4fv(color);

            float s = 0.75;
            glTranslatef(pos0[0], pos0[1], pos0[2]);
            GLS::glRotateq(ori0);
            glTranslatef(pos1[0], pos1[1], pos1[2]);
            bool billboard = true;
            if (!billboard) GLS::glRotateq(ori1);
            glScalef(s, s, s);
            if (billboard) {
                float n[16];
                GLS::glGetTransposeInverseRotationMatrix(n);
                glMultMatrixf(n);
            }
            int l = name.length();
            glTranslatef(-l * 0.5f, 0, 0);
            GLF::glprint(name.c_str());
        }
        glPopAttrib();
    }
    glPopMatrix();
}

