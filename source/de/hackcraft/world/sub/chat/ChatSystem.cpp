#include "ChatSystem.h"

#include "de/hackcraft/log/Logger.h"

#include "de/hackcraft/world/World.h"

#include "de/hackcraft/world/sub/chat/rChatGroup.h"
#include "de/hackcraft/world/sub/chat/rChatMember.h"
#include "de/hackcraft/world/sub/chat/rChatMessage.h"

#include <cassert>

Logger* ChatSystem::logger = Logger::getLogger("de.hackcraft.world.sub.chat.ChatSystem");

ChatSystem* ChatSystem::instance = NULL;


ChatSystem* ChatSystem::getInstance() {
    assert(instance != NULL);
    return instance;
}


ChatSystem::ChatSystem() {
    instance = this;
}


ChatSystem::~ChatSystem() {
}


OID ChatSystem::getGroup(std::string name) {
    for (std::map<OID,rChatGroup*>::iterator i = groups.begin(); i != groups.end(); i++) {
        if ((*i).second->name.compare(name) == 0) {
            return (*i).second->id;
        }
    }
    rChatGroup* group = new rChatGroup();
    group->name = name;
    groups[group->id] = group;
    logger->debug() << "Chat group created: " << group->id << " as " << group->name << "\n";
    return group->id;
}


void ChatSystem::addToGroup(OID gid, Entity* member) {
    if (member == NULL) {
        logger->error() << "No object given while trying to add object to chat group.\n";
        return;
    }
    if (groups[gid] == NULL) {
        logger->error() << "No such chat group " << gid << " while trying to add object " << member->oid << " to group.\n";
        return;
    }
    rChatMember* chatMember = findChatMemberByEntity(member->oid);
    if (chatMember == NULL) {
        logger->error() << "No member for entity to add to chat group.\n";
        return;
    }
    
    logger->debug() << "Adding object " << chatMember->id << " to chat group " << gid << " " << groups[gid]->name << ".\n";
    groups[gid]->members.push_back(chatMember->id);
}


void ChatSystem::add(rChatMember* member) {
    members[member->id] = member;
}


rChatMember* ChatSystem::findChatMemberByEntity(OID entityID) {
    if (memberByEntity.find(entityID) != memberByEntity.end()) {
        return memberByEntity.at(entityID);
    }
    
    for(std::pair<OID,rChatMember*> p : members) {
        rChatMember* member = p.second;
        
        if (member->object->oid == entityID) {
            memberByEntity[entityID] = member;
            return member;
        }
    }
    
    return NULL;
}


void ChatSystem::sendMessage(OID delay, OID sender /* = 0 */, OID recvid /* = 0 */, std::string type, std::string text, void* blob) {
    //logger->trace() << "sendMessageT(dly=" << delay << ", sdr=" << sender << ", rid=" << recvid << ", txt=" << text << ")\n";
    Timing* timing = World::getInstance()->getTiming();
    timing->advanceDelta();
    rChatMessage* message = new rChatMessage(sender, recvid, timing->getTimekey() + delay, 0, type, text, blob);
    mMessages.push(message);
}


void ChatSystem::dispatchMessages() {
    // TODO: Remove return statement to enable dispatchMessages.
    //return;
    
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

            rChatGroup* group = groups[message->getReceiver()];
            if (group == NULL) continue;

            for(OID i: group->members) {
                if (members.find(i) != members.end()) {
                    rChatMember* member = members[i];
                    member->recvMessage(message);
                }
            }
        } else {
            // There are no more overdue messages to be dispatched.
            break;
        }
    }
}

