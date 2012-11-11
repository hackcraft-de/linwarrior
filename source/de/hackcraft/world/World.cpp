#include "World.h"

#include "de/hackcraft/opengl/GL.h"

#include "de/hackcraft/world/Entity.h"

#include <cassert>

#include <sstream>
#include <string>


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

void World::sendMessage(OID delay, OID sender /* = 0 */, OID recvid /* = 0 */, std::string type, std::string text, void* blob) {
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
        GL::glPushMatrix();
        object->transform();
        GL::glPopMatrix();
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
        GL::glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
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
        GL::glPushMatrix();
        {
            object->drawSolid();
        }
        GL::glPopMatrix();
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
        GL::glPushMatrix();
        {
            object->drawEffect();
        }
        GL::glPopMatrix();
    }

    if (!true) {
        GL::glPushAttrib(GL_ALL_ATTRIB_BITS);
        {
            GL::glDisable(GL_CULL_FACE);
            GL::glDisable(GL_TEXTURE_2D);
            GL::glDisable(GL_TEXTURE_3D);
            //float d = 2000;
            //float min[] = {-d, 0, -d};
            //float max[] = {+d, 40, +d};
            GL::glPushMatrix();
            {
                // Draw Geomap information.
            }
            GL::glPopMatrix();
        }
        GL::glPopAttrib();
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


float World::constrainParticle(Entity* ex, float* worldpos, float radius) {
    float depth = 0;

    bool groundplane = !true;
    if (groundplane && (worldpos[1] - radius < 0.0f)) {
        worldpos[1] = 0.0f + radius;
        depth += -(worldpos[1] - radius) + 0.000001f;
    }
    
    for (Subsystem* sub : subsystems) {
        depth += sub->constrainParticle(ex, worldpos, radius);
    }
    
    return depth;
}

