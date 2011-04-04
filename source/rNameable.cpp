#include "rNameable.h"

#include <GL/glew.h>
#include "psi3d/snippetsgl.h"
#include "psi3d/instfont.h"

DEFINE_glprintf


rNameable::rNameable(cObject* obj) : name("Unnamed"), description("Undescribed"), designation(0) {
    role = "NAMEABLE";
    object = obj;
    vector_zero(pos0);
    vector_zero(pos1);
    quat_zero(ori0);
    quat_zero(ori1);
    color[0] = color[1] = color[2] = color[3] = 1.0f;
    effect = true;
}

rNameable::rNameable(rNameable * original) : name("Unnamed"), description("Undescribed"), designation(0) {
    role = "NAMEABLE";
    if (original != NULL) {
        object = original->object;
        name = original->name;
        description = original->description;
        designation = original->designation;
    }
}

rComponent* rNameable::clone() {
    return new rNameable(this);
}

void rNameable::drawEffect() {
    if (!effect) return;

    glPushMatrix();
    {
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        {
            SGL::glUseProgram_fgplaintexture();
            glColor4fv(color);

            float s = 0.75;
            //glTranslatef(pos[0], pos[1] + 0.5f*s, pos[2]);
            glTranslatef(pos0[0], pos0[1], pos0[2]);
            SGL::glRotateq(ori0);
            glTranslatef(pos1[0], pos1[1], pos1[2]);
            bool billboard = true;
            if (!billboard) SGL::glRotateq(ori1);
            glScalef(s, s, s);
            if (billboard) {
                float n[16];
                SGL::glGetTransposeInverseRotationMatrix(n);
                glMultMatrixf(n);
            }
            int l = name.length();
            glTranslatef(-l * 0.5f, 0, 0);
            glprintf(name.c_str());
        }
        glPopAttrib();
    }
    glPopMatrix();
}

