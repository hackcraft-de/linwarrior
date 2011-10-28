#include "World.h"

#include "Background.h"
#include "Mission.h"

#include "de/hackcraft/world/Entity.h"

#include <cassert>

#include <string>
using std::string;

#include <sstream>
using std::stringstream;

#include <iostream>
using std::cout;
using std::endl;


World* World::instance = NULL;

World::World() {
    World::instance = this;

    mMission = NULL;

    // Earth
    vector_set(mGravity, 0.0f, -9.80665f, 0.0f);
    // Mars
    //vector_set(mGravity, 0.0f, -3.69f, 0.0f);

    //mAirdensity = 0.1785; // Loose Helium Space
    //mAirdensity = 1.204f / 3.0f; // Terraformed Mars Air
    mAirdensity = 1.204f; // Earth Air
    //mAirdensity = 1000.0f; // Water
    mGndfriction = 0.13;

    mTiming.setFPS(30);
    mTiming.setDate(2010, 350);
    mTiming.setTime(01, 01, 01);

    mViewdistance = 150;

    //getSystemHour(mTime);
}


// Accessors

OID World::getOID() {
    return mTiming.getTimekey();
}

Timing* World::getTiming() {
    return &mTiming;
}

Entity* World::getObject(OID oid) {
    return mIndex[oid];
}

float* World::getGravity() {
    return mGravity;
}

float World::getGndfriction() {
    return mGndfriction;
}

float World::getAirdensity() {
    return mAirdensity;
}

void World::setViewdistance(float viewdistance) {
    mViewdistance = viewdistance;
}

float World::getViewdistance() {
    return mViewdistance;
}


// Grouping

OID World::getGroup(std::string name) {
    for (std::map<OID,Group*>::iterator i = mGroupIndex.begin(); i != mGroupIndex.end(); i++) {
        if ((*i).second->name.compare(name) == 0) {
            return (*i).second->gid;
        }
    }
    Group* group = new Group();
    group->gid = (OID) group;
    group->name = name;
    mGroupIndex[group->gid] = group;
    cout << "Group created: " << group->gid << " as " << group->name << "\n";
    return group->gid;
}

void World::addToGroup(OID gid, Entity* member) {
    if (member == NULL) {
        cout << "No object given while trying to add object to group.\n";
    }
    if (mGroupIndex[gid] == NULL) {
        cout << "No such group " << gid << " while trying to add object " << member->oid << " to group.\n";
        return;
    }
    cout << "Adding object " << member->oid << " to group " << gid << " " << mGroupIndex[gid]->name << ".\n";
    mGroupIndex[gid]->members.push_back(member->oid);
}

    
// Messaging

void World::sendMessage(OID delay, OID sender /* = 0 */, OID recvid /* = 0 */, string type, string text, void* blob) {
    //cout << "sendMessageT(dly=" << delay << ", sdr=" << sender << ", gid=" << groupid << ", txt=" << text << ")" << endl;
    mTiming.advanceDelta();
    Message* message = new Message(sender, recvid, mTiming.getTimekey() + delay, 0, type, text, blob);
    assert(message != NULL);
    mMessages.push(message);
}


// Spawning And Fragging

void World::spawnObject(Entity *object) {
    if (object == NULL) throw "Null object for spawnObject given.";

    OID serid = getOID();
    assert(serid != 0);
    cout << serid << " spawning at deltacycle " << mTiming.getDeltacycle() << endl;
    mIndex[serid] = object;
    object->oid = serid;
    object->seconds = 0;
    mTiming.advanceDelta();

    mObjects.push_back(object);
    object->spawn();
    // cout << serid << " spawn complete.\n";
}

void World::fragObject(Entity *object) {
    if (object->oid == 0) return;
    mObjects.remove(object);
    mCorpses.push_back(object);
    mIndex.erase(object->oid);
    object->frag();
    object->oid = 0;
}

void World::bagFragged() {
    // Finally delete objects which where fragged before.
    while (!mCorpses.empty()) {
        Entity* object = mCorpses.front();
        mCorpses.pop_front();
        delete object;
    };
}


// Simulation Step

void World::advanceTime(int deltamsec) {
    //cout << "advanceTime()\n";
    if (mMission) mMission->checkConditions();
    mTiming.advanceTime(deltamsec);
    //cout << getSerial() << ": " << mHour << " " << mMinute << " " << mSecond << " " << mDeltacycle << endl;
}

void World::clusterObjects() {
    try {
        int j = 0;
        mGeomap.clear();
        mUncluster.clear();

        for(Entity* o : mObjects) {
            float px = o->pos0[0];
            float pz = o->pos0[2];
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

void World::dispatchMessages() {
    OID now = getOID();
    while (!mMessages.empty()) {
        Message* message = mMessages.top();
        if (message->getTimestamp() <= now) {
            // Remove from to-send-list.
            mMessages.pop();
            // Keep record of sent messages.
            mDispatchedMessages.push_front(message);
            // Deliver info about new message to individual group members.
            if (message->getReceiver() == 0) continue;

            Group* group = mGroupIndex[message->getReceiver()];
            if (group == NULL) continue;

            for(OID i: group->members) {
                Entity* object = mIndex[i];
                if (object != NULL) {
                    object->message(message);
                }
            }
        } else {
            // There are no more overdue messages to be dispatched.
            break;
        }
    }
}

void World::animateObjects() {
    //cout << "animateObjects()\n";
    float spf = mTiming.getSPF();

    foreachNoInc(i, mObjects) {
        Entity* object = *i++;
        object->seconds += spf;
        object->animate(spf);
    }
}

void World::transformObjects() {
    //cout << "transformObjects()\n";

    foreachNoInc(i, mObjects) {
        Entity* object = *i++;
        glPushMatrix();
        object->transform();
        glPopMatrix();
    }
}

void World::drawBack() {
    //cout << "drawBack()\n";
    mBackground.drawBackground(mTiming.getTime24());
}

void World::drawSolid(Entity* camera, std::list<Entity*>* objects) {
    //cout << "drawSolid()\n";
    float* origin = camera->pos0;
    if (objects == NULL) objects = &mObjects;

    float maxrange = mViewdistance;
    float maxrange2 = maxrange*maxrange;

    foreachNoInc(i, *objects) {
        Entity* object = *i++;
        float x = object->pos0[0] - origin[0];
        float z = object->pos0[2] - origin[2];
        float d2 = x * x + z * z;
        if (d2 > maxrange2) continue;
        glPushMatrix();
        {
            object->drawSolid();
        }
        glPopMatrix();
    }
}

void World::drawEffect(Entity* camera, std::list<Entity*>* objects) {
    //cout << "drawEffect()\n";
    float* origin = camera->pos0;
    if (objects == NULL) objects = &mObjects;

    float maxrange2 = mViewdistance * mViewdistance;

    foreachNoInc(i, *objects) {
        Entity* object = *i++;
        float x = object->pos0[0] - origin[0];
        float z = object->pos0[2] - origin[2];
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

OID World::getGeokey(long x, long z) {
    OID xpart = ((OID) ((long(x))&0xFFFFFFFF)) >> 5;
    //cout << x << " ~ " << xpart << endl;
    OID zpart = ((OID) ((long(z))&0xFFFFFFFF)) >> 5;
    OID key = (xpart << 32) | zpart;
    //cout << key << endl;
    return key;
}

std::list<Entity*>* World::getGeoInterval(float* min2f, float* max2f, bool addunclustered) {
    std::list<Entity*>* found = new std::list<Entity*>();
    const long f = 1 << 5;

    long ax = ((long) min2f[0] / f) * f;
    long az = ((long) min2f[1] / f) * f;
    long bx = ((long) max2f[0] / f) * f;
    long bz = ((long) max2f[1] / f) * f;

    int n = 0;
    for (long j = az - f * 1; j <= bz + f * 1; j += f) {
        for (long i = ax - f * 1; i <= bx + f * 1; i += f) {
            std::list<Entity*>* l = &(mGeomap[getGeokey(i, j)]);
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

string World::getNames(std::list<Entity*>* objects) {
    if (objects == NULL) objects = &mObjects;
    stringstream s;
    s << "[";

    for(Entity* o: *objects) {
        s << " ";
        s << (o->name);
        s << "#";
        s << (o->oid);
        s << "(";

        for(OID tag: o->tags) {
            s << " " << tag << " ";
        }
        s << ")";
    }
    s << " ]";
    return s.str();
}

std::list<Entity*>* World::filterByTags(Entity* ex, std::set<OID>* rolemask, bool all, int maxamount, std::list<Entity*>* objects) {
    std::list<Entity*>* result = new std::list<Entity*>;
    assert(result != NULL);
    int amount = maxamount;
    if (objects == NULL) objects = &mObjects;

    for(Entity* object: *objects) {
        if (amount == 0) break;
        if (object->oid == 0) continue;
        // Filter Condition
        if (all) {
            if (!object->allTags(rolemask)) continue;
        } else {
            if (!object->anyTags(rolemask)) continue;
        }
        if (object == ex) continue;
        amount--;
        result->push_back(object);
    }
    return result;
}

std::list<Entity*>* World::filterByRange(Entity* ex, float* origin, float minrange, float maxrange, int maxamount, std::list<Entity*>* objects) {
    std::list<Entity*>* result = new std::list<Entity*>;
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

    for(Entity* object: *objects) {
        if (amount == 0) break;
        if (object == ex) continue;
        if (object->oid == 0) continue;
        // Filter Condition
        float diff[3];
        vector_sub(diff, origin, object->pos0);
        float d2 = vector_dot(diff, diff);
        //float d = vector_distance(origin, object->mPos);
        float r1 = fmax(0.0f, minrange - object->radius);
        float r2 = maxrange + object->radius;
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

std::list<Entity*>* World::filterByName(Entity* ex, char* name, int maxamount, std::list<Entity*>* objects) {
    std::list<Entity*>* result = new std::list<Entity*>;
    assert(result != NULL);
    int amount = maxamount;
    if (objects == NULL) objects = &mObjects;

    for(Entity* object: *objects) {
        if (amount == 0) break;
        if (object == ex) continue;
        if (object->oid == 0) continue;
        // Filter Condition
        if (strcmp(object->name.c_str(), name) != 0) continue;
        amount--;
        result->push_back(object);
    }
    return result;
}

std::list<Entity*>* World::filterByBeam(Entity* ex, float* pointa, float* pointb, float radius, int maxamount, std::list<Entity*>* objects) {
    float* x1 = pointa;
    float* x2 = pointb;
    std::list<Entity*>* result = new std::list<Entity*>;
    assert(result != NULL);
    int amount = maxamount;
    if (objects == NULL) objects = &mObjects;

    for(Entity* object: *objects) {
        if (amount == 0) break;
        if (object == ex) continue;
        if (object->oid == 0) continue;
        // Filter Condition

        float* x0 = object->pos0;
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
        float is = radius + object->radius;

        if (d2 > is * is) continue;

        amount--;
        result->push_back(object);
    }
    return result;
}

float World::constrainParticle(Entity* ex, float* worldpos, float radius) {
    float depth = 0;
    float maxrange = 25;
    bool groundplane = !true;
    if (groundplane)
        if (worldpos[1] - radius < 0.0f) {
            worldpos[1] = 0.0f + radius;
            depth += -(worldpos[1] - radius) + 0.000001f;
        }
    std::list<Entity*>* range = filterByRange(ex, worldpos, 0.0f, maxrange, -1, NULL);
    if (!range->empty()) {

        for(Entity* object: *range) {
            depth += object->constrain(worldpos, radius, NULL, ex);
        }
    }
    delete range;
    return depth;
}

