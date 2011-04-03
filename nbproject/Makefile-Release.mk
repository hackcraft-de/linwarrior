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
	${OBJECTDIR}/source/cAlert.o \
	${OBJECTDIR}/source/cLandscape.o \
	${OBJECTDIR}/source/rRigged.o \
	${OBJECTDIR}/source/rCollider.o \
	${OBJECTDIR}/source/rWeaponHoming.o \
	${OBJECTDIR}/source/rWeaponPlasmagun.o \
	${OBJECTDIR}/source/cBuilding.o \
	${OBJECTDIR}/source/cWorld.o \
	${OBJECTDIR}/source/cBackground.o \
	${OBJECTDIR}/source/rGrouping.o \
	${OBJECTDIR}/source/cTiming.o \
	${OBJECTDIR}/source/rMobile.o \
	${OBJECTDIR}/source/rComcom.o \
	${OBJECTDIR}/source/cObject.o \
	${OBJECTDIR}/source/rWeapon.o \
	${OBJECTDIR}/source/cSolid.o \
	${OBJECTDIR}/source/rWeaponSparkgun.o \
	${OBJECTDIR}/source/rNameable.o \
	${OBJECTDIR}/source/rTraceable.o \
	${OBJECTDIR}/source/rControlled.o \
	${OBJECTDIR}/source/cTree.o \
	${OBJECTDIR}/_ext/880981645/cMessage.o \
	${OBJECTDIR}/source/rComponent.o \
	${OBJECTDIR}/source/cPlanetmap.o \
	${OBJECTDIR}/source/cPadmap.o \
	${OBJECTDIR}/source/rWeaponExplosion.o \
	${OBJECTDIR}/source/rDamageable.o \
	${OBJECTDIR}/source/rWeaponRaybeam.o \
	${OBJECTDIR}/source/cMission.o \
	${OBJECTDIR}/source/rCamera.o \
	${OBJECTDIR}/source/cMech.o \
	${OBJECTDIR}/source/rWeaponMachinegun.o \
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

${OBJECTDIR}/source/cAlert.o: source/cAlert.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/cAlert.o source/cAlert.cpp

${OBJECTDIR}/source/cLandscape.o: source/cLandscape.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/cLandscape.o source/cLandscape.cpp

${OBJECTDIR}/source/rRigged.o: source/rRigged.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/rRigged.o source/rRigged.cpp

${OBJECTDIR}/source/rCollider.o: source/rCollider.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/rCollider.o source/rCollider.cpp

${OBJECTDIR}/source/rWeaponHoming.o: source/rWeaponHoming.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/rWeaponHoming.o source/rWeaponHoming.cpp

${OBJECTDIR}/source/rWeaponPlasmagun.o: source/rWeaponPlasmagun.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/rWeaponPlasmagun.o source/rWeaponPlasmagun.cpp

${OBJECTDIR}/source/cBuilding.o: source/cBuilding.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/cBuilding.o source/cBuilding.cpp

${OBJECTDIR}/source/cWorld.o: source/cWorld.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/cWorld.o source/cWorld.cpp

${OBJECTDIR}/source/cBackground.o: source/cBackground.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/cBackground.o source/cBackground.cpp

${OBJECTDIR}/source/rGrouping.o: source/rGrouping.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/rGrouping.o source/rGrouping.cpp

${OBJECTDIR}/source/cTiming.o: source/cTiming.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/cTiming.o source/cTiming.cpp

${OBJECTDIR}/source/rMobile.o: source/rMobile.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/rMobile.o source/rMobile.cpp

${OBJECTDIR}/source/rComcom.o: source/rComcom.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/rComcom.o source/rComcom.cpp

${OBJECTDIR}/source/cObject.o: source/cObject.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/cObject.o source/cObject.cpp

${OBJECTDIR}/source/rWeapon.o: source/rWeapon.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/rWeapon.o source/rWeapon.cpp

${OBJECTDIR}/source/cSolid.o: source/cSolid.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/cSolid.o source/cSolid.cpp

${OBJECTDIR}/source/rWeaponSparkgun.o: source/rWeaponSparkgun.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/rWeaponSparkgun.o source/rWeaponSparkgun.cpp

${OBJECTDIR}/source/rNameable.o: source/rNameable.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/rNameable.o source/rNameable.cpp

${OBJECTDIR}/source/rTraceable.o: source/rTraceable.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/rTraceable.o source/rTraceable.cpp

${OBJECTDIR}/source/rControlled.o: source/rControlled.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/rControlled.o source/rControlled.cpp

${OBJECTDIR}/source/cTree.o: source/cTree.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/cTree.o source/cTree.cpp

${OBJECTDIR}/_ext/880981645/cMessage.o: /media/44EA-7693/linux_workspaces/netbeans_workspace/linwarrior/source/cMessage.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/880981645
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/880981645/cMessage.o /media/44EA-7693/linux_workspaces/netbeans_workspace/linwarrior/source/cMessage.cpp

${OBJECTDIR}/source/rComponent.o: source/rComponent.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/rComponent.o source/rComponent.cpp

${OBJECTDIR}/source/cPlanetmap.o: source/cPlanetmap.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/cPlanetmap.o source/cPlanetmap.cpp

${OBJECTDIR}/source/cPadmap.o: source/cPadmap.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/cPadmap.o source/cPadmap.cpp

${OBJECTDIR}/source/rWeaponExplosion.o: source/rWeaponExplosion.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/rWeaponExplosion.o source/rWeaponExplosion.cpp

${OBJECTDIR}/source/rDamageable.o: source/rDamageable.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/rDamageable.o source/rDamageable.cpp

${OBJECTDIR}/source/rWeaponRaybeam.o: source/rWeaponRaybeam.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/rWeaponRaybeam.o source/rWeaponRaybeam.cpp

${OBJECTDIR}/source/cMission.o: source/cMission.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/cMission.o source/cMission.cpp

${OBJECTDIR}/source/rCamera.o: source/rCamera.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/rCamera.o source/rCamera.cpp

${OBJECTDIR}/source/cMech.o: source/cMech.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/cMech.o source/cMech.cpp

${OBJECTDIR}/source/rWeaponMachinegun.o: source/rWeaponMachinegun.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/rWeaponMachinegun.o source/rWeaponMachinegun.cpp

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
