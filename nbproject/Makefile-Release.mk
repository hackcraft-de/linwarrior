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
	${OBJECTDIR}/source/main.o \
	${OBJECTDIR}/source/cWeaponSparkgun.o \
	${OBJECTDIR}/source/cWeaponMachinegun.o \
	${OBJECTDIR}/source/cAlert.o \
	${OBJECTDIR}/source/cComcom.o \
	${OBJECTDIR}/source/cLandscape.o \
	${OBJECTDIR}/source/cWeaponHoming.o \
	${OBJECTDIR}/source/cWeaponPlasmagun.o \
	${OBJECTDIR}/source/cBuilding.o \
	${OBJECTDIR}/source/cBackground.o \
	${OBJECTDIR}/source/cWorld.o \
	${OBJECTDIR}/source/cWeapon.o \
	${OBJECTDIR}/source/cObject.o \
	${OBJECTDIR}/source/cSolid.o \
	${OBJECTDIR}/source/cTree.o \
	${OBJECTDIR}/source/cPlanetmap.o \
	${OBJECTDIR}/source/cPadmap.o \
	${OBJECTDIR}/source/cWeaponExplosion.o \
	${OBJECTDIR}/source/cMission.o \
	${OBJECTDIR}/source/cWeaponRaybeam.o \
	${OBJECTDIR}/source/cMech.o \
	${OBJECTDIR}/source/cController.o \
	${OBJECTDIR}/source/cSurface.o


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

${OBJECTDIR}/source/main.o: source/main.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/main.o source/main.cpp

${OBJECTDIR}/source/cWeaponSparkgun.o: source/cWeaponSparkgun.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/cWeaponSparkgun.o source/cWeaponSparkgun.cpp

${OBJECTDIR}/source/cWeaponMachinegun.o: source/cWeaponMachinegun.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/cWeaponMachinegun.o source/cWeaponMachinegun.cpp

${OBJECTDIR}/source/cAlert.o: source/cAlert.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/cAlert.o source/cAlert.cpp

${OBJECTDIR}/source/cComcom.o: source/cComcom.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/cComcom.o source/cComcom.cpp

${OBJECTDIR}/source/cLandscape.o: source/cLandscape.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/cLandscape.o source/cLandscape.cpp

${OBJECTDIR}/source/cWeaponHoming.o: source/cWeaponHoming.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/cWeaponHoming.o source/cWeaponHoming.cpp

${OBJECTDIR}/source/cWeaponPlasmagun.o: source/cWeaponPlasmagun.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/cWeaponPlasmagun.o source/cWeaponPlasmagun.cpp

${OBJECTDIR}/source/cBuilding.o: source/cBuilding.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/cBuilding.o source/cBuilding.cpp

${OBJECTDIR}/source/cBackground.o: source/cBackground.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/cBackground.o source/cBackground.cpp

${OBJECTDIR}/source/cWorld.o: source/cWorld.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/cWorld.o source/cWorld.cpp

${OBJECTDIR}/source/cWeapon.o: source/cWeapon.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/cWeapon.o source/cWeapon.cpp

${OBJECTDIR}/source/cObject.o: source/cObject.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/cObject.o source/cObject.cpp

${OBJECTDIR}/source/cSolid.o: source/cSolid.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/cSolid.o source/cSolid.cpp

${OBJECTDIR}/source/cTree.o: source/cTree.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/cTree.o source/cTree.cpp

${OBJECTDIR}/source/cPlanetmap.o: source/cPlanetmap.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/cPlanetmap.o source/cPlanetmap.cpp

${OBJECTDIR}/source/cPadmap.o: source/cPadmap.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/cPadmap.o source/cPadmap.cpp

${OBJECTDIR}/source/cWeaponExplosion.o: source/cWeaponExplosion.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/cWeaponExplosion.o source/cWeaponExplosion.cpp

${OBJECTDIR}/source/cMission.o: source/cMission.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/cMission.o source/cMission.cpp

${OBJECTDIR}/source/cWeaponRaybeam.o: source/cWeaponRaybeam.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/cWeaponRaybeam.o source/cWeaponRaybeam.cpp

${OBJECTDIR}/source/cMech.o: source/cMech.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/cMech.o source/cMech.cpp

${OBJECTDIR}/source/cController.o: source/cController.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/cController.o source/cController.cpp

${OBJECTDIR}/source/cSurface.o: source/cSurface.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/cSurface.o source/cSurface.cpp

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
