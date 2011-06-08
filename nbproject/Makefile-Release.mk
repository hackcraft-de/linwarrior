#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Release
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/source/de/hackcraft/comp/rGrouping.o \
	${OBJECTDIR}/source/de/hackcraft/comp/rController.o \
	${OBJECTDIR}/source/de/hackcraft/world/cBackground.o \
	${OBJECTDIR}/source/de/hackcraft/io/Texfile.o \
	${OBJECTDIR}/source/de/hackcraft/proc/Road.o \
	${OBJECTDIR}/source/de/hackcraft/comp/rRigged.o \
	${OBJECTDIR}/source/de/hackcraft/proc/Facade.o \
	${OBJECTDIR}/source/de/hackcraft/comp/rNameable.o \
	${OBJECTDIR}/source/de/hackcraft/comp/rTraceable.o \
	${OBJECTDIR}/source/de/hackcraft/util/Message.o \
	${OBJECTDIR}/source/de/hackcraft/object/cTree.o \
	${OBJECTDIR}/source/de/hackcraft/object/cPlanetmap.o \
	${OBJECTDIR}/source/de/hackcraft/comp/rCollider.o \
	${OBJECTDIR}/source/de/hackcraft/proc/Solid.o \
	${OBJECTDIR}/source/de/hackcraft/world/cWorld.o \
	${OBJECTDIR}/source/de/hackcraft/comp/rComponent.o \
	${OBJECTDIR}/source/de/hackcraft/comp/rCamera.o \
	${OBJECTDIR}/source/de/hackcraft/comp/rWeaponMachinegun.o \
	${OBJECTDIR}/source/de/hackcraft/proc/Landscape.o \
	${OBJECTDIR}/source/de/hackcraft/object/cBuilding.o \
	${OBJECTDIR}/source/de/hackcraft/comp/rScatter.o \
	${OBJECTDIR}/source/de/hackcraft/main.o \
	${OBJECTDIR}/source/de/hackcraft/object/cAlert.o \
	${OBJECTDIR}/source/de/hackcraft/comp/rWeapon.o \
	${OBJECTDIR}/source/de/hackcraft/comp/rPadmap.o \
	${OBJECTDIR}/source/de/hackcraft/comp/rMobile.o \
	${OBJECTDIR}/source/de/hackcraft/util/Timing.o \
	${OBJECTDIR}/source/de/hackcraft/comp/rComcom.o \
	${OBJECTDIR}/source/de/hackcraft/world/cMission.o \
	${OBJECTDIR}/source/de/hackcraft/comp/rWeaponSparkgun.o \
	${OBJECTDIR}/source/de/hackcraft/comp/rWeaponExplosion.o \
	${OBJECTDIR}/source/de/hackcraft/comp/rWeaponPlasmagun.o \
	${OBJECTDIR}/source/de/hackcraft/comp/rDamageable.o \
	${OBJECTDIR}/source/de/hackcraft/comp/rWeaponHoming.o \
	${OBJECTDIR}/source/de/hackcraft/proc/Surface.o \
	${OBJECTDIR}/source/de/hackcraft/object/cObject.o \
	${OBJECTDIR}/source/de/hackcraft/comp/rWeaponRaybeam.o \
	${OBJECTDIR}/source/de/hackcraft/object/cMech.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-Release.mk dist/Release/GNU-Linux-x86/linwarrior

dist/Release/GNU-Linux-x86/linwarrior: ${OBJECTFILES}
	${MKDIR} -p dist/Release/GNU-Linux-x86
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/linwarrior ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/source/de/hackcraft/comp/rGrouping.o: source/de/hackcraft/comp/rGrouping.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/comp
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/comp/rGrouping.o source/de/hackcraft/comp/rGrouping.cpp

${OBJECTDIR}/source/de/hackcraft/comp/rController.o: source/de/hackcraft/comp/rController.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/comp
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/comp/rController.o source/de/hackcraft/comp/rController.cpp

${OBJECTDIR}/source/de/hackcraft/world/cBackground.o: source/de/hackcraft/world/cBackground.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/world
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/world/cBackground.o source/de/hackcraft/world/cBackground.cpp

${OBJECTDIR}/source/de/hackcraft/io/Texfile.o: source/de/hackcraft/io/Texfile.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/io
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/io/Texfile.o source/de/hackcraft/io/Texfile.cpp

${OBJECTDIR}/source/de/hackcraft/proc/Road.o: source/de/hackcraft/proc/Road.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/proc
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/proc/Road.o source/de/hackcraft/proc/Road.cpp

${OBJECTDIR}/source/de/hackcraft/comp/rRigged.o: source/de/hackcraft/comp/rRigged.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/comp
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/comp/rRigged.o source/de/hackcraft/comp/rRigged.cpp

${OBJECTDIR}/source/de/hackcraft/proc/Facade.o: source/de/hackcraft/proc/Facade.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/proc
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/proc/Facade.o source/de/hackcraft/proc/Facade.cpp

${OBJECTDIR}/source/de/hackcraft/comp/rNameable.o: source/de/hackcraft/comp/rNameable.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/comp
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/comp/rNameable.o source/de/hackcraft/comp/rNameable.cpp

${OBJECTDIR}/source/de/hackcraft/comp/rTraceable.o: source/de/hackcraft/comp/rTraceable.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/comp
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/comp/rTraceable.o source/de/hackcraft/comp/rTraceable.cpp

${OBJECTDIR}/source/de/hackcraft/util/Message.o: source/de/hackcraft/util/Message.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/util
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/util/Message.o source/de/hackcraft/util/Message.cpp

${OBJECTDIR}/source/de/hackcraft/object/cTree.o: source/de/hackcraft/object/cTree.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/object
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/object/cTree.o source/de/hackcraft/object/cTree.cpp

${OBJECTDIR}/source/de/hackcraft/object/cPlanetmap.o: source/de/hackcraft/object/cPlanetmap.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/object
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/object/cPlanetmap.o source/de/hackcraft/object/cPlanetmap.cpp

${OBJECTDIR}/source/de/hackcraft/comp/rCollider.o: source/de/hackcraft/comp/rCollider.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/comp
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/comp/rCollider.o source/de/hackcraft/comp/rCollider.cpp

${OBJECTDIR}/source/de/hackcraft/proc/Solid.o: source/de/hackcraft/proc/Solid.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/proc
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/proc/Solid.o source/de/hackcraft/proc/Solid.cpp

${OBJECTDIR}/source/de/hackcraft/world/cWorld.o: source/de/hackcraft/world/cWorld.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/world
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/world/cWorld.o source/de/hackcraft/world/cWorld.cpp

${OBJECTDIR}/source/de/hackcraft/comp/rComponent.o: source/de/hackcraft/comp/rComponent.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/comp
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/comp/rComponent.o source/de/hackcraft/comp/rComponent.cpp

${OBJECTDIR}/source/de/hackcraft/comp/rCamera.o: source/de/hackcraft/comp/rCamera.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/comp
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/comp/rCamera.o source/de/hackcraft/comp/rCamera.cpp

${OBJECTDIR}/source/de/hackcraft/comp/rWeaponMachinegun.o: source/de/hackcraft/comp/rWeaponMachinegun.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/comp
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/comp/rWeaponMachinegun.o source/de/hackcraft/comp/rWeaponMachinegun.cpp

${OBJECTDIR}/source/de/hackcraft/proc/Landscape.o: source/de/hackcraft/proc/Landscape.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/proc
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/proc/Landscape.o source/de/hackcraft/proc/Landscape.cpp

${OBJECTDIR}/source/de/hackcraft/object/cBuilding.o: source/de/hackcraft/object/cBuilding.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/object
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/object/cBuilding.o source/de/hackcraft/object/cBuilding.cpp

${OBJECTDIR}/source/de/hackcraft/comp/rScatter.o: source/de/hackcraft/comp/rScatter.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/comp
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/comp/rScatter.o source/de/hackcraft/comp/rScatter.cpp

${OBJECTDIR}/source/de/hackcraft/main.o: source/de/hackcraft/main.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/main.o source/de/hackcraft/main.cpp

${OBJECTDIR}/source/de/hackcraft/object/cAlert.o: source/de/hackcraft/object/cAlert.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/object
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/object/cAlert.o source/de/hackcraft/object/cAlert.cpp

${OBJECTDIR}/source/de/hackcraft/comp/rWeapon.o: source/de/hackcraft/comp/rWeapon.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/comp
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/comp/rWeapon.o source/de/hackcraft/comp/rWeapon.cpp

${OBJECTDIR}/source/de/hackcraft/comp/rPadmap.o: source/de/hackcraft/comp/rPadmap.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/comp
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/comp/rPadmap.o source/de/hackcraft/comp/rPadmap.cpp

${OBJECTDIR}/source/de/hackcraft/comp/rMobile.o: source/de/hackcraft/comp/rMobile.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/comp
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/comp/rMobile.o source/de/hackcraft/comp/rMobile.cpp

${OBJECTDIR}/source/de/hackcraft/util/Timing.o: source/de/hackcraft/util/Timing.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/util
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/util/Timing.o source/de/hackcraft/util/Timing.cpp

${OBJECTDIR}/source/de/hackcraft/comp/rComcom.o: source/de/hackcraft/comp/rComcom.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/comp
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/comp/rComcom.o source/de/hackcraft/comp/rComcom.cpp

${OBJECTDIR}/source/de/hackcraft/world/cMission.o: source/de/hackcraft/world/cMission.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/world
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/world/cMission.o source/de/hackcraft/world/cMission.cpp

${OBJECTDIR}/source/de/hackcraft/comp/rWeaponSparkgun.o: source/de/hackcraft/comp/rWeaponSparkgun.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/comp
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/comp/rWeaponSparkgun.o source/de/hackcraft/comp/rWeaponSparkgun.cpp

${OBJECTDIR}/source/de/hackcraft/comp/rWeaponExplosion.o: source/de/hackcraft/comp/rWeaponExplosion.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/comp
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/comp/rWeaponExplosion.o source/de/hackcraft/comp/rWeaponExplosion.cpp

${OBJECTDIR}/source/de/hackcraft/comp/rWeaponPlasmagun.o: source/de/hackcraft/comp/rWeaponPlasmagun.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/comp
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/comp/rWeaponPlasmagun.o source/de/hackcraft/comp/rWeaponPlasmagun.cpp

${OBJECTDIR}/source/de/hackcraft/comp/rDamageable.o: source/de/hackcraft/comp/rDamageable.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/comp
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/comp/rDamageable.o source/de/hackcraft/comp/rDamageable.cpp

${OBJECTDIR}/source/de/hackcraft/comp/rWeaponHoming.o: source/de/hackcraft/comp/rWeaponHoming.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/comp
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/comp/rWeaponHoming.o source/de/hackcraft/comp/rWeaponHoming.cpp

${OBJECTDIR}/source/de/hackcraft/proc/Surface.o: source/de/hackcraft/proc/Surface.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/proc
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/proc/Surface.o source/de/hackcraft/proc/Surface.cpp

${OBJECTDIR}/source/de/hackcraft/object/cObject.o: source/de/hackcraft/object/cObject.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/object
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/object/cObject.o source/de/hackcraft/object/cObject.cpp

${OBJECTDIR}/source/de/hackcraft/comp/rWeaponRaybeam.o: source/de/hackcraft/comp/rWeaponRaybeam.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/comp
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/comp/rWeaponRaybeam.o source/de/hackcraft/comp/rWeaponRaybeam.cpp

${OBJECTDIR}/source/de/hackcraft/object/cMech.o: source/de/hackcraft/object/cMech.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/object
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/object/cMech.o source/de/hackcraft/object/cMech.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Release
	${RM} dist/Release/GNU-Linux-x86/linwarrior

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
