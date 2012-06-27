#include "ChatSystem.h"

#include "de/hackcraft/world/World.h"

#include <iostream>
#include <cassert>

ChatSystem* ChatSystem::instance = NULL;

ChatSystem::ChatSystem() {
    instance = this;
}

ChatSystem::~ChatSystem() {
}

ChatSystem* ChatSystem::getInstance() {
    assert(instance != NULL);
    return instance;
}

OID ChatSystem::getGroup(std::string name) {
    for (std::map<OID,rChatGroup*>::iterator i = mGroupIndex.begin(); i != mGroupIndex.end(); i++) {
        if ((*i).second->name.compare(name) == 0) {
            return (*i).second->gid;
        }
    }
    rChatGroup* group = new rChatGroup();
    group->gid = (OID) group;
    group->name = name;
    mGroupIndex[group->gid] = group;
    std::cout << "Group created: " << group->gid << " as " << group->name << "\n";
    return group->gid;
}

void ChatSystem::addToGroup(OID gid, Entity* member) {
    if (member == NULL) {
        std::cout << "No object given while trying to add object to group.\n";
    }
    if (mGroupIndex[gid] == NULL) {
        std::cout << "No such group " << gid << " while trying to add object " << member->oid << " to group.\n";
        return;
    }
    std::cout << "Adding object " << member->oid << " to group " << gid << " " << mGroupIndex[gid]->name << ".\n";
    mGroupIndex[gid]->members.push_back(member->oid);
}

void ChatSystem::sendMessage(OID delay, OID sender /* = 0 */, OID recvid /* = 0 */, std::string type, std::string text, void* blob) {
    //std::cout << "sendMessageT(dly=" << delay << ", sdr=" << sender << ", rid=" << recvid << ", txt=" << text << ")\n";
    Timing* timing = World::getInstance()->getTiming();
    timing->advanceDelta();
    rChatMessage* message = new rChatMessage(sender, recvid, timing->getTimekey() + delay, 0, type, text, blob);
    mMessages.push(message);
}

void ChatSystem::dispatchMessages() {
    // TODO: Remove return statement to enable dispatchMessages.
    return;
    
    OID now = World::getInstance()->getOID();
    while (!mMessages.empty()) {
        rChatMessage* message = mMessages.top();
        if (message->getTimestamp() <= now) {
            // Remove from to-send-list.
            mMessages.pop();
            // Keep record of sent messages.
            mDispatchedMessages.push_front(message);
            // Deliver info about new message to individual group members.
            if (message->getReceiver() == 0) continue;

            rChatGroup* group = mGroupIndex[message->getReceiver()];
            if (group == NULL) continue;

            for(OID i: group->members) {
                if (comcoms.find(i) != comcoms.end()) {
                    rChatMember* object = comcoms[i];
                    object->message(message);
                }
            }
        } else {
            // There are no more overdue messages to be dispatched.
            break;
        }
    }
}

