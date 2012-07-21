#include "rCamera.h"

#include "de/hackcraft/psi3d/GLS.h"

#include "de/hackcraft/opengl/GL.h"


std::string rCamera::cname = "CAMERA";
unsigned int rCamera::cid = 3311;

rCamera::rCamera(Entity * obj) : cameraswitch(0), camerashake(0), firstperson(true), camerastate(1) {
    object = obj;

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

    GL::glPushMatrix();
    {
        // Get inverse components of head pos and ori.
        GL::glLoadIdentity();
        GL::glTranslatef(pos0[0], pos0[1], pos0[2]);
        GLS::glRotateq(ori0);
        GL::glTranslatef(pos1[0], pos1[1], pos1[2]);
        GLS::glRotateq(ori1);
        // FIXME: Camera forward is inverted, therefore rotate.
        GL::glRotatef(180, 0, 1, 0);

        GLS::glGetTransposeInverseRotationMatrix(rot_inv);
        GLS::glGetInverseTranslationMatrix(pos_inv);
        // Compose Camera Matrix from inverse components
        GL::glLoadIdentity();
        GL::glMultMatrixf(rot_inv);
        GL::glRotatef(grand() * jerk, 1, 0, 0);
        GL::glRotatef(grand() * jerk, 0, 1, 0);
        GL::glRotatef(grand() * jerk, 0, 0, 1);
        GL::glMultMatrixf(pos_inv);
        GL::glTranslatef(grand() * shake, grand() * shake, grand() * shake);
        GL::glGetFloatv(GL_MODELVIEW_MATRIX, cam);
        //matrix_print(cam);
    }
    GL::glPopMatrix();

    int cs = abs(camerastate);
    if (cs == 1) { // 1st
        GL::glMultMatrixf(cam);
    } else if (cs == 2) { // 3rd
        GL::glTranslatef(0, 0, -5);
        GL::glRotatef(15, 1, 0, 0);
        GL::glMultMatrixf(cam);
    } else if (cs == 3) { // 3rd Far
        GL::glTranslatef(0, 0, -15);
        GL::glRotatef(15, 1, 0, 0);
        GL::glMultMatrixf(cam);
    } else if (cs == 4) { // Reverse 3rd Far
        GL::glTranslatef(0, 0, -15);
        GL::glRotatef(15, 1, 0, 0);
        GL::glRotatef(180, 0, 1, 0);
        GL::glMultMatrixf(cam);
    } else if (cs == 5) { // Map Near
        GL::glTranslatef(0, 0, -50);
        GL::glRotatef(90, 1, 0, 0);
        GL::glMultMatrixf(pos_inv);
    } else if (cs == 6) { // Map Far
        GL::glTranslatef(0, 0, -100);
        GL::glRotatef(90, 1, 0, 0);
        GL::glMultMatrixf(pos_inv);
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


float rCamera::grand() {
    float r = ((rand()%100 + rand()%100 + rand()%100 + rand()%100 + rand()%100) * 0.01f * 0.2f - 0.5f);
    return r;
}
