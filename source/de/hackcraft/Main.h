/* 
 * File:     Main.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on March 28, 2008, 21:25 PM (1999)
 */

#ifndef MAIN_H
#define	MAIN_H

class Main {
public:
    /** Called at exit, needs to be declared here to suppress warning. */
    virtual ~Main() {};
    /** Called directly by the main entry point. */
    virtual int run(int argc, char** args) = 0;
};

#endif	/* MAIN_H */

