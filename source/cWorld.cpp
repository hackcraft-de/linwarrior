// cWorld.cpp

#include "cWorld.h"

#include "cBackground.h"
#include "cObject.h"
#include "cMission.h"

#include <cassert>

#include <string>
using std::string;

#include <sstream>
using std::stringstream;

#include <iostream>
using std::cout;
using std::endl;


cWorld* cWorld::instance = NULL;

cWorld::cWorld() {
    cWorld::instance = this;

    mMission = NULL;

    mGravity.reserve(3);
    mGravity.resize(3);
    mGravity[0] = 0;
    mGravity[1] = -9.80665f; // Earth
    //mGravity[1] = -3.69f; // Mars
    mGravity[2] = 0;

    //mAirdensity = 0.1785; // Loose Helium Space
    //mAirdensity = 1.204f / 3.0f; // Terraformed Mars Air
    mAirdensity = 1.204f; // Earth Air
    //mAirdensity = 1000.0f; // Water
    mGndfriction = 0.13;

    mTiming.setFPS(30);
    mTiming.setDate(2010, 350);
    mTiming.setTime(01, 01, 01);

    mViewdistance = 190;

    //getSystemHour(mTime);
}


// Accessors

OID cWorld::getOID() {
    return mTiming.getTimekey();
}

cTiming* cWorld::getTiming() {
    return &mTiming;
}

cObject* cWorld::getObject(OID oid) {
    return mIndex[oid];
}

std::vector<float>* cWorld::getGravity() {
    return &mGravity;
}

float cWorld::getGndfriction() {
    return mGndfriction;
}

float cWorld::getAirdensity() {
    return mAirdensity;
}

void cWorld::setViewdistance(float viewdistance) {
    mViewdistance = viewdistance;
}

float cWorld::getViewdistance() {
    return mViewdistance;
}


// Messaging

void cWorld::sendMessage(OID delay, OID sender /* = 0 */, OID recvid /* = 0 */, string type, string text, void* blob) {
    //cout << "sendMessageT(dly=" << delay << ", sdr=" << sender << ", gid=" << groupid << ", txt=" << text << ")" << endl;
    mTiming.advanceDelta();
    cMessage* message = new cMessage(sender, recvid, mTiming.getTimekey() + delay, 0, type, text, blob);
    assert(message != NULL);
    mMessages.push(message);
}


// Spawning And Fragging

void cWorld::spawnObject(cObject *object) {
    if (object == NULL) throw "Null object for spawnObject given.";

    OID serid = getOID();
    assert(serid != 0);
    cout << serid << " spawning at deltacycle " << mTiming.getDeltacycle() << endl;
    mIndex[serid] = object;
    object->base->oid = serid;
    mTiming.advanceDelta();
    
    mObjects.push_back(object);
    object->onSpawn();
    // cout << serid << " spawn complete.\n";
}

void cWorld::fragObject(cObject *object) {
    if (object->hasRole(rRole::FRAGGED)) return;
    object->addRole(rRole::FRAGGED);
    mObjects.remove(object);
    mCorpses.push_back(object);
    mIndex.erase(object->base->oid);
    object->onFrag();
}

void cWorld::bagFragged() {
    // Finally delete objects which where fragged before.
    while (!mCorpses.empty()) {
        cObject* object = mCorpses.front();
        mCorpses.pop_front();
        delete object;
    };
}


// Simulation Step


void cWorld::advanceTime(int deltamsec) {
    //cout << "advanceTime()\n";
    if (mMission) mMission->checkConditions();
    mTiming.advanceTime(deltamsec);
    //cout << getSerial() << ": " << mHour << " " << mMinute << " " << mSecond << " " << mDeltacycle << endl;
}


void cWorld::clusterObjects() {
    try {
        int j = 0;
        mGeomap.clear();
        mUncluster.clear();
        foreach(i, mObjects) {
            cObject* o = *i;
            float px = o->traceable->pos[0];
            float pz = o->traceable->pos[2];
            if (!finitef(px) || !finitef(pz)) {
                mUncluster.push_back(o);
            } else {
                mGeomap[getGeokey(px, pz)].push_back(o);
            }
            j++;
        }
    } catch (char* s) {
        cout << "Could not cluster world: " << s << endl;
    }
}


void cWorld::dispatchMessages() {
    OID now = getOID();
    while (!mMessages.empty()) {
        cMessage* message = mMessages.top();
        if (message->getTimestamp() <= now) {
            // Remove from to-send-list.
            mMessages.pop();
            // Keep record of sent messages.
            mDispatchedMessages.push_front(message);
            // Deliver info about new message to individual receiver(s).
            if (message->getReceiver() != 0) {
                // Message to single receiver.
                cObject* object = mIndex[message->getReceiver()];
                // Unable to deliver message to non-existent object?
                if (object == NULL) continue;
                // Deliver.
                object->onMessage(message);
                // Later this shall be handled by the object/role itself in onMessage(message).
                if (object->hasRole(rRole::GROUPING)) {
                    // Message to group receivers.
                    rGrouping* group = object->grouping;
                    // Unable to deliver message to non-existent group?
                    if (group == NULL) continue;
                    // Inform each group member of the new message.
                    foreach(i, group->mMembers) {
                        OID oid = *i;
                        this->sendMessage(0, message->getSender(), oid, message->getType(), message->getText(), message->getBlob());
                    }
                }
            }
        } else {
            // There are no more overdue messages to be dispatched.
            break;
        }
    }
}

void cWorld::animateObjects() {
    //cout << "animateObjects()\n";
    float spf = mTiming.getSPF();

    foreachNoInc(i, mObjects) {
        cObject* object = *i++;
        object->animate(spf);
    }
}

void cWorld::transformObjects() {
    //cout << "transformObjects()\n";

    foreachNoInc(i, mObjects) {
        cObject* object = *i++;
        glPushMatrix();
        object->transform();
        glPopMatrix();
    }
}

void cWorld::drawBack() {
    //cout << "drawBack()\n";
    mBackground.drawBackground(mTiming.getTime24());
}

void cWorld::drawSolid(cObject* camera, std::list<cObject*>* objects) {
    //cout << "drawSolid()\n";
    float* origin = camera->traceable->pos.data();
    if (objects == NULL) objects = &mObjects;

    float maxrange = mViewdistance;
    float maxrange2 = maxrange*maxrange;

    foreachNoInc(i, *objects) {
        cObject* object = *i++;
        float x = object->traceable->pos[0] - origin[0];
        float z = object->traceable->pos[2] - origin[2];
        float d2 = x * x + z*z;
        if (d2 > maxrange2) continue;
        glPushMatrix();
        {
            object->drawSolid();
        }
        glPopMatrix();
    }
}

void cWorld::drawEffect(cObject* camera, std::list<cObject*>* objects) {
    //cout << "drawEffect()\n";
    float* origin = camera->traceable->pos.data();
    if (objects == NULL) objects = &mObjects;

    float maxrange2 = mViewdistance * mViewdistance;

    foreachNoInc(i, *objects) {
        cObject* object = *i++;
        float x = object->traceable->pos[0] - origin[0];
        float z = object->traceable->pos[2] - origin[2];
        float d2 = x * x + z*z;
        if (d2 > maxrange2) continue;
        glPushMatrix();
        {
            object->drawEffect();
        }
        glPopMatrix();
    }

    if (!true) {
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        {
            glDisable(GL_CULL_FACE);
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_TEXTURE_3D);
            //float d = 2000;
            //float min[] = {-d, 0, -d};
            //float max[] = {+d, 40, +d};
            glPushMatrix();
            {
                // Draw Geomap information.
            }
            glPopMatrix();
        }
        glPopAttrib();
    }
}


// World Scanning And Filtering


OID cWorld::getGeokey(long x, long z) {
    OID xpart = ((OID)((long(x))&0xFFFFFFFF))>>5;
    //cout << x << " ~ " << xpart << endl;
    OID zpart = ((OID)((long(z))&0xFFFFFFFF))>>5;
    OID key = (xpart << 32) | zpart;
    //cout << key << endl;
    return key;
}


std::list<cObject*>* cWorld::getGeoInterval(float* min2f, float* max2f, bool addunclustered) {
    std::list<cObject*>* found = new std::list<cObject*>();
    const long f = 1 << 5;

    long ax = ((long) min2f[0] / f) * f;
    long az = ((long) min2f[1] / f) * f;
    long bx = ((long) max2f[0] / f) * f;
    long bz = ((long) max2f[1] / f) * f;

    int n = 0;
    for (long j = az-f*1; j <= bz+f*1; j+=f) {
        for (long i = ax-f*1; i <= bx+f*1; i+=f) {
            std::list<cObject*>* l = &(mGeomap[getGeokey(i,j)]);
            if (l != NULL) {
                //cout << "found " << l->size() << endl;
                found->insert(found->begin(), l->begin(), l->end());
            }
            n++;
        }
    }
    //cout << min2f[0] << ":" << min2f[1] << " - " << max2f[0] << ":" << max2f[1] << " " << n <<  " FOUND " << found->size() << endl;
    //cout << ax << ":" << az << " - " << bx << ":" << bz << " " << n <<  " FOUND " << found->size() << endl;

    if (addunclustered && !mUncluster.empty()) {
        found->insert(found->begin(), mUncluster.begin(), mUncluster.end());
    }

    return found;
}


string cWorld::getNames(std::list<cObject*>* objects) {
    if (objects == NULL) objects = &mObjects;
    stringstream s;
    s << "[";

    foreach(i, *objects) {
        cObject* o = *i;
        s << " ";
        if (o->nameable) {
            s << (o->nameable->name);
        }
        s << "#";
        s << (o->base->oid);
        if (o->socialised) {
            s << "(";
            //s << (o->socialised->roles.to_ulong());
            foreach(j, o->roleset) {
                OID oid = *j;
                s << " " << oid << " ";
            }
            s << ")";
        }
    }
    s << " ]";
    return s.str();
}

std::list<cObject*>* cWorld::filterByRole(cObject* ex, std::set<OID>* rolemask, bool all, int maxamount, std::list<cObject*>* objects) {
    std::list<cObject*>* result = new std::list<cObject*>;
    assert(result != NULL);
    int amount = maxamount;
    if (objects == NULL) objects = &mObjects;

    foreach(i, *objects) {
        if (amount == 0) break;
        cObject *object = *i;
        if (object->hasRole(rRole::FRAGGED)) continue;
        // Filter Condition
        if (all) {
            if (! object->allRoles(rolemask) ) continue;
        } else {
            if (! object->anyRoles(rolemask) ) continue;
        }
        if (object == ex) continue;
        amount--;
        result->push_back(object);
    }
    return result;
}


std::list<cObject*>* cWorld::filterByRange(cObject* ex, float* origin, float minrange, float maxrange, int maxamount, std::list<cObject*>* objects) {
    std::list<cObject*>* result = new std::list<cObject*>;
    assert(result != NULL);
    int amount = maxamount;
    bool all = false;
    if (objects == NULL) {
        //objects = &mObjects;
        all = true;
        float maxrange_ = maxrange + 1;
        float min[] = {origin[0] - maxrange_, origin[2] - maxrange_};
        float max[] = {origin[0] + maxrange_, origin[2] + maxrange_};
        objects = getGeoInterval(min, max);
        //cout << "clustered:" << objects->size() << " vs " << mObjects.size() << endl;
    }

    foreach(i, *objects) {
        if (amount == 0) break;
        cObject *object = *i;
        if (object == ex) continue;
        if (object->hasRole(rRole::FRAGGED)) continue;
        // Filter Condition
        float diff[3];
        vector_sub(diff, origin, object->traceable->pos);
        float d2 = vector_dot(diff, diff);
        //float d = vector_distance(origin, object->mPos);
        float r1 = fmax(0.0f, minrange - object->traceable->radius);
        float r2 = maxrange + object->traceable->radius;
        if (d2 < r1 * r1 || d2 > r2 * r2) continue;
        amount--;
        result->push_back(object);
    }
    if (all) {
        // Add global objects to result.
        result->insert(result->end(), mUncluster.begin(), mUncluster.end());
        // Cluster-Result is always freshly allocated for us => delete.
        delete objects;
    }
    return result;
}

std::list<cObject*>* cWorld::filterByName(cObject* ex, char* name, int maxamount, std::list<cObject*>* objects) {
    std::list<cObject*>* result = new std::list<cObject*>;
    assert(result != NULL);
    int amount = maxamount;
    if (objects == NULL) objects = &mObjects;

    foreach(i, *objects) {
        if (amount == 0) break;
        cObject *object = *i;
        if (object == ex) continue;
        if (object->hasRole(rRole::FRAGGED)) continue;
        // Filter Condition
        if (object->nameable != NULL && strcmp(object->nameable->name.c_str(), name) != 0) continue;
        amount--;
        result->push_back(object);
    }
    return result;
}

std::list<cObject*>* cWorld::filterByBeam(cObject* ex, float* pointa, float* pointb, float radius, int maxamount, std::list<cObject*>* objects) {
    float* x1 = pointa;
    float* x2 = pointb;
    std::list<cObject*>* result = new std::list<cObject*>;
    assert(result != NULL);
    int amount = maxamount;
    if (objects == NULL) objects = &mObjects;

    foreach(i, *objects) {
        if (amount == 0) break;
        cObject *object = *i;
        if (object == ex) continue;
        if (object->hasRole(rRole::FRAGGED)) continue;
        // Filter Condition

        float* x0 = object->traceable->pos.data();
        float u[] = {x0[0] - x1[0], x0[1] - x1[1], x0[2] - x1[2]};
        float v[] = {x2[0] - x1[0], x2[1] - x1[1], x2[2] - x1[2]};
        float a = u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
        float b = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
        float delta = a / b;

        //cout << "delta: " << delta << "\n";

        if (delta < -0.01) continue;
        if (delta > +1.01) continue;

        float p[] = {
            x1[0] + delta * (x2[0] - x1[0]),
            x1[1] + delta * (x2[1] - x1[1]),
            x1[2] + delta * (x2[2] - x1[2])
        };

        float dv[] = {
            p[0] - x0[0],
            p[1] - x0[1],
            p[2] - x0[2]
        };

        float d2 = dv[0] * dv[0] + dv[1] * dv[1] + dv[2] * dv[2];
        float is = radius + object->traceable->radius;

        if (d2 > is * is) continue;

        amount--;
        result->push_back(object);
    }
    return result;
}


