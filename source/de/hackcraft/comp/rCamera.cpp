#include "rCamera.h"

#include <GL/glew.h>

#include "de/hackcraft/psi3d/snippetsgl.h"

#define grand() ((rand()%100 + rand()%100 + rand()%100 + rand()%100 + rand()%100) * 0.01f * 0.2f - 0.5f)

rCamera::rCamera(cObject * obj) : cameraswitch(0), camerashake(0), firstperson(true), camerastate(1) {
    object = obj;
    role = "CAMERA";

    quat_zero(ori1);
    vector_zero(pos1);
    quat_zero(ori0);
    vector_zero(pos0);
}

void rCamera::camera() {
    float cam[16];
    float rot_inv[16];
    float pos_inv[16];

    float intensity = 0.0 + pow(camerashake, 1.5f);
    float shake = 0.004f * intensity;
    float jerk = 0.95f * intensity;

    glPushMatrix();
    {
        // Get inverse components of head pos and ori.
        glLoadIdentity();
        glTranslatef(pos0[0], pos0[1], pos0[2]);
        SGL::glRotateq(ori0);
        glTranslatef(pos1[0], pos1[1], pos1[2]);
        SGL::glRotateq(ori1);
        // FIXME: Camera forward is inverted, therefore rotate.
        glRotatef(180, 0, 1, 0);

        SGL::glGetTransposeInverseRotationMatrix(rot_inv);
        SGL::glGetInverseTranslationMatrix(pos_inv);
        // Compose Camera Matrix from inverse components
        glLoadIdentity();
        glMultMatrixf(rot_inv);
        glRotatef(grand() * jerk, 1, 0, 0);
        glRotatef(grand() * jerk, 0, 1, 0);
        glRotatef(grand() * jerk, 0, 0, 1);
        glMultMatrixf(pos_inv);
        glTranslatef(grand() * shake, grand() * shake, grand() * shake);
        glGetFloatv(GL_MODELVIEW_MATRIX, cam);
        //matrix_print(cam);
    }
    glPopMatrix();

    int cs = abs(camerastate);
    if (cs == 1) { // 1st
        glMultMatrixf(cam);
    } else if (cs == 2) { // 3rd
        glTranslatef(0, 0, -5);
        glRotatef(15, 1, 0, 0);
        glMultMatrixf(cam);
    } else if (cs == 3) { // 3rd Far
        glTranslatef(0, 0, -15);
        glRotatef(15, 1, 0, 0);
        glMultMatrixf(cam);
    } else if (cs == 4) { // Reverse 3rd Far
        glTranslatef(0, 0, -15);
        glRotatef(15, 1, 0, 0);
        glRotatef(180, 0, 1, 0);
        glMultMatrixf(cam);
    } else if (cs == 5) { // Map Near
        glTranslatef(0, 0, -50);
        glRotatef(90, 1, 0, 0);
        glMultMatrixf(pos_inv);
    } else if (cs == 6) { // Map Far
        glTranslatef(0, 0, -100);
        glRotatef(90, 1, 0, 0);
        glMultMatrixf(pos_inv);
    } // if camerastate
}

void rCamera::animate(float spf) {
    const int MAX_CAMERAMODES = 6;
    if (cameraswitch) {
        // Only if Camera State is not transitional
        // then switch to next perspective on button press.
        if (camerastate > 0) {
            camerastate = (1 + (camerastate % MAX_CAMERAMODES));
            camerastate *= -1; // Set transitional.
        }
    } else {
        // Set Camera State as fixed.
        camerastate = abs(camerastate);
    }
    firstperson = (abs(camerastate) == 1);
}

