#include "rCollider.h"

#include "psi3d/macros.h"
#include "GL/glew.h"
#include "psi3d/snippetsgl.h"
#include "cParticle.h"

rCollider::rCollider(cObject * obj) {
    object = obj;
    role = "COLLIDER";

    quat_zero(ori0);
    vector_zero(pos0);
    radius = 0.1f;
    ratio = 0.0f;
    height = 0.1f;
}

float rCollider::constrainParticle(float* worldpos, float radius, float* localpos, cObject* enactor) {
    float localpos_[3];
    { // Transform to local.
        quat ori_inv;
        quat_cpy(ori_inv, this->ori0);
        quat_conj(ori_inv);

        vector_cpy(localpos_, worldpos);
        vector_sub(localpos_, localpos_, this->pos0);
        quat_apply(localpos_, ori_inv, localpos_);
    }

    float base[3] = {0, -0.0 - radius, 0};
    float radius_ = this->radius + radius;
    float height = this->height + 2 * radius;
    float localprj[3] = {0, 0, 0};

    float depth = 0;
    depth = cParticle::constraintParticleByCylinder(localpos_, base, radius_, height, localprj);
    //depth = cParticle::constraintParticleByCone(localpos_, base, radius_, height, localprj);

    if (depth <= 0) return 0;

    if (localpos != NULL) vector_cpy(localpos, localprj);

    { // Transform to global.
        quat_apply(worldpos, this->ori0, localprj);
        vector_add(worldpos, worldpos, this->pos0);
    }

    return depth;
}

void rCollider::animate(float spf) {
    height = (ratio > 0) ? radius * ratio : height;
}

void rCollider::drawEffect() {
    return; // !!

    glPushMatrix();
    {
        glTranslatef(pos0[0], pos0[1], pos0[2]);
        SGL::glRotateq(ori0);

        glPushAttrib(GL_ALL_ATTRIB_BITS);
        {
            SGL::glUseProgram_fgplaincolor();

            cPrimitives::glAxis(1.0f);

            float c1[] = {1.0f, 0.0f, 0.0f, 1.0f};
            float c2[] = {0.5f, 0.0f, 0.0f, 1.0f};
            vec3 range = {radius, height * 0.5f, radius};
            vec3 center = {0, height * 0.5f, 0};
            // Draw cylinder.
            {
                float a = 0;
                const int n = 12;
                const float a_inc = M_PI * 2.0 / (double) n;
                // XZ-Bottom-Plane
                glColor4fv(c1);
                a = 0;
                glBegin(GL_LINE_STRIP);

                loopi(n + 1) {
                    float rx = sin(a) * range[0];
                    float rz = cos(a) * range[2];
                    a += a_inc;
                    glVertex3f(center[0] + rx, center[1] - range[1], center[2] + rz);
                }
                glEnd();
                // XZ-Top-Plane
                glColor4fv(c1);
                a = 0;
                glBegin(GL_LINE_STRIP);

                loopi(n + 1) {
                    float rx = sin(a) * range[0];
                    float rz = cos(a) * range[2];
                    a += a_inc;
                    glVertex3f(center[0] + rx, center[1] + range[1], center[2] + rz);
                }
                glEnd();
                // Sides
                glColor4fv(c2);
                a = 0;
                glBegin(GL_LINES);

                loopi(n) {
                    float rx = sin(a) * range[0];
                    float rz = cos(a) * range[2];
                    a += a_inc;
                    glVertex3f(center[0] + rx, center[1] - range[1], center[2] + rz);
                    glVertex3f(center[0] + rx, center[1] + range[1], center[2] + rz);
                }
                glEnd();
                //break;
            }
        }
        glPopAttrib();
    }
    glPopMatrix();
}

