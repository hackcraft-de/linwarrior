#include "Main.h"

// Startup code: Dispatching sub program startups.

#include "de/hackcraft/game/GameMain.h"

#include "de/hackcraft/util/HashMap.h"

#include "de/hackcraft/lang/String.h"

#include "de/hackcraft/log/Logger.h"

#include <cstdlib>
#include <cstdio>
#include <iostream>


/** Just a test class. */
class TestMain : public Main {
public:
    
    ~TestMain() {
        std::cout << "Tearing down.\n";
    }
    
    
    virtual int run(int argc, char** args) {
        
        std::cout << "Arguments for test module:\n";
        
        for (int i = 0; i < argc; i++) {
            std::cout << args[i] << "\n";
        }
        
        std::cout << "Testing...\n";
        
        //testHM();
        testLogger();
        
        std::cout << "Finishing.\n";
        
        return 0;
    }
    
private:
    
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

    
    void testLogger() {
        
        Logger* logger = Logger::getLogger("/de/hackcraft/main");

        logger->trace() << "log" << " test " << 1 << "\n";
        logger->trace() + "log" + " test " + 1 + "\n";
    }
    
};


Main* mainInstance;


/** Called at exit */
void cleanup() {
    
    if (mainInstance != NULL) {
        delete mainInstance;
    }
}


//#ifdef wonly_WIN32
//extern "C" int __stdcall WinMain(void* hInstance, void* hPrevInstance, char* lpCmdLine, int nCmdShow) {
//    int argc = 2;
//    char *args[] = {"Nothing", "Nothing", "Nothing"};
//#else
//#endif

int main(int argc, char **args) {
    
    try {
        atexit(cleanup);
        
        std::string subprogram = (argc >= 2) ? std::string(args[1]) : std::string("");
        
        if (subprogram == "game") {
            mainInstance = new GameMain();
            
        } else if (subprogram == "test") {
            mainInstance = new TestMain();
            
        } else {
            std::cout << "Please specify module to run: <program> <module> [parameters]\n";
            std::cout << "  game   Run game module\n";
            std::cout << "  test   Run test module\n";
            return 0;
        }
        
        return mainInstance->run(argc-1, &args[1]);
        
    } catch (char* s) {
        std::cout << "Fatal exception caught:\n" << s << "\n";
        
    } catch (const char* s) {
        std::cout << "Fatal exception caught:\n" << s << "\n";
    }
    return 0;    
}

