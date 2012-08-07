#include "rChatMember.h"


#include "de/hackcraft/psi3d/Console.h"
#include "de/hackcraft/psi3d/GLS.h"

#include "de/hackcraft/world/World.h"

#include "de/hackcraft/world/sub/chat/rChatMessage.h"

#include <cassert>

std::string rChatMember::cname = "COMCOM";
unsigned int rChatMember::cid = 5007;

rChatMember::rChatMember(Entity* obj) {
    object = obj;
    
    id = (OID) this;

    int w = 60;
    int h = 10;
    mConsole = Console::Console_new(w * h, w);
    assert(mConsole != NULL);
    Console* console = (Console*) mConsole;
    //Console_printf(mConsole, "* Welcome to LinWarrior3D\n");
    Console::Console_print(console, "ComCOM(1) - MENU\n\n");
    Console::Console_print(console, " [1] Formation\n");
    Console::Console_print(console, " [2] Engage\n");
    Console::Console_print(console, " [3] Report\n");
    Console::Console_print(console, " [4] Objectives\n");
    Console::Console_print(console, "\n\n > ");
    mLastMessage = 0;
}

void rChatMember::animate(float spf) {
    rChatMessage* message = NULL; //World::getInstance()->recvMessage(0, mLastMessage);
    if (message != NULL) {
        mLastMessage = message->getTimestamp();
    }
}

void rChatMember::drawHUD() {
    if (!active) return;

    //GL::glColor4f(0,0.1,0,0.2);
//    GL::glBegin(GL_QUADS);
//    GL::glVertex3f(3, 1, 0);
//    GL::glVertex3f(0, 1, 0);
//    GL::glVertex3f(0, 0, 0);
//    GL::glVertex3f(3, 0, 0);
//    GL::glEnd();
    GL::glPushMatrix();
    {
        GL::glColor4f(0.9, 0.9, 0.9, 0.5);
        //GL::glColor4f(0.99, 0.99, 0.19, 1);
        Console* console = (Console*) mConsole;
        float w = console->width;
        float h = console->size / console->width;
        GL::glTranslatef(0, 1.0f + (1.0f / h), 0);
        GL::glScalef(3.0f / w, 1.0f / h, 1.0f);
        Console::Console_draw(console);
    }
    GL::glPopMatrix();
}

void rChatMember::recvMessage(rChatMessage* msg) {
    
    Console* console = (Console*) mConsole;
    Console::Console_print(console, msg->getText().c_str());
}
