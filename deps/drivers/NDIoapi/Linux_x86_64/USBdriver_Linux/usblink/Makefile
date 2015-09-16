###############################################################################
#
#   Copyright (C) 2005, Northern Digital Inc. All rights reserved.
#
#   Name:       oapi/src/devices/Makefile
#
#   Description:
#
#   Targets:
#       all     - compiles, creates libs, and links the code into binaries
#       clean   - deletes all $(OBJEXT)ect files created by this makefile
#       compile - compiles the specified files, defaults to including debug 
#                   information
#       lib     - generates libraries from compiled $(OBJEXT)ects
#       link    - creates binaries from libraries and compiled $(OBJEXT)ects
#       depends - creates the dependency file for the $(OBJEXT)ects to be compiled
#
#   Command line variables:
#       PROD_REL    - set =1 to compile without debugging information
#       PLATFORM    - set to the target platform that the code should be 
#                       compiled for. Must be of ALLOWEDPLATFORMS.
#
###############################################################################

# define path to root of project
PROJ_DIR=../../..

 # specifies the platforms that this file is able to build for
# PLATFORM must be one of the ALLOWEDPLATFORMS
ALLOWEDPLATFORMS=x86

# include the project environment
include $(PROJ_DIR)/make/prjenv.mak

# cause the compiler to generate the correct linkage (no @size decoration)
CFLAGS_PRJ+=/D_NO_STDCALL

# a binary file to be created
DLL1=$(BIN_DIR)/usblink.$(DLLEXT)


DLL1_SRCS=$(wildcard *.c)
DLL1_LIBS=FTD2XX.lib $(GENLIB_LIB) $(NDSERVICES_LIB)

LFLAGS_MAK+=/DEFAULTLIB:user32 /DEFAULTLIB:ws2_32

# include the project rules
include $(PROJ_DIR)/make/prjrules.mak




