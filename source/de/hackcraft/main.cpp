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

static int test(int argc, char **args) {
    cout << "Arguments for test module:\n";
    for (int i = 0; i < argc; i++) {
        cout << args[i] << "\n";
    }
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
        cout << "Fatal exception caught:\n" << s << endl;
        
    } catch (const char* s) {
        cout << "Fatal exception caught:\n" << s << endl;
    }
    return 0;    
}
