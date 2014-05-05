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
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/mo/Assets.o \
	${OBJECTDIR}/mo/Mesh.o \
	${OBJECTDIR}/mo/Model.o \
	${OBJECTDIR}/mo/Renderer.o \
	${OBJECTDIR}/mo/Texture2D.o


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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libmo.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libmo.a: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libmo.a
	${AR} -rv ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libmo.a ${OBJECTFILES} 
	$(RANLIB) ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libmo.a

${OBJECTDIR}/mo/Assets.o: mo/Assets.cpp 
	${MKDIR} -p ${OBJECTDIR}/mo
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../glm -I../lodepng -I../ogli -I../glew-1.9.0/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/mo/Assets.o mo/Assets.cpp

${OBJECTDIR}/mo/Mesh.o: mo/Mesh.cpp 
	${MKDIR} -p ${OBJECTDIR}/mo
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../glm -I../lodepng -I../ogli -I../glew-1.9.0/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/mo/Mesh.o mo/Mesh.cpp

${OBJECTDIR}/mo/Model.o: mo/Model.cpp 
	${MKDIR} -p ${OBJECTDIR}/mo
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../glm -I../lodepng -I../ogli -I../glew-1.9.0/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/mo/Model.o mo/Model.cpp

${OBJECTDIR}/mo/Renderer.o: mo/Renderer.cpp 
	${MKDIR} -p ${OBJECTDIR}/mo
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../glm -I../lodepng -I../ogli -I../glew-1.9.0/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/mo/Renderer.o mo/Renderer.cpp

${OBJECTDIR}/mo/Texture2D.o: mo/Texture2D.cpp 
	${MKDIR} -p ${OBJECTDIR}/mo
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../glm -I../lodepng -I../ogli -I../glew-1.9.0/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/mo/Texture2D.o mo/Texture2D.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libmo.a

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
