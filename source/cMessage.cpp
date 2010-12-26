/* 
 * File:   cMessage.cpp
 * Author: benben
 * 
 * Created on 23. Dezember 2010, 15:41
 */

#include "cMessage.h"

cMessage::cMessage(OID sender, OID receiver, OID timestamp, OID bestbefore, std::string type, std::string text, void* blob) {
    mSender = sender;
    mReceiver = receiver;
    mTimestamp = timestamp;
    mBestbefore = bestbefore;
    mType = type;
    mText = text;
    mBlob = blob;
}

cMessage::cMessage(const cMessage& orig) {
    mSender = orig.mSender;
    mReceiver = orig.mReceiver;
    mTimestamp = orig.mTimestamp;
    mBestbefore = orig.mBestbefore;
    mType = orig.mType;
    mText = orig.mText;
    mBlob = orig.mBlob;
}

cMessage::~cMessage() {
    // Not our responsibility to delete anything:
    // delete getBlob() externally.
}

OID cMessage::getSender() {
    return mSender;
}

OID cMessage::getReceiver() {
    return mReceiver;
}

OID cMessage::getTimestamp() {
    return mTimestamp;
}

OID cMessage::getBestbefore() {
    return mBestbefore;
}

std::string cMessage::getType() {
    return mType;
}

std::string cMessage::getText() {
    return mText;
}

void* cMessage::getBlob() {
    return mBlob;
}

/// Compare time for message priority queue, fifo.
bool cMessage::operator()(cMessage const* a, cMessage const* b) const {
    return a->mTimestamp > b->mTimestamp;
}

