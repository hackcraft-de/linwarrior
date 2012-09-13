/* 
 * File:    AL.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on September 13, 2012, 8:42 PM
 */


#ifndef AL_H
#define	AL_H

#if 1

#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <ALUT/alut.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#ifndef __WIN32
#include <AL/alext.h>
#endif
#include <AL/alut.h>
#endif


#else

class AL {
public:

private:

};

#endif

#endif	/* AL_H */

