#include "rChatMessage.h"

rChatMessage::rChatMessage(OID sender, OID receiver, OID timestamp, OID bestbefore, std::string type, std::string text, void* blob) {
    mSender = sender;
    mReceiver = receiver;
    mTimestamp = timestamp;
    mBestbefore = bestbefore;
    mType = type;
    mText = text;
    mBlob = blob;
}

rChatMessage::rChatMessage(const rChatMessage* orig) {
    mSender = orig->mSender;
    mReceiver = orig->mReceiver;
    mTimestamp = orig->mTimestamp;
    mBestbefore = orig->mBestbefore;
    mType = orig->mType;
    mText = orig->mText;
    mBlob = orig->mBlob;
}

rChatMessage::~rChatMessage() {
    // Not our responsibility to delete anything:
    // delete getBlob() externally.
}

OID rChatMessage::getSender() {
    return mSender;
}

OID rChatMessage::getReceiver() {
    return mReceiver;
}

OID rChatMessage::getTimestamp() {
    return mTimestamp;
}

OID rChatMessage::getBestbefore() {
    return mBestbefore;
}

std::string rChatMessage::getType() {
    return mType;
}

std::string rChatMessage::getText() {
    return mText;
}

void* rChatMessage::getBlob() {
    return mBlob;
}

/// Compare time for message priority queue, fifo.

bool rChatMessage::operator()(rChatMessage const* a, rChatMessage const* b) const {
    return a->mTimestamp > b->mTimestamp;
}

