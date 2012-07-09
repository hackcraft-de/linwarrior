/* 
 * File:     main.cpp
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on March 28, 2008, 21:25 PM (1999)
 */


// Startup code: Dispatching sub program startups.

#include <cstdlib>
#include <cstdio>

using namespace std;

#include "de/hackcraft/game/GameMain.h"

#include <de/hackcraft/util/HashMap.h>
#include <de/hackcraft/lang/String.h>

void testHM1() {
    HashMap<Object*,Object*>* h = new HashMap<Object*,Object*>();
    
    Object a;
    Object b;
    h->put(&a, &b);
}

void testHM2() {
    HashMap<String*,String*>* h = new HashMap<String*,String*>();
    
    for (int i = 0; i <= ('z'-'a'); i++) {
        //std::cout << i << "\n";
        char x[] = { char('a' + i), '\0' };
        char y[] = { char('A' + i), '\0' };
        h->put(new String(x), new String(y));
    }
    for (int i = 0; i <= ('z'-'a'); i++) {
        char x[] = { char('a' + i), '\0' };
        String* y = h->get(new String(x));
        std::cout << y->c_str();
    }
}

void testHM() {
    //testHM1();
    testHM2();
}

#include "de/hackcraft/log/Logger.h"

void testLogger() {
    Logger* logger = Logger::getLogger("/de/hackcraft/main");
    
    logger->trace() << "log" << " test " << 1 << "\n";
    logger->trace() + "log" + " test " + 1 + "\n";
}

static int test(int argc, char **args) {
    cout << "Arguments for test module:\n";
    for (int i = 0; i < argc; i++) {
        cout << args[i] << "\n";
    }
    //testHM();
    testLogger();
    return 0;
}

//#ifdef wonly_WIN32
//extern "C" int __stdcall WinMain(void* hInstance, void* hPrevInstance, char* lpCmdLine, int nCmdShow) {
//    int argc = 2;
//    char *args[] = {"Nothing", "Nothing", "Nothing"};
//#else
//#endif

int main(int argc, char **args) {
    
    try {
        string subprogram = (argc >= 2) ? string(args[1]) : string("");
        
        if (subprogram == "game") {
            return (new GameMain())->run(argc-1, &args[1]);
            
        } else if (subprogram == "test") {
            return test(argc-1, &args[1]);
            
        } else {
            cout << "Please specify module to run: <program> <module> [parameters]\n";
            cout << "  game   Run game module\n";
            cout << "  test   Run test module\n";
            return 0;
        }
        
    } catch (char* s) {
        cout << "Fatal exception caught:\n" << s << "\n";
        
    } catch (const char* s) {
        cout << "Fatal exception caught:\n" << s << "\n";
    }
    return 0;    
}
