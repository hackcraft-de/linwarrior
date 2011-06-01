#include "cAlert.h"

#include "de/hackcraft/world/cWorld.h"

#include "de/hackcraft/comp/rNameable.h"

#include "de/hackcraft/psi3d/GLS.h"
#include "de/hackcraft/psi3d/Particle.h"

#include <ostream>
using std::cout;
using std::endl;


bool cAlert::sDrawzone = !true;

cAlert::cAlert(float* center, float* range, int shapetype, std::string msgtype, std::string msgtext, OID receiver, std::set<OID>* include, std::set<OID>* exclude, bool positive, bool posedge, bool once, OID fusedelay) {
    vector_cpy(shape.center, center);
    vector_cpy(shape.range, range);

    this->radius = 1 + fmax(fabs(shape.range[0]), fmax(fabs(shape.range[1]), fabs(shape.range[2])));
    if (this->radius >= 25) {
        // Make global "position".
        vector_set(pos0, float_NAN, float_NAN, float_NAN);
    } else {
        vector_cpy(pos0, center);
    }

    nameable = new rNameable(this);

    this->positive = positive;
    this->posedge = posedge;
    this->fusedelay = fusedelay;
    this->once = once;
    this->fired = false;
    this->msgtype = msgtype;
    this->msgtext = msgtext;
    this->receiver = receiver;
    shape.type = shapetype;
    inc_sense = *include;
    exc_sense = *exclude;
    fusedelay = 0;
}

float cAlert::constrain(float* worldpos, float radius, float* localpos, cObject* enactor) {
    //std::cout << "called" << std::endl;
    // Don't react on scanning and shooting.
    if (enactor == NULL) return 0;
    if (once && fired) return 0;
    if (!enactor->anyTags(&inc_sense) || enactor->anyTags(&exc_sense)) return 0;

    rShape* s = &shape;
    float depth = 0;
    float projection[3];
    switch (s->type) {
        case rShape::CYLINDER:
        {
            float base[] = {shape.center[0], shape.center[1] - shape.range[1], shape.center[2]};
            float radius = fmax(shape.range[0], shape.range[2]);
            float height = 2.0f * shape.range[1];
            depth = Particle::constraintParticleByCylinder(worldpos, base, radius, height, projection);
            break;
        }
        case rShape::BOX:
        {
            float min[3];
            float max[3];
            vector_set3i(min, shape.center[i] - shape.range[i]);
            vector_set3i(max, shape.center[i] + shape.range[i]);
            depth = Particle::constraintParticleByBox(worldpos, min, max, projection);
            break;
        }
        case rShape::SPHERE:
        {
            depth = Particle::constraintParticleBySphere(worldpos, shape.center, shape.range[0], projection);
            break;
        }
        case rShape::CONE:
        {
            float base[] = {shape.center[0], shape.center[1] - shape.range[1], shape.center[2]};
            float radius = fmax(shape.range[0], shape.range[2]);
            float height = 2.0f * shape.range[1];
            depth = Particle::constraintParticleByCone(worldpos, base, radius, height, projection);
            break;
        }
        default:
            break;
    }
    //std::cout << depth << std::endl;
    // Is that possible intruder yet unknown.
    if (intruders.find(enactor->oid) == intruders.end()) {
        // The intruder was outside and now there is an intrusion.
        if ((depth != 0) == positive) {
            intruders.insert(enactor->oid);
            if (posedge) {
                fired = true;
                //cout << "fusedelay=" << fusedelay << endl;
                cWorld::instance->sendMessage(fusedelay, oid, receiver, msgtype, msgtext, NULL);
            }
        }
    } else {
        // The intruder was inside and now the intruder left the zone.
        if ((depth == 0) == positive) {
            intruders.erase(enactor->oid);
            if (!posedge) {
                fired = true;
                cWorld::instance->sendMessage(fusedelay, oid, receiver, msgtype, msgtext, NULL);
            }
        }
    }
    //if (fired) cout << "FIRED" << endl;
    //return depth;
    return 0.0f;
}

void cAlert::drawEffect() {
    if (!sDrawzone) return;

    float edge = posedge ? 1.0f : 0.0f;
    float c_pos[] = {0, 1, edge, 1};
    float c_neg[] = {1, 0, edge, 1};
    float* c1 = positive ? c_pos : c_neg;
    float c2[] = {c1[0]*0.66f, c1[1]*0.66f, c1[2]*0.66f, c1[3]};
    float c3[] = {c1[0]*0.33f, c1[1]*0.33f, c1[2]*0.33f, c1[3]};

    rShape* s = &shape;
    float* center = s->center;
    float* range = s->range;

    glPushName((GLuint) (this->oid & 0xFFFF));

    if (nameable) {
        vector_cpy(nameable->pos0, center);
        nameable->color[0] = 0.99f;
        nameable->color[1] = 0.99f;
        nameable->color[2] = 0.20f;
        nameable->drawEffect();
    }

    //if (!sDrawzone) return;

    glPushMatrix();
    {
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        {
            GLS::glUseProgram_fgplaincolor();
            /*
            glDisable(GL_LIGHTING);
            glDisable(GL_FOG);
            glDisable(GL_CULL_FACE);
             */

            glLineWidth(3);
            glLineStipple(0.01, 0xFC);
            glEnable(GL_LINE_STIPPLE);
            switch (s->type) {
                case rShape::CYLINDER:
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
                    break;
                }
                case rShape::BOX:
                {
                    // Front-XY-Plane
                    glColor4fv(c1);
                    glBegin(GL_LINE_STRIP);
                    {
                        glVertex3f(center[0] - range[0], center[1] - range[1], center[2] - range[2]);
                        glVertex3f(center[0] + range[0], center[1] - range[1], center[2] - range[2]);
                        glVertex3f(center[0] + range[0], center[1] + range[1], center[2] - range[2]);
                        glVertex3f(center[0] - range[0], center[1] + range[1], center[2] - range[2]);
                        glVertex3f(center[0] - range[0], center[1] - range[1], center[2] - range[2]);
                    }
                    glEnd();
                    // Back-XY-Plane
                    glColor4fv(c1);
                    glBegin(GL_LINE_STRIP);
                    {
                        glVertex3f(center[0] - range[0], center[1] - range[1], center[2] + range[2]);
                        glVertex3f(center[0] + range[0], center[1] - range[1], center[2] + range[2]);
                        glVertex3f(center[0] + range[0], center[1] + range[1], center[2] + range[2]);
                        glVertex3f(center[0] - range[0], center[1] + range[1], center[2] + range[2]);
                        glVertex3f(center[0] - range[0], center[1] - range[1], center[2] + range[2]);
                    }
                    glEnd();
                    // Front-Back-Lines
                    glColor4fv(c2);
                    glBegin(GL_LINES);
                    {
                        glVertex3f(center[0] - range[0], center[1] - range[1], center[2] - range[2]);
                        glVertex3f(center[0] - range[0], center[1] - range[1], center[2] + range[2]);

                        glVertex3f(center[0] + range[0], center[1] - range[1], center[2] - range[2]);
                        glVertex3f(center[0] + range[0], center[1] - range[1], center[2] + range[2]);

                        glVertex3f(center[0] + range[0], center[1] + range[1], center[2] - range[2]);
                        glVertex3f(center[0] + range[0], center[1] + range[1], center[2] + range[2]);

                        glVertex3f(center[0] - range[0], center[1] + range[1], center[2] - range[2]);
                        glVertex3f(center[0] - range[0], center[1] + range[1], center[2] + range[2]);
                    }
                    glEnd();
                    break;
                }
                case rShape::SPHERE:
                {
                    float a = 0;
                    const int n = 12;
                    const float a_inc = M_PI * 2.0 / (double) n;
                    // XZ-Plane
                    glColor4fv(c2);
                    a = 0;
                    glBegin(GL_LINE_STRIP);

                    loopi(n + 1) {
                        float rx = sin(a) * range[0];
                        float rz = cos(a) * range[2];
                        a += a_inc;
                        glVertex3f(center[0] + rx, center[1], center[2] + rz);
                    }
                    glEnd();
                    // XY-Plane
                    glColor4fv(c3);
                    a = 0;
                    glBegin(GL_LINE_STRIP);

                    loopi(n + 1) {
                        float rx = sin(a) * range[0];
                        float ry = cos(a) * range[1];
                        a += a_inc;
                        glVertex3f(center[0] + rx, center[1] + ry, center[2]);
                    }
                    glEnd();
                    // YZ-Plane
                    glColor4fv(c1);
                    a = 0;
                    glBegin(GL_LINE_STRIP);

                    loopi(n + 1) {
                        float rz = sin(a) * range[2];
                        float ry = cos(a) * range[1];
                        a += a_inc;
                        glVertex3f(center[0], center[1] + ry, center[2] + rz);
                    }
                    glEnd();
                    break;
                }
                case rShape::CONE:
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
                    // Sides
                    glColor4fv(c2);
                    a = 0;
                    glBegin(GL_LINES);

                    loopi(n) {
                        float rx = sin(a) * range[0];
                        float rz = cos(a) * range[2];
                        a += a_inc;
                        glVertex3f(center[0] + rx, center[1] - range[1], center[2] + rz);
                        glVertex3f(center[0], center[1] + range[1], center[2]);
                    }
                    glEnd();
                    break;
                }
                default:
                {
                    break;
                }
            }

        }
        glPopAttrib();
    }
    glPopMatrix();

    glPopName();
}
