#include "rTarget.h"

#include "de/hackcraft/psi3d/GLS.h"
#include "de/hackcraft/psi3d/GLF.h"
#include "de/hackcraft/psi3d/Primitive.h"

#include "de/hackcraft/world/Entity.h"

#include "de/hackcraft/world/sub/weapon/rTarcom.h"

#include <GL/glew.h>

#include <cassert>

std::string rTarget::cname = "DAMAGEABLE";
unsigned int rTarget::cid = 5704;

rTarget::rTarget(Entity* obj) {
    object = obj;
    assert(object != NULL);

    radius = 0.25;
    height = 1.80;
    alife = true;
    dead = !alife;
    loopi(MAX_PARTS) hp[i] = 100.0f;
    disturber = 0;
    disturbance = 0;
    vector_zero(pos0);
}

rTarget::rTarget(rTarget * original) {
    if (original == NULL) {
        throw "Target: Null instance for copy when calling copy constructor.";
    } else {
        object = original->object;
        radius = original->radius;
        height = original->height;
        alife = original->alife;
        dead = original->dead;
        loopi(MAX_PARTS) hp[i] = original->hp[i];
        disturber = original->disturber;
        disturbance = original->disturbance;
    }
}

Component* rTarget::clone() {
    return new rTarget(this);
}

bool rTarget::damage(float* localpos, float damage, Entity* enactor) {
    int hitzone = rTarget::BODY;
    if (localpos[1] < 0.66 * height && hp[rTarget::LEGS] > 0) hitzone = rTarget::LEGS;
    else if (localpos[0] < -0.33 * radius && hp[rTarget::LEFT] > 0) hitzone = rTarget::LEFT;
    else if (localpos[0] > +0.33 * radius && hp[rTarget::RIGHT] > 0) hitzone = rTarget::RIGHT;

    hp[hitzone] -= damage;
    if (hp[hitzone] < 0.0f) hp[hitzone] = 0.0f;
    if (hp[BODY] <= 0.0f) {
        alife = false;
        dead = !alife;
    }
    
    if (enactor != NULL && damage > 0.0001f) {
        
        // Setup an averaging memory slot for that enactor.
        if (damageImpulse.find(enactor->oid) == damageImpulse.end()) {
            damageAverage[enactor->oid] = 0;
            damageImpulse[enactor->oid] = 0;
        }
        
        // Sum the disturbance (damage) for the enactor over the time of one frame.
        damageImpulse[enactor->oid] += damage;
    }

    return alife;
}

void rTarget::animate(float spf) {

    // Average the disturbance (damage) for the enactor over time.
    float alpha = 0.01;

    OID maxDisturber = 0;
    float maxDisturbance = 0;
    
    // Loop through memory, average new damage in and find biggest disturbance.
    for (std::map<OID,float>::iterator i = damageAverage.begin(); i != damageAverage.end(); ++i) {
        OID enactor = (*i).first;
        float disturbanceValue = (*i).second;
        
        damageAverage[enactor] = (1-alpha) * damageAverage[enactor] + alpha * damageImpulse[enactor];
        damageImpulse[enactor] = 0;
        
        if (disturbanceValue > maxDisturbance) {
            maxDisturbance = disturbanceValue;
            maxDisturber = enactor;
        }
        //if (object->oid % 1000ULL == 104) {
                //std::cout << object->oid << " Disturber: " << enactor << " => " << disturbanceValue << "\n";
        //}
    }

    // If there already was a disturber then switch only if the difference is big enough.
    if ( disturber != maxDisturber && disturber != 0 && damageAverage.find(disturber) != damageAverage.end()) {
        if (maxDisturbance > damageAverage[disturber] + 0.0005) {
            disturber = maxDisturber;
            disturbance = maxDisturbance;
        }
    } else {
            disturber = maxDisturber;
            disturbance = maxDisturbance;
    }
    
    //if (disturber != 0 && disturbance > 0.001 && (object->oid % 1000ULL == 104)) {
        //std::cout << object->oid << " Misturber: " << disturber << " => " << disturbance << "\n";
    //}
}

void rTarget::drawHUD() {
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
            int left = rTarget::LEFT;
            if (i != 1) glColor4f(1 - hp[left]*0.01, hp[left]*0.01, 0.4, 0.2);
            Primitive::glLineSquare(0.1f);
            // Torsor&Head
            glTranslatef(1, 0, 0);
            glScalef(1, 1.5, 1);
            int body = rTarget::BODY;
            if (i != 1) glColor4f(1 - hp[body]*0.01, hp[body]*0.01, 0.4, 0.2);
            Primitive::glLineSquare(0.1f);
            // Right Arm&Shoulder
            glTranslatef(1, 0, 0);
            glScalef(1, 1.0f / 1.5f, 1);
            int right = rTarget::RIGHT;
            if (i != 1) glColor4f(1 - hp[right]*0.01, hp[right]*0.01, 0.4, 0.2);
            Primitive::glLineSquare(0.1f);
            // Legs
            glTranslatef(-1.6, -1, 0);
            glScalef(2.2, 1, 1);
            int legs = rTarget::LEGS;
            if (i != 1) glColor4f(1 - hp[legs]*0.01, hp[legs]*0.01, 0.4, 0.2);
            Primitive::glLineSquare(0.1f);
        }
        glPopMatrix();
    }

    glPushMatrix();
    {
        glColor4f(0.09, 0.99, 0.09, 1);
        glScalef(1.0f / 20.0f, 1.0f / 10.0f, 1.0f);
        glTranslatef(0, 1, 0);
        GLF::glprint("hackcraft.de");
        glTranslatef(0, 9, 0);
        int left = rTarget::LEFT;
        int body = rTarget::BODY;
        int right = rTarget::RIGHT;
        int legs = rTarget::LEGS;
        GLF::glprintf("L %3.0f  T %3.0f  R %3.0f\n       B %3.0f", hp[left], hp[body], hp[right], hp[legs]);
    }
    glPopMatrix();
}

bool rTarget::isEnemy(rTarcom* tarcom) {
    return tarcom->isEnemy(&tags);
}

bool rTarget::anyTags(std::set<OID>* tagset) {
    std::set<OID> result;
    std::set_intersection(tags.begin(), tags.end(), tagset->begin(), tagset->end(), std::inserter(result, result.begin()));
    return (!result.empty());
}

bool rTarget::allTags(std::set<OID>* tagset) {
    std::set<OID> result;
    std::set_intersection(tags.begin(), tags.end(), tagset->begin(), tagset->end(), std::inserter(result, result.begin()));
    return (result.size() == tagset->size());
}

bool rTarget::hasTag(OID tag) {
    return (tags.find(tag) != tags.end());
}

void rTarget::addTag(OID tag) {
    tags.insert(tag);
}

void rTarget::remTag(OID tag) {
    tags.erase(tag);
}

