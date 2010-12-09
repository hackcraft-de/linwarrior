// cWorld.cpp

#include "cWorld.h"

#include "cBackground.h"
#include "cObject.h"
#include "cMission.h"

// for FORMATBUFFER
#include "psi3d/instfont.h"

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

    mBackground = NULL;
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

    mFPS = 30.0f;
    mSPF = 1.0f / mFPS;

    mYear = 2009;
    mDay = 125;
    mHour = 01;
    mMinute = 01;
    mSecond = 01;
    mMSec = 0;
    mFrame = 0;
    mDeltacycle = 1;

    mViewdistance = 190;

    rGroup* defaultgroup = new rGroup();
    defaultgroup->mID = 0;
    mGroups[defaultgroup->mID] = defaultgroup;

    //getSystemHour(mTime);
}


// Querries

OID cWorld::getOID() {
    OID ddd = mDay;
    OID hh = mHour;
    OID mm = mMinute;
    OID ss = mSecond;
    OID ff = mFrame;
    OID serid = ((((ddd * 100 + hh) *100 + mm) *100 + ss) * 100 + ff) * 10000 + mDeltacycle;
    return serid;
}


// Messaging

void cWorld::sendMessage(OID delay, OID sender /* = 0 */, OID groupid /* = 0 */, const char* format, ...) {
    FORMATBUFFER
    sendMessageT(delay, sender, groupid, string(buffer), NULL);
}


void cWorld::sendMessageT(OID delay, OID sender /* = 0 */, OID groupid /* = 0 */, string text, void* blob) {
    //cout << "sendMessageT(dly=" << delay << ", sdr=" << sender << ", gid=" << groupid << ", txt=" << text << ")" << endl;
    cWorld::rMessage* message = new cWorld::rMessage;
    assert(message != NULL);
    message->mSender = sender;
    message->mReceiver = 0;
    message->mGroup = groupid;
    mDeltacycle++;
    message->mTimestamp = getOID() + delay;
    message->mBestbefore = 0;
    message->mText = text;
    message->mBlob = blob;

    // Is there any such group if no individual receiver specified.
    if (message->mReceiver == 0) {
        rGroup* g = mGroups[groupid];
        if (g == NULL) throw "no such group.";
    }
    mMessages.push(message);
}


// Spawning And Fragging

void cWorld::spawnObject(cObject *object) {
    if (object == NULL) throw "Null object for spawnObject given.";

    OID serid = getOID();

    cout << serid << " spawning at deltacycle " << mDeltacycle << endl;

    mIndex[serid] = object;

    object->base->oid = serid;
    mDeltacycle++;

    mObjects.push_back(object);
    object->onSpawn();
    // cout << serid << " spawn complete.\n";
}

void cWorld::fragObject(cObject *object) {
    if (object->hasRole(FRAGGED)) return;
    object->addRole(FRAGGED);
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
    //cout << "0\n";
    mSPF = float(deltamsec) / 1000.0f;
    mFPS = 1.0 / (mSPF + 0.000001f);
    //cout << "0_\n";

    mFrame++;
    mDeltacycle = 1;
    mMSec += deltamsec;
    if (mMSec >= 1000) {
        mFrame = 0;
        mSecond += mMSec / 1000;
        mMSec %= 1000;
        if (mSecond >= 60) {
            mMinute += mSecond / 60;
            mSecond %= 60;
            if (mMinute >= 60) {
                mHour += mMinute / 60;
                mMinute %= 60;
                if (mHour >= 24) {
                    mDay += mHour / 24;
                    mHour %= 24;
                    if (mDay >= 366) {
                        mYear += mDay / 366;
                        mDay %= 366;
                    }
                }
            }
        }
    }

    //cout << "1\n";
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
        rMessage* message = mMessages.top();
        if (message->mTimestamp <= now) {
            // Remove from to-send-list.
            mMessages.pop();
            // Keep record of sent messages.
            mDispatchedMessages.push_front(message);
            // Deliver info about new message to individual receiver(s).
            if (message->mReceiver != 0) {
                // Message to single receiver.
                cObject* object = mIndex[message->mReceiver];
                // Unable to deliver message to non-existent object?
                if (object == NULL) continue;
                // Deliver.
                object->onMessage(message);
            } else {
                // Message to group receivers.
                rGroup* group = mGroups[message->mGroup];
                // Unable to deliver message to non-existent group?
                if (group == NULL) continue;
                // Inform each group member of the new message.

                foreach(i, group->mMembers) {
                    OID oid = *i;
                    cObject* object = mIndex[oid];
                    // Unable to deliver message to non-existent object?
                    if (object == NULL) continue;
                    // Deliver.
                    object->onMessage(message);
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
    float spf = mSPF;

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
    float time24 = float(mHour) + float(mMinute) / 60.0f + float(mSecond) / (60.0f * 60.0f) + float(mMSec) / (60.0f * 60.0f * 1000.0f);
    if (mBackground) mBackground->drawBackground(time24);
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


std::list<cObject*>* cWorld::getGeoInterval(float* min2f, float* max2f) {
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
        if (object->hasRole(FRAGGED)) continue;
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
        if (object->hasRole(FRAGGED)) continue;
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
        if (object->hasRole(FRAGGED)) continue;
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
        if (object->hasRole(FRAGGED)) continue;
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


