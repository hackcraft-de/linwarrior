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
	${OBJECTDIR}/source/de/hackcraft/world/comp/rTraceable.o \
	${OBJECTDIR}/source/de/hackcraft/world/comp/rGrouping.o \
	${OBJECTDIR}/source/de/hackcraft/world/comp/rRigged.o \
	${OBJECTDIR}/source/de/hackcraft/world/cBackground.o \
	${OBJECTDIR}/source/de/hackcraft/world/comp/weapon/rWeaponMachinegun.o \
	${OBJECTDIR}/source/de/hackcraft/io/Texfile.o \
	${OBJECTDIR}/source/de/hackcraft/proc/Road.o \
	${OBJECTDIR}/source/de/hackcraft/proc/Facade.o \
	${OBJECTDIR}/source/de/hackcraft/util/Message.o \
	${OBJECTDIR}/source/de/hackcraft/world/object/cAlert.o \
	${OBJECTDIR}/source/de/hackcraft/proc/Solid.o \
	${OBJECTDIR}/source/de/hackcraft/world/comp/computer/rComcom.o \
	${OBJECTDIR}/source/de/hackcraft/world/cWorld.o \
	${OBJECTDIR}/source/de/hackcraft/world/object/cObject.o \
	${OBJECTDIR}/source/de/hackcraft/world/object/cPlanetmap.o \
	${OBJECTDIR}/source/de/hackcraft/world/comp/rMobile.o \
	${OBJECTDIR}/source/de/hackcraft/world/comp/computer/rController.o \
	${OBJECTDIR}/source/de/hackcraft/world/comp/rNameable.o \
	${OBJECTDIR}/source/de/hackcraft/world/comp/weapon/rWeaponExplosion.o \
	${OBJECTDIR}/source/de/hackcraft/proc/Landscape.o \
	${OBJECTDIR}/source/de/hackcraft/world/comp/rPadmap.o \
	${OBJECTDIR}/source/de/hackcraft/world/comp/weapon/rWeaponPlasmagun.o \
	${OBJECTDIR}/source/de/hackcraft/main.o \
	${OBJECTDIR}/source/de/hackcraft/util/Timing.o \
	${OBJECTDIR}/source/de/hackcraft/world/object/cMech.o \
	${OBJECTDIR}/source/de/hackcraft/world/comp/weapon/rWeapon.o \
	${OBJECTDIR}/source/de/hackcraft/world/comp/weapon/rWeaponHoming.o \
	${OBJECTDIR}/source/de/hackcraft/world/object/cBuilding.o \
	${OBJECTDIR}/source/de/hackcraft/world/object/cTree.o \
	${OBJECTDIR}/source/de/hackcraft/world/cMission.o \
	${OBJECTDIR}/source/de/hackcraft/world/comp/rScatter.o \
	${OBJECTDIR}/source/de/hackcraft/world/comp/rDamageable.o \
	${OBJECTDIR}/source/de/hackcraft/world/comp/rComponent.o \
	${OBJECTDIR}/source/de/hackcraft/world/comp/weapon/rWeaponRaybeam.o \
	${OBJECTDIR}/source/de/hackcraft/world/comp/rCollider.o \
	${OBJECTDIR}/source/de/hackcraft/proc/Surface.o \
	${OBJECTDIR}/source/de/hackcraft/world/comp/rCamera.o \
	${OBJECTDIR}/source/de/hackcraft/world/comp/weapon/rWeaponSparkgun.o


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

${OBJECTDIR}/source/de/hackcraft/world/comp/rTraceable.o: source/de/hackcraft/world/comp/rTraceable.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/world/comp
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/world/comp/rTraceable.o source/de/hackcraft/world/comp/rTraceable.cpp

${OBJECTDIR}/source/de/hackcraft/world/comp/rGrouping.o: source/de/hackcraft/world/comp/rGrouping.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/world/comp
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/world/comp/rGrouping.o source/de/hackcraft/world/comp/rGrouping.cpp

${OBJECTDIR}/source/de/hackcraft/world/comp/rRigged.o: source/de/hackcraft/world/comp/rRigged.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/world/comp
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/world/comp/rRigged.o source/de/hackcraft/world/comp/rRigged.cpp

${OBJECTDIR}/source/de/hackcraft/world/cBackground.o: source/de/hackcraft/world/cBackground.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/world
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/world/cBackground.o source/de/hackcraft/world/cBackground.cpp

${OBJECTDIR}/source/de/hackcraft/world/comp/weapon/rWeaponMachinegun.o: source/de/hackcraft/world/comp/weapon/rWeaponMachinegun.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/world/comp/weapon
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/world/comp/weapon/rWeaponMachinegun.o source/de/hackcraft/world/comp/weapon/rWeaponMachinegun.cpp

${OBJECTDIR}/source/de/hackcraft/io/Texfile.o: source/de/hackcraft/io/Texfile.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/io
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/io/Texfile.o source/de/hackcraft/io/Texfile.cpp

${OBJECTDIR}/source/de/hackcraft/proc/Road.o: source/de/hackcraft/proc/Road.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/proc
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/proc/Road.o source/de/hackcraft/proc/Road.cpp

${OBJECTDIR}/source/de/hackcraft/proc/Facade.o: source/de/hackcraft/proc/Facade.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/proc
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/proc/Facade.o source/de/hackcraft/proc/Facade.cpp

${OBJECTDIR}/source/de/hackcraft/util/Message.o: source/de/hackcraft/util/Message.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/util
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/util/Message.o source/de/hackcraft/util/Message.cpp

${OBJECTDIR}/source/de/hackcraft/world/object/cAlert.o: source/de/hackcraft/world/object/cAlert.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/world/object
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/world/object/cAlert.o source/de/hackcraft/world/object/cAlert.cpp

${OBJECTDIR}/source/de/hackcraft/proc/Solid.o: source/de/hackcraft/proc/Solid.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/proc
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/proc/Solid.o source/de/hackcraft/proc/Solid.cpp

${OBJECTDIR}/source/de/hackcraft/world/comp/computer/rComcom.o: source/de/hackcraft/world/comp/computer/rComcom.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/world/comp/computer
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/world/comp/computer/rComcom.o source/de/hackcraft/world/comp/computer/rComcom.cpp

${OBJECTDIR}/source/de/hackcraft/world/cWorld.o: source/de/hackcraft/world/cWorld.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/world
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/world/cWorld.o source/de/hackcraft/world/cWorld.cpp

${OBJECTDIR}/source/de/hackcraft/world/object/cObject.o: source/de/hackcraft/world/object/cObject.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/world/object
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/world/object/cObject.o source/de/hackcraft/world/object/cObject.cpp

${OBJECTDIR}/source/de/hackcraft/world/object/cPlanetmap.o: source/de/hackcraft/world/object/cPlanetmap.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/world/object
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/world/object/cPlanetmap.o source/de/hackcraft/world/object/cPlanetmap.cpp

${OBJECTDIR}/source/de/hackcraft/world/comp/rMobile.o: source/de/hackcraft/world/comp/rMobile.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/world/comp
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/world/comp/rMobile.o source/de/hackcraft/world/comp/rMobile.cpp

${OBJECTDIR}/source/de/hackcraft/world/comp/computer/rController.o: source/de/hackcraft/world/comp/computer/rController.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/world/comp/computer
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/world/comp/computer/rController.o source/de/hackcraft/world/comp/computer/rController.cpp

${OBJECTDIR}/source/de/hackcraft/world/comp/rNameable.o: source/de/hackcraft/world/comp/rNameable.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/world/comp
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/world/comp/rNameable.o source/de/hackcraft/world/comp/rNameable.cpp

${OBJECTDIR}/source/de/hackcraft/world/comp/weapon/rWeaponExplosion.o: source/de/hackcraft/world/comp/weapon/rWeaponExplosion.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/world/comp/weapon
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/world/comp/weapon/rWeaponExplosion.o source/de/hackcraft/world/comp/weapon/rWeaponExplosion.cpp

${OBJECTDIR}/source/de/hackcraft/proc/Landscape.o: source/de/hackcraft/proc/Landscape.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/proc
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/proc/Landscape.o source/de/hackcraft/proc/Landscape.cpp

${OBJECTDIR}/source/de/hackcraft/world/comp/rPadmap.o: source/de/hackcraft/world/comp/rPadmap.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/world/comp
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/world/comp/rPadmap.o source/de/hackcraft/world/comp/rPadmap.cpp

${OBJECTDIR}/source/de/hackcraft/world/comp/weapon/rWeaponPlasmagun.o: source/de/hackcraft/world/comp/weapon/rWeaponPlasmagun.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/world/comp/weapon
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/world/comp/weapon/rWeaponPlasmagun.o source/de/hackcraft/world/comp/weapon/rWeaponPlasmagun.cpp

${OBJECTDIR}/source/de/hackcraft/main.o: source/de/hackcraft/main.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/main.o source/de/hackcraft/main.cpp

${OBJECTDIR}/source/de/hackcraft/util/Timing.o: source/de/hackcraft/util/Timing.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/util
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/util/Timing.o source/de/hackcraft/util/Timing.cpp

${OBJECTDIR}/source/de/hackcraft/world/object/cMech.o: source/de/hackcraft/world/object/cMech.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/world/object
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/world/object/cMech.o source/de/hackcraft/world/object/cMech.cpp

${OBJECTDIR}/source/de/hackcraft/world/comp/weapon/rWeapon.o: source/de/hackcraft/world/comp/weapon/rWeapon.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/world/comp/weapon
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/world/comp/weapon/rWeapon.o source/de/hackcraft/world/comp/weapon/rWeapon.cpp

${OBJECTDIR}/source/de/hackcraft/world/comp/weapon/rWeaponHoming.o: source/de/hackcraft/world/comp/weapon/rWeaponHoming.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/world/comp/weapon
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/world/comp/weapon/rWeaponHoming.o source/de/hackcraft/world/comp/weapon/rWeaponHoming.cpp

${OBJECTDIR}/source/de/hackcraft/world/object/cBuilding.o: source/de/hackcraft/world/object/cBuilding.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/world/object
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/world/object/cBuilding.o source/de/hackcraft/world/object/cBuilding.cpp

${OBJECTDIR}/source/de/hackcraft/world/object/cTree.o: source/de/hackcraft/world/object/cTree.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/world/object
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/world/object/cTree.o source/de/hackcraft/world/object/cTree.cpp

${OBJECTDIR}/source/de/hackcraft/world/cMission.o: source/de/hackcraft/world/cMission.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/world
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/world/cMission.o source/de/hackcraft/world/cMission.cpp

${OBJECTDIR}/source/de/hackcraft/world/comp/rScatter.o: source/de/hackcraft/world/comp/rScatter.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/world/comp
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/world/comp/rScatter.o source/de/hackcraft/world/comp/rScatter.cpp

${OBJECTDIR}/source/de/hackcraft/world/comp/rDamageable.o: source/de/hackcraft/world/comp/rDamageable.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/world/comp
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/world/comp/rDamageable.o source/de/hackcraft/world/comp/rDamageable.cpp

${OBJECTDIR}/source/de/hackcraft/world/comp/rComponent.o: source/de/hackcraft/world/comp/rComponent.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/world/comp
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/world/comp/rComponent.o source/de/hackcraft/world/comp/rComponent.cpp

${OBJECTDIR}/source/de/hackcraft/world/comp/weapon/rWeaponRaybeam.o: source/de/hackcraft/world/comp/weapon/rWeaponRaybeam.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/world/comp/weapon
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/world/comp/weapon/rWeaponRaybeam.o source/de/hackcraft/world/comp/weapon/rWeaponRaybeam.cpp

${OBJECTDIR}/source/de/hackcraft/world/comp/rCollider.o: source/de/hackcraft/world/comp/rCollider.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/world/comp
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/world/comp/rCollider.o source/de/hackcraft/world/comp/rCollider.cpp

${OBJECTDIR}/source/de/hackcraft/proc/Surface.o: source/de/hackcraft/proc/Surface.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/proc
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/proc/Surface.o source/de/hackcraft/proc/Surface.cpp

${OBJECTDIR}/source/de/hackcraft/world/comp/rCamera.o: source/de/hackcraft/world/comp/rCamera.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/world/comp
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/world/comp/rCamera.o source/de/hackcraft/world/comp/rCamera.cpp

${OBJECTDIR}/source/de/hackcraft/world/comp/weapon/rWeaponSparkgun.o: source/de/hackcraft/world/comp/weapon/rWeaponSparkgun.cpp 
	${MKDIR} -p ${OBJECTDIR}/source/de/hackcraft/world/comp/weapon
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/de/hackcraft/world/comp/weapon/rWeaponSparkgun.o source/de/hackcraft/world/comp/weapon/rWeaponSparkgun.cpp

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
