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

#ifdef wonly_WIN32

extern "C" int __stdcall WinMain(void* hInstance, void* hPrevInstance, char* lpCmdLine, int nCmdShow) {
    int argc = 2;
    char *args[] = {"Nothing", "Nothing", "Nothing"};
#else

int main(int argc, char **args) {
#endif
    
    try {
        return (new GameMain())->run(argc, args);
    } catch (char* s) {
        cout << "Fatal exception caught:\n" << s << endl;
    } catch (const char* s) {
        cout << "Fatal exception caught:\n" << s << endl;
    }
    return 0;    
}
