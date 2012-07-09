#include "World.h"

#include "de/hackcraft/world/Entity.h"

#include <cassert>

#include <sstream>
using std::stringstream;

#include <string>
using std::string;

Logger* World::logger = Logger::getLogger("de.hackcraft.world.World");

World* World::instance = NULL;

World::World() {
    World::instance = this;

    mTiming.setFPS(30);
    mTiming.setDate(2010, 350);
    mTiming.setTime(01, 01, 01);

    mViewdistance = 150;
    mVisobjects = NULL;
    mVisorigin[0] = mVisorigin[1] = mVisorigin[2] = 0;

    //getSystemHour(mTime);
}


// Accessors

World* World::getInstance() {
    return instance;
}

OID World::getOID() {
    return mTiming.getTimekey();
}

Timing* World::getTiming() {
    return &mTiming;
}

Entity* World::getObject(OID oid) {
    return mIndex[oid];
}

void World::setViewdistance(float viewdistance) {
    mViewdistance = viewdistance;
}

float World::getViewdistance() {
    return mViewdistance;
}


// Grouping

OID World::getGroup(std::string name) {
    for (std::map<OID,Group*>::iterator i = mGroupIndex.begin(); i != mGroupIndex.end(); ++i) {
        if ((*i).second->name.compare(name) == 0) {
            return (*i).second->gid;
        }
    }
    Group* group = new Group();
    group->gid = (OID) group;
    group->name = name;
    mGroupIndex[group->gid] = group;
    logger->info() << "Group created: " << group->gid << " as " << group->name << "\n";
    return group->gid;
}

void World::addToGroup(OID gid, Entity* member) {
    if (member == NULL) {
        logger->error() << "No object given while trying to add object to group.\n";
    }
    if (mGroupIndex[gid] == NULL) {
        logger->error() << "No such group " << gid << " while trying to add object " << member->oid << " to group.\n";
        return;
    }
    logger->debug() << "Adding object " << member->oid << " to group " << gid << " " << mGroupIndex[gid]->name << ".\n";
    mGroupIndex[gid]->members.push_back(member->oid);
}

    
// Messaging

void World::sendMessage(OID delay, OID sender /* = 0 */, OID recvid /* = 0 */, string type, string text, void* blob) {
    /*
    //logger->trace() << "sendMessageT(dly=" << delay << ", sdr=" << sender << ", gid=" << groupid << ", txt=" << text << ")" << "\n";
    mTiming.advanceDelta();
    Message* message = new Message(sender, recvid, mTiming.getTimekey() + delay, 0, type, text, blob);
    assert(message != NULL);
    mMessages.push(message);
    */
}


// Spawning And Fragging

void World::spawnObject(Entity *object) {
    if (object == NULL) throw "Null object for spawnObject given.";

    OID serid = getOID();
    assert(serid != 0);
    logger->debug() << serid << " spawning at deltacycle " << mTiming.getDeltacycle() << "\n";
    mIndex[serid] = object;
    object->oid = serid;
    object->seconds = 0;
    mTiming.advanceDelta();

    mObjects.push_back(object);
    object->spawn();
    // logger->debug() << serid << " spawn complete.\n";
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
    //logger->trace() << "advanceTime()\n";
    mTiming.advanceTime(deltamsec);
    
    for (Subsystem* sub : subsystems) {
        sub->advanceTime(deltamsec);
    }
    //logger->trace() << getSerial() << ": " << mHour << " " << mMinute << " " << mSecond << " " << mDeltacycle << "\n";
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
                mGeomap.put(px, pz, o);
            }
            j++;
        }
        
        for (Subsystem* sub : subsystems) {
            sub->clusterObjects();
        }
    } catch (char* s) {
        logger->error() << "Could not cluster world: " << s << "\n";
    }
}

void World::dispatchMessages() {
    /*
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
    */
    
    for (Subsystem* sub : subsystems) {
        sub->dispatchMessages();
    }
}

void World::animateObjects() {
    //logger->trace() << "animateObjects()\n";
    float spf = mTiming.getSPF();

    foreachNoInc(i, mObjects) {
        Entity* object = *i++;
        object->seconds += spf;
        object->animate(spf);
    }

    for (Subsystem* sub : subsystems) {
        sub->animateObjects();
    }
}

void World::transformObjects() {
    //logger->trace() << "transformObjects()\n";

    foreachNoInc(i, mObjects) {
        Entity* object = *i++;
        glPushMatrix();
        object->transform();
        glPopMatrix();
    }

    for (Subsystem* sub : subsystems) {
        sub->transformObjects();
    }
}

void World::setupView(float* pos, float* ori) {
    // Find objects in visible range.
    float* origin = pos;
    float maxrange = getViewdistance();
    float min[] = {origin[0] - maxrange, origin[2] - maxrange};
    float max[] = {origin[0] + maxrange, origin[2] + maxrange};
    
    if (mVisobjects != NULL) {
        delete mVisobjects;
    }
    
    mVisobjects = getGeoInterval(min, max, true);
    assert(mVisobjects != NULL);
    //logger->trace() << "vis:" << objects->size() << " vs " << mObjects.size() << "\n";
    
    mVisorigin[0] = pos[0];
    mVisorigin[1] = pos[1];
    mVisorigin[2] = pos[2];

    for (Subsystem* sub : subsystems) {
        sub->setupView(pos, ori);
    }
}

bool World::drawBack() {
    //logger->trace() << "drawBack()\n";
    bool any = false;
    for (Subsystem* sub : subsystems) {
        any |= sub->drawBack();
    }
    
    if (!any) {
        glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    }
    
    return true;
}

void World::drawSolid() {
    //logger->trace() << "drawSolid()\n";
    float* origin = mVisorigin;
    std::list<Entity*>* objects = (mVisobjects == NULL) ? &mObjects : mVisobjects;

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

    for (Subsystem* sub : subsystems) {
        sub->drawSolid();
    }
}

void World::drawEffect() {
    //logger->trace() << "drawEffect()\n";
    float* origin = mVisorigin;
    std::list<Entity*>* objects = (mVisobjects == NULL) ? &mObjects : mVisobjects;

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

    for (Subsystem* sub : subsystems) {
        sub->drawEffect();
    }
}


// World Scanning And Filtering

std::list<Entity*>* World::getGeoInterval(float* min2f, float* max2f, bool addunclustered) {
    std::list<Entity*>* found = mGeomap.getGeoInterval(min2f, max2f);
    //logger->trace() << min2f[0] << ":" << min2f[1] << " - " << max2f[0] << ":" << max2f[1] << " " << n <<  " FOUND " << found->size() << "\n";
    //logger->trace() << ax << ":" << az << " - " << bx << ":" << bz << " " << n <<  " FOUND " << found->size() << "\n";

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
        //logger->trace() << "clustered:" << objects->size() << " vs " << mObjects.size() << "\n";
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

        //logger->trace() << "delta: " << delta << "\n";

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
    if (groundplane && (worldpos[1] - radius < 0.0f)) {
        worldpos[1] = 0.0f + radius;
        depth += -(worldpos[1] - radius) + 0.000001f;
    }
    
    for (Subsystem* sub : subsystems) {
        depth += sub->constrainParticle(ex, worldpos, radius);
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

