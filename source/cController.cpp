#include "cController.h"

#include "cObject.h"
#include "cWorld.h"

#include <cstdlib>
#include <cassert>
#include <iostream>

using std::cout;
using std::endl;
using std::string;

#define debug_state !true
#define debug_transitions !true
#define debug_step !true

// -------------------------------------------------------------

cController::cController(cObject* entity, bool enabled) {
    controlledDevice = entity;
    controllerEnabled = enabled;
}

cController::~cController() {
    while (!commandStack.empty()) commandStack.pop_back();
}

void cController::printState() {
    int framesize = getFrameSize();
    string framename = getFrameName();
    cout << "CtrlSt of " << controlledDevice->name.c_str() << "#" << controlledDevice->oid << "  Frame: " << framename << "  Framesize:" << framesize << "  Stack: " << commandStack.size() << endl;
}

string cController::getFrameName() {
    //cout << "getFrameName()\n";
    const char* names[] = {
        "WAIT", "ATTACK", "FOLLOW", "GOTO", "REPEAT",
        "NOSTATENAME"
    };
    unsigned int i = commandStack.back();
    i = (i < OPCODE_MAX) ? i : OPCODE_MAX - 1;
    string s = string(names[i]);
    return s;
};

unsigned int cController::getFrameSizeOf(int opcode) {
    switch (opcode) {
        case WAIT: return 3;
        case ATTACK: return 2;
        case FOLLOW: return 3;
        case GOTO: return 5;
        case REPEAT : return 2;
        default: return 2;
    }
}

unsigned int cController::getFrameSize() {
    return getFrameSizeOf(commandStack.back());
}

OID cController::getParameter(int offset) {
    return commandStack[commandStack.size() - offset - 1];
}

void cController::setParameter(int offset, OID value) {
    commandStack[commandStack.size() - offset - 1] = value;
}

void cController::push(OID value) {
    commandStack.push_back(value);
}

void cController::pop() {
    if (debug_transitions) {
        cout << controlledDevice->name.c_str() << "#" << controlledDevice->oid << ".pop()\n";
    }
    int size = getFrameSize(); // Important: eval outside loop-condition!
    loopi(size) commandStack.pop_back();
    if (debug_transitions) {
        printState();
    }
}

void cController::process(float spf) {
    if (debug_state) cout << "cController::process()\n";

    if (!controllerEnabled) return;
    if (controlledDevice == NULL) return;

    if (commandStack.empty()) pushWaitEvent();

    if (debug_step) printState();

    switch (commandStack.back()) {
        case WAIT: waitEvent();
            break;
        case ATTACK: attackEnemy();
            break;
        case FOLLOW: followLeader();
            break;
        case GOTO: gotoDestination();
            break;
        case REPEAT: repeatInstructions();
            break;
        default: cout << "Invalid Instruction Request!\n";
            break;
    }
    if (debug_step) {
        cout << "=> ";
        printState();
    }
}


// -------------------------------------------------------------

void cController::pushWaitEvent(long mseconds, bool patrol) {
    push(patrol);
    push(mseconds);
    push(WAIT);
}

void cController::waitEvent() {
    //OID opcode = getParameter(0);
    long mseconds = getParameter(1);
    bool patrol = getParameter(2);
    
    {
        controlledDevice->do_idle();
    }

    if (patrol) {
        OID enemy = 0;
        if (enemy == 0) enemy = controlledDevice->disturbedBy();
        if (enemy == 0) enemy = controlledDevice->enemyNearby();
        if (enemy) {
            {
                OID self = controlledDevice->oid;
                std::stringstream s;
                s << self << ": Intruder!\n";
                cWorld::instance->sendMessage(0, self, 0, "DEBUG", s.str());
            }
            pushAttackEnemy(enemy);
        }
    }

    if (mseconds > 0) {
        mseconds -= 1000/40;
        setParameter(1, mseconds);
        if (mseconds <= 0) {
            pop();
            return;
        }
    }
}

// -------------------------------------------------------------

void cController::pushAttackEnemy(OID entity) {
    if (debug_transitions) {
        cout << controlledDevice->name.c_str() << "#" << controlledDevice->oid << ".pushAttackEnemy( " << entity << " )\n";
    }
    {
        //OID self = mDevice->mSerial;
        //cWorld::instance->sendMessage("@%llu: All ur base belongs to us.\n", entity);
    }
    push(entity);
    push(ATTACK);
}
 
void cController::attackEnemy() {
    //OID opcode = getParameter(0);
    OID entity = getParameter(1);

    {
        controlledDevice->do_moveFor(NULL);
        controlledDevice->do_aimFor(entity);
        controlledDevice->do_moveNear();
        controlledDevice->do_aimAt();
        if (controlledDevice->inWeaponRange() > 0.5) controlledDevice->do_fireAt();
        //((cMech*)mDevice)->Pattern(tf, "nrnlln");
    }
    
    cObject* target = cWorld::instance->getObject(entity);
    if (target == NULL) {
        // Target disappeared (removed from world: fragged).
        controlledDevice->do_aimFor(NULL);
        pop();
        return;
    } else if (!controlledDevice->socialised->isEnemy(&target->tags)) {
        // Not an enemy anymore (maybe dead or not interesting anymore).
        controlledDevice->do_aimFor(NULL);
        pop();
        return;
    } else if (controlledDevice->inTargetRange() < 0.01) {
        // Target is out of targeting range.
        controlledDevice->do_aimFor(NULL);
        pop();
        return;
    }
}

// -------------------------------------------------------------

void cController::pushFollowLeader(OID entity, bool patrol) {
    if (debug_transitions) {
        cout << controlledDevice->name.c_str() << "#" << controlledDevice->oid << ".pushFollowLeader( " << entity << ", " << patrol << " )\n";
    }
    push(patrol ? 1 : 0);
    push(entity);
    push(FOLLOW);
}

void cController::followLeader() {
    //OID opcode = getParameter(0);
    OID entity = getParameter(1);
    OID patrol = getParameter(2);

    {
        controlledDevice->do_aimFor(entity);
        controlledDevice->do_moveFor(NULL);
        controlledDevice->do_moveNear();
        controlledDevice->do_aimAt();
    }

    if (patrol) {
        OID nearby = controlledDevice->enemyNearby();
        if (nearby) {
            pushAttackEnemy(nearby);
            return;
        }
    }
}

// -------------------------------------------------------------

void cController::pushGotoDestination(float* v, bool patrol) {
    if (v == NULL) return;
    if (debug_transitions) {
        cout << controlledDevice->name.c_str() << "#" << controlledDevice->oid << ".pushGotoDestination( <" << v[0] << "," << v[1] << "," << v[2] << ">, " << patrol << " )\n";
    }
    unsigned long *p = (unsigned long*) v;
    push(patrol ? !0 : 0);
    push(p[2]);
    push(p[1]);
    push(p[0]);
    push(GOTO);
}

void cController::gotoDestination() {
    //OID opcode = getParameter(0);
    OID v0 = getParameter(1);
    OID v1 = getParameter(2);
    OID v2 = getParameter(3);
    OID patrol = getParameter(4);
    unsigned long p[] = {
        v0, v1, v2
    };
    float* v = (float*) p;

    {
        if (debug_state) cout << "going " << ((patrol > 0) ? "patrolling" : "directly") << " to <" << v[0] << "," << v[1] << "," << v[2] << " >\n";
        controlledDevice->do_moveFor(v);
        //controlledDevice->do_aimFor(0);
        controlledDevice->do_aimAt();
        controlledDevice->do_moveTowards();
    }

    float range = controlledDevice->inDestinationRange();
    if (debug_state) cout << "DestinationRange " << range << endl;
    if (range > 0.0f) {
        pop();
        return;
    }

    if (patrol) {
        OID nearby = controlledDevice->enemyNearby();
        if (nearby) {
            pushAttackEnemy(nearby);
            return;
        }
    }
}

// -------------------------------------------------------------

void cController::pushRepeatInstructions(int n) {
    if (debug_transitions) {
        cout << controlledDevice->name.c_str() << "#" << controlledDevice->oid << ".pushRepeatInstructions( " << n << " )\n";
    }
    push(n);
    push(REPEAT);
}

void cController::repeatInstructions() {
    OID opcode = getParameter(0);
    OID n = getParameter(1);

    if (debug_state) {
        printState();
    }

    unsigned int first = getFrameSize();
    if (debug_state) cout << "first " << first << " -> opcode " << opcode << endl;
    unsigned int last = first;
    loopi(n) {
        int opcode = getParameter(last);
        last += getFrameSizeOf(opcode);
        if (debug_state) cout << "last " << last << " -> opcode " << opcode << endl;
    }

    int size = last - first;
    if (debug_state) cout << size << " = " << last << " - " << first << endl;

    loopi(size) {
        push(getParameter(last - 1));
    }
}


