/* 
 * File:   cMessage.cpp
 * Author: benben
 * 
 * Created on 23. Dezember 2010, 15:41
 */

#include "Message.h"

Message::Message(OID sender, OID receiver, OID timestamp, OID bestbefore, std::string type, std::string text, void* blob) {
    mSender = sender;
    mReceiver = receiver;
    mTimestamp = timestamp;
    mBestbefore = bestbefore;
    mType = type;
    mText = text;
    mBlob = blob;
}

Message::Message(const Message& orig) {
    mSender = orig.mSender;
    mReceiver = orig.mReceiver;
    mTimestamp = orig.mTimestamp;
    mBestbefore = orig.mBestbefore;
    mType = orig.mType;
    mText = orig.mText;
    mBlob = orig.mBlob;
}

Message::~Message() {
    // Not our responsibility to delete anything:
    // delete getBlob() externally.
}

OID Message::getSender() {
    return mSender;
}

OID Message::getReceiver() {
    return mReceiver;
}

OID Message::getTimestamp() {
    return mTimestamp;
}

OID Message::getBestbefore() {
    return mBestbefore;
}

std::string Message::getType() {
    return mType;
}

std::string Message::getText() {
    return mText;
}

void* Message::getBlob() {
    return mBlob;
}

/// Compare time for message priority queue, fifo.

bool Message::operator()(Message const* a, Message const* b) const {
    return a->mTimestamp > b->mTimestamp;
}

