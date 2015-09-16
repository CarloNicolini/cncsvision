#-----------------------------------------------------------------------------
# SAMPLES.MAK - makefile for the 32-bit console versions of the sample
#   programs linked to OAPI.DLL
#
#   Uncomment the COMP line for your compiler in the Compilers section.
#
# For Borland C++:
#   Change the BC_LIB_DIR line to your compiler's standard library path
#   Use 'make -fsamples.mak' to build all of the sample programs.
#   Use 'make -fsamples.mak sampleX.exe' to build a specific sample
#       program, where 'X' is the index number of the sample program
#       (e.g., sample5.exe for the fifth program).
#
# For Microsoft C++:
#   Use 'nmake /f samples.mak' to build all of the sample programs.
#   Use 'nmake /f samples.mak sampleX.exe' to build a specific sample
#       program, where 'X' is the index number of the sample program
#       (e.g., sample5.exe for the fifth program); use 'name /f
#       samples.mak sleep.obj' first to build sleep.obj if necessary.
#
# For Watcom C++
#   Use 'wmake /ms /f samples.mak' to build all of the sample programs.
#   Use 'wmake /ms /f samples.mak sampleX.exe' to build a specific sample
#       program, where 'X' is the index number of the sample program
#       (e.g., sample5.exe for the fifth program).
#
# For nonsupported compilers:
#   First, make an export library for linking the OPTOTRAK Application
#   Programmer's Interface DLL Oapi.dll. Use the module definition file
#   OAPI.DEF in the NDLIB/GENERIC subdirectory for a listing of the exported
#   functions. Use the supported   compiler macro definitions in the
#   Compilation and Linking sections as a guide for your own compiler.
#   You may also need to change the directives in the Targets section.
#
#   Northern Digital, Inc. - 2005
#-----------------------------------------------------------------------------


#-----------------------------------------------------------------------------
# Compilers
#-----------------------------------------------------------------------------
# COMP = BC5
COMP = MSVC
# COMP = WC11

#-----------------------------------------------------------------------------
# Directories
#-----------------------------------------------------------------------------
SAMPLES_DIR = .
NDLIB_DIR   = ..\ndlib
INCLUDE_DIR = $(NDLIB_DIR)\include

# For BC users, set BC_LIB_DIR to your compiler library path.
!IF "$(COMP)" == "BC5"
BC_LIB_DIR = Q:\COMP\BC5\LIB
!ENDIF

#-----------------------------------------------------------------------------
# Compilation
#-----------------------------------------------------------------------------
!IF "$(COMP)" == "BC5"
CC      = Bcc32
CPFLAGS = -DWIN32 -D_CONSOLE
CFLAGS  = -c -w -v- -Od -I$(INCLUDE_DIR)
!ELSE IF "$(COMP)" == "MSVC"
CC      = cl
CPFLAGS = -DWIN32 -DSTRICT -D_CONSOLE
CFLAGS  = -c -DNDEBUG -ML -Gd -Od -W3 -Zp -I$(INCLUDE_DIR)
!ELSE IF "$(COMP)" == "WC11"
CC      = wcc386
CPFLAGS = -DWIN32 -D_CONSOLE
CFLAGS  = -bt=nt -i=$(INCLUDE_DIR) -4r -w4 -e25 -zq -od -d0 -mf -zp4
!ELSE
!ERROR A supported compiler has not been specified
# Add your alternate compiler macro definitions here and remove the preceeding
# ERROR line for use of unsupported compilers.
!ENDIF

#-----------------------------------------------------------------------------
# Linking
#-----------------------------------------------------------------------------
DLLNAME = OAPI

!if "$(COMP)" == "BC5"
LINKER    = TLink32
LINKFLAGS = /v- /j$(BC_LIB_DIR) /Tpe /ap /c /x
LIBS      = $(NDLIB_DIR)\$(COMP)\$(DLLNAME).lib c0x32.obj import32.lib cw32.lib
!ELSE IF "$(COMP)" == "MSVC"
LINKER    = link
LINKFLAGS = /SUBSYSTEM:console
LIBS      = $(NDLIB_DIR)\$(COMP)\$(DLLNAME).lib sleep.obj /DEFAULTLIB:user32.lib \
            gdi32.lib winmm.lib comdlg32.lib comctl32.lib
!ELSE IF "$(COMP)" == "WC11"
LINKER    = wlink
LINKFLAGS = SYS nt op maxe=10 op q file
LIBS      = lib $(NDLIB_DIR)\$(COMP)\$(DLLNAME).lib
!ELSE
# add your alternate linker macro definitions here
!ENDIF

OT_OBJS  = ot_aux.obj
certus_OBJS = certus_aux.obj

#-----------------------------------------------------------------------------
# Targets
#-----------------------------------------------------------------------------
PROGS=sample1.exe  sample2.exe  sample3.exe  sample4.exe  sample5.exe  \
      sample6.exe  sample7.exe  sample8.exe  sample9.exe  sample10.exe \
      sample11.exe sample12.exe sample13.exe sample14.exe sample15.exe \
      sample16.exe sample17.exe sample18.exe sample19.exe sample20.exe \
      sample21.exe sample22.exe \
      certus_sample.exe \
      certus_sample1.exe  certus_sample2.exe  certus_sample3.exe  certus_sample4.exe  \
      certus_sample11.exe certus_sample12.exe \
      certus_sample13.exe certus_sample14.exe certus_sample15.exe \
      certus_sample16.exe certus_sample17.exe certus_sample18.exe certus_sample19.exe \
      optsetup.exe download.exe register.exe align.exe 

!IF "$(COMP)" == "MSVC"
SLEEP = sleep.obj
all: $(SLEEP) $(PROGS)
!ELSE
all: $(PROGS)
!ENDIF

optsetup.exe : optsetup.obj $(SLEEP)
    $(LINKER) $(LINKFLAGS) optsetup.obj $(LIBS)

download.exe : download.obj $(SLEEP)
    $(LINKER) $(LINKFLAGS) download.obj $(LIBS)

register.exe : register.obj 
    $(LINKER) $(LINKFLAGS) register.obj $(LIBS)

align.exe : align.obj 
    $(LINKER) $(LINKFLAGS) align.obj $(LIBS)

sample1.exe : sample1.obj $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) sample1.obj $(OT_OBJS) $(LIBS)

sample2.exe : sample2.obj $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) sample2.obj $(OT_OBJS) $(LIBS)

sample3.exe : sample3.obj $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) sample3.obj $(OT_OBJS) $(LIBS)

sample4.exe : sample4.obj $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) sample4.obj $(OT_OBJS) $(LIBS)

sample5.exe : sample5.obj $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) sample5.obj $(OT_OBJS) $(LIBS)

sample6.exe : sample6.obj $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) sample6.obj $(OT_OBJS) $(LIBS)

sample7.exe : sample7.obj $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) sample7.obj $(OT_OBJS) $(LIBS)

sample8.exe : sample8.obj $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) sample8.obj $(OT_OBJS) $(LIBS)

sample9.exe : sample9.obj $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) sample9.obj $(OT_OBJS) $(LIBS)

sample10.exe : sample10.obj $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) sample10.obj $(OT_OBJS) $(LIBS)

sample11.exe : sample11.obj $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) sample11.obj $(OT_OBJS) $(LIBS)

sample12.exe : sample12.obj $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) sample12.obj $(OT_OBJS) $(LIBS)

sample13.exe : sample13.obj $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) sample13.obj $(OT_OBJS) $(LIBS)

sample14.exe : sample14.obj $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) sample14.obj $(OT_OBJS) $(LIBS)

sample15.exe : sample15.obj $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) sample15.obj $(OT_OBJS) $(LIBS)

sample16.exe : sample16.obj $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) sample16.obj $(OT_OBJS) $(LIBS)

sample17.exe : sample17.obj $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) sample17.obj $(OT_OBJS) $(LIBS)

sample18.exe : sample18.obj $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) sample18.obj $(OT_OBJS) $(LIBS)

sample19.exe : sample19.obj $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) sample19.obj $(OT_OBJS) $(LIBS)

sample20.exe : sample20.obj $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) sample20.obj $(OT_OBJS) $(LIBS)

sample21.exe : sample21.obj $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) sample21.obj $(OT_OBJS) $(LIBS)

sample22.exe : sample22.obj $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) sample22.obj $(OT_OBJS) $(LIBS)

certus_sample.exe : certus_sample.obj $(certus_OBJS) $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) certus_sample.obj $(certus_OBJS) $(OT_OBJS) $(LIBS)

certus_sample1.exe : certus_sample1.obj $(certus_OBJS) $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) certus_sample1.obj $(certus_OBJS) $(OT_OBJS) $(LIBS)

certus_sample2.exe : certus_sample2.obj $(certus_OBJS) $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) certus_sample2.obj $(certus_OBJS) $(OT_OBJS) $(LIBS)

certus_sample3.exe : certus_sample3.obj $(certus_OBJS) $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) certus_sample3.obj $(certus_OBJS) $(OT_OBJS) $(LIBS)

certus_sample4.exe : certus_sample4.obj $(certus_OBJS) $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) certus_sample4.obj $(certus_OBJS) $(OT_OBJS) $(LIBS)

certus_sample5.exe : certus_sample5.obj $(certus_OBJS) $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) certus_sample5.obj $(certus_OBJS) $(OT_OBJS) $(LIBS)

certus_sample6.exe : certus_sample6.obj $(certus_OBJS) $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) certus_sample6.obj $(certus_OBJS) $(OT_OBJS) $(LIBS)

certus_sample7.exe : certus_sample7.obj $(certus_OBJS) $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) certus_sample7.obj $(certus_OBJS) $(OT_OBJS) $(LIBS)

certus_sample8.exe : certus_sample8.obj $(certus_OBJS) $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) certus_sample8.obj $(certus_OBJS) $(OT_OBJS) $(LIBS)

certus_sample9.exe : certus_sample9.obj $(certus_OBJS) $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) certus_sample9.obj $(certus_OBJS) $(OT_OBJS) $(LIBS)

certus_sample10.exe : certus_sample10.obj $(certus_OBJS) $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) certus_sample10.obj $(certus_OBJS) $(OT_OBJS) $(LIBS)

certus_sample11.exe : certus_sample11.obj $(certus_OBJS) $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) certus_sample11.obj $(certus_OBJS) $(OT_OBJS) $(LIBS)

certus_sample12.exe : certus_sample12.obj $(certus_OBJS) $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) certus_sample12.obj $(certus_OBJS) $(OT_OBJS) $(LIBS)

certus_sample13.exe : certus_sample13.obj $(certus_OBJS) $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) certus_sample13.obj $(certus_OBJS) $(OT_OBJS) $(LIBS)

certus_sample14.exe : certus_sample14.obj $(certus_OBJS) $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) certus_sample14.obj $(certus_OBJS) $(OT_OBJS) $(LIBS)

certus_sample15.exe : certus_sample15.obj $(certus_OBJS) $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) certus_sample15.obj $(certus_OBJS) $(OT_OBJS) $(LIBS)

certus_sample16.exe : certus_sample16.obj $(certus_OBJS) $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) certus_sample16.obj $(certus_OBJS) $(OT_OBJS) $(LIBS)

certus_sample17.exe : certus_sample17.obj $(certus_OBJS) $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) certus_sample17.obj $(certus_OBJS) $(OT_OBJS) $(LIBS)

certus_sample18.exe : certus_sample18.obj $(certus_OBJS) $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) certus_sample18.obj $(certus_OBJS) $(OT_OBJS) $(LIBS)

certus_sample19.exe : certus_sample19.obj $(certus_OBJS) $(OT_OBJS) $(SLEEP)
    $(LINKER) $(LINKFLAGS) certus_sample19.obj $(certus_OBJS) $(OT_OBJS) $(LIBS)

{$(SAMPLES_DIR)}.c{}.obj:
    $(CC) $(CFLAGS) $(CPFLAGS) $(SAMPLES_DIR)\$*.c

clean:
    -@erase "*.obj"

cleanall: clean
    -@erase "*.exe"
