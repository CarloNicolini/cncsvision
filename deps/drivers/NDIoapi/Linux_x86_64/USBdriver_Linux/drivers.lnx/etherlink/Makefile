###############################################################################
#
#   Copyright (C) 2005, Northern Digital Inc. All rights reserved.
#
#   Name:       oapi/src/drivers.lnx/etherlink/Makefile
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

# include the project environment
include $(PROJ_DIR)/make/prjenv.mak

DLL1_SRCS=\
	etherlink.c

DLL1=$(LIB_DIR)/etherlink.$(DLLEXT)

# include the project rules
include $(PROJ_DIR)/make/prjrules.mak
