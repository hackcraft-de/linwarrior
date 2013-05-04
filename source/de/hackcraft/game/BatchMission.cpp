#include "BatchMission.h"

#include "de/hackcraft/lang/Thread.h"

#include "de/hackcraft/log/Logger.h"

#include "de/hackcraft/world/Entity.h"
#include "de/hackcraft/world/World.h"

#include "de/hackcraft/world/object/cMech.h"

#include "de/hackcraft/world/sub/background/BackgroundSystem.h"

#include "de/hackcraft/world/sub/camera/CameraSystem.h"

#include "de/hackcraft/world/sub/chat/ChatSystem.h"

#include "de/hackcraft/world/sub/cityscape/CityscapeSystem.h"

#include "de/hackcraft/world/sub/computer/ComputerSystem.h"

#include "de/hackcraft/world/sub/landscape/LandscapeSystem.h"
#include "de/hackcraft/world/sub/landscape/rPlanetmap.h"

#include "de/hackcraft/world/sub/misc/MiscSystem.h"

#include "de/hackcraft/world/sub/mobile/MobileSystem.h"
#include "de/hackcraft/world/sub/mobile/rMobile.h"

#include "de/hackcraft/world/sub/model/ModelSystem.h"

#include "de/hackcraft/world/sub/physics/PhysicsSystem.h"

#include "de/hackcraft/world/sub/trigger/TriggerSystem.h"
#include "de/hackcraft/world/sub/trigger/rTrigger.h"

#include "de/hackcraft/world/sub/weapon/WeaponSystem.h"
#include "de/hackcraft/world/sub/weapon/rTarget.h"


#include <fstream>
#include <list>
#include <string>
#include <sstream>


Logger* BatchMission::logger = Logger::getLogger("de.hackcraft.game.BatchMission");


BatchMission::BatchMission() {
}


BatchMission::~BatchMission() {
}


Entity* BatchMission::init(World* world) {
    
    
    
    srand(0);
    
    logger->info() << "Loading global properties...\n";
    
    //try {
        globalProperties.load("/base/global.properties");
    //} catch (...) {
    //    logger->info() << "Could not load global properties.\n";
    //}
    
    logger->info() << "Setting mission date and time...\n";
    
    if (!true) {
        world->getTiming()->setTime(12);
    } else {
        float hour;
        getSystemHour(hour);
        hour = fmod(hour, 24);
        world->getTiming()->setTime(hour, 60 * (hour - (int) hour));
    }

    if (true) {
        logger->info() << "Initializing subsystems...\n";
        initSystems();
    }

    if (true) {
        logger->info() << "Initializing planetary maps...\n";
        planetmap = new rPlanetmap(&globalProperties);
        LandscapeSystem::getInstance()->add(planetmap);
    }
    
    if (false) {
        logger->info() << "Initializing call groups...\n";
        //group_alliance_player = ChatSystem::getInstance()->getGroup("/call/alliance/player");
        //group_alliance_wingmen = ChatSystem::getInstance()->getGroup("/call/alliance/wingmen");
        //group_alliance_all = ChatSystem::getInstance()->getGroup("/call/alliance/all");
        //group_enemies_all = ChatSystem::getInstance()->getGroup("/call/enemies/all");
    }

    if (false) {
        logger->info() << "Initializing default sensitivities...\n";
        //inc_sense.insert(World::getInstance()->getGroup(PLR_HUMAN));
        //exc_sense.insert(World::getInstance()->getGroup(HLT_DEAD));
    }
    
    if (true) {
        logger->info() << "Loading mission...\n";
        Thread::sleep(2000);
        std::ifstream fis("data/base/game/mission.cfg");
        execScript(&fis);
        fis.close();
    }
    
    //Thread::sleep(4000);
    //throw "abort";
    
    float p[] = {0,0,0};
    Entity* player = initPlayerParty(p);
        
    return player;
}


void BatchMission::initSystems() {
    
    World* world = World::getInstance();
    
    logger->info() << "Initializing chat subsystem...\n";
    world->subsystems.push_back(new ChatSystem());

    logger->info() << "Initializing background subsystem...\n";
    world->subsystems.push_back(new BackgroundSystem(&globalProperties));
    
    logger->info() << "Initializing trigger subsystem...\n";
    world->subsystems.push_back(new TriggerSystem());

    logger->info() << "Initializing city subsystem...\n";
    world->subsystems.push_back(new CityscapeSystem());

    logger->info() << "Initializing experimental or preliminary subsystems...\n";
    world->subsystems.push_back(new LandscapeSystem());
    world->subsystems.push_back(new MobileSystem());
    world->subsystems.push_back(new PhysicsSystem());
    world->subsystems.push_back(new ModelSystem());
    world->subsystems.push_back(new WeaponSystem());
    world->subsystems.push_back(new ComputerSystem());
    world->subsystems.push_back(new CameraSystem());
    world->subsystems.push_back(new MiscSystem());
}


int BatchMission::execScript(std::istream* is) {
    
    int result = 0;
    
    std::istream& fis = *is;
            
    logger->info() << "test\n";
    
    // Read command lines:
    
    for (std::string line; std::getline(fis, line); ) {
        
        logger->info() << line << "\n";

        std::stringstream lis(line);
        
        std::list<std::string> args;
        //std::list<std::string>* args = new std::list<std::string>();
        
        // Read command line arguments from current line:
        
        for (std::string arg; std::getline(lis, arg,' '); ) {
            
            //logger->info() << arg << "\n";
            args.push_back(arg);
        }
        
        if (false) {
            for (std::string arg : args) {

                logger->info() << arg << "|";
            }
            logger->info() << "\n";
        }
        
        // Dispatch command:
        
        //Thread::sleep(500);
        result = execCommand(&args);
        
        //delete args;
    }
    
    return result;
}


int BatchMission::execCommand(std::list<std::string>* args) {
    
    int result = 0;
    
    std::string cmd = args->front();
    
    if (cmd.compare("set") == 0) {
        //logger->info() << "set command\n";
        execCommandSet(args);
    } else {
        //logger->info() << "unknown command " << "!\n";
    }
 
    return result;
}


int BatchMission::execCommandSet(std::list<std::string>* args) {
    
    int result = 0;
    
    args->pop_front();
    std::string key = args->front();
    args->pop_front();
    std::string val = args->front();
    
    logger->info() << "set [" << key << "] = [" << val << "]\n";
    
    globalProperties.put(key, val);
    
    return result;
}


Entity* BatchMission::initPlayerParty(float* position) {
    
    Entity* player = NULL;

    float* p = position;

    // Player
    if (true) {
        float* pos = vector_new(p[0] - 0, 0.1, p[2] - 0);
        float* rot = vector_new(0, -170, 0);

        adjustHeight(pos);

        Propmap c;

        std::stringstream pos_;
        pos_ << "(" << pos[0] << "," << pos[1] << "," << pos[2] << ")";
        c.put("mech.pos", pos_.str());

        std::stringstream rot_;
        rot_ << "(" << rot[0] << "," << rot[1] << "," << rot[2] << ")";
        c.put("mech.rot", rot_.str());

        c.put("mech.name", "Playermech");
        c.put("mech.displayname", "Alpha");
        c.put("mech.model", globalProperties.getProperty("mission.player.mech", "frogger"));
        //c.put("mech.model", "frogger");
        c.put("mech.ai", "false");
        //c.put("mech.tags"], "{FAC_BLUE,PLR_HUMAN}");
        //c.put("mech.enemies"], "{FAC_RED}");
        //c.put("mech.notenemies"], "{HLT_DEAD}");
        
        c.put("mech.Center", "none");
        c.put("mech.LTorsor", "Plasma");
        c.put("mech.RTorsor", "Raybeam");
        c.put("mech.LUpArm", "Homing");
        c.put("mech.RUpArm", "Homing");
        c.put("mech.LLoArm", "Machinegun");
        c.put("mech.RLoArm", "Machinegun");

        cMech* mech = new cMech(&c);
        if (mech == NULL) throw "No memory for player mech!";
        player = mech;
        
        mech->target->addTag(World::getInstance()->getGroup(FAC_BLUE));
        mech->target->addTag(World::getInstance()->getGroup(PLR_HUMAN));
        
        mech->tarcom->addEnemy(World::getInstance()->getGroup(FAC_RED));
        mech->tarcom->addEnemy(World::getInstance()->getGroup(HLT_DEAD), false);

        World::getInstance()->spawnObject(mech);
        //ChatSystem::getInstance()->addToGroup(group_alliance_player, mech);
        //ChatSystem::getInstance()->addToGroup(group_alliance_wingmen, mech);
        //ChatSystem::getInstance()->addToGroup(group_alliance_all, mech);
        logger->debug() << "after first spawn\n";

        rTrigger* trigger = new rTrigger();
        trigger->addTag(World::getInstance()->getGroup(FAC_BLUE));
        trigger->addTag(World::getInstance()->getGroup(PLR_HUMAN));
        TriggerSystem::getInstance()->add(trigger);
        mech->components.push_back(trigger);
        trigger->addBinding(&trigger->pos0, mech->mobile->pos0, sizeof(vec3));
    }

    return player;
}


void BatchMission::adjustHeight(float* pos) {
    float color[16];
    planetmap->getHeight(pos[0], pos[2], color);
    pos[1] = color[3] + 0.001f;
}

