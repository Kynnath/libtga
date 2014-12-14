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
CND_PLATFORM=MinGW-Windows
CND_DLIB_EXT=dll
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/TGA/Image.o \
	${OBJECTDIR}/src/TGA/tga.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-m64 -Wfatal-errors -Wall -Wextra -pedantic -Winit-self -Wmissing-include-dirs -Wswitch-default -Wswitch-enum -Wswitch-default -Wfloat-equal -Wshadow -Wcast-qual -Wcast-align -Wwrite-strings -Wconversion -Wsign-conversion -Wlogical-op -Wmissing-declarations -Wmissing-noreturn -Wmissing-format-attribute -Wpacked -Wredundant-decls -Wunreachable-code -Winvalid-pch -Wvolatile-register-var -Wdisabled-optimization -Wstack-protector -std=c++1y
CXXFLAGS=-m64 -Wfatal-errors -Wall -Wextra -pedantic -Winit-self -Wmissing-include-dirs -Wswitch-default -Wswitch-enum -Wswitch-default -Wfloat-equal -Wshadow -Wcast-qual -Wcast-align -Wwrite-strings -Wconversion -Wsign-conversion -Wlogical-op -Wmissing-declarations -Wmissing-noreturn -Wmissing-format-attribute -Wpacked -Wredundant-decls -Wunreachable-code -Winvalid-pch -Wvolatile-register-var -Wdisabled-optimization -Wstack-protector -std=c++1y

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/liblibtga.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/liblibtga.a: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/liblibtga.a
	${AR} -rv ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/liblibtga.a ${OBJECTFILES} 
	$(RANLIB) ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/liblibtga.a

${OBJECTDIR}/src/TGA/Image.o: src/TGA/Image.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/TGA
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../libtools/src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/TGA/Image.o src/TGA/Image.cpp

${OBJECTDIR}/src/TGA/tga.o: src/TGA/tga.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/TGA
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../libtools/src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/TGA/tga.o src/TGA/tga.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/liblibtga.a

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
