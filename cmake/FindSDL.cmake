# Find the SDL includes and library
#
# This module defines
# SDL_INCLUDE_DIR
# SDL_LIBRARIES
# SDL_FOUND

### FIND SDL ON  LINUX
# Download SDL from https://www.SDL.org, then unzip
# ./configure; make; sudo make install

if(UNIX)
    find_path(SDL_INCLUDE_DIR
    NAMES 
      SDL.h
    PATHS
        /usr/local/include/SDL2
        /usr/include)
    find_library(SDL_LIBRARIES
    NAMES
        libSDL2
        libSDL2.a
    PATHS
        /usr/local/lib)
endif(UNIX)

if(WIN32)
message(STATUS "Looking for SDL on Windows")
    find_path(SDL_INCLUDE_DIR
    NAMES 
      SDL.h
    PATHS
        "C:/Program Files/SDL2-2.0.3/include/")
      
    find_library(SDL_LIBRARIES
    NAMES
        SDL2
    PATHS
        "C:/Program Files/SDL2-2.0.3/lib/x64/")
endif(WIN32)

if(SDL_LIBRARIES)
    message(STATUS "Found SDL:")
    message(STATUS " - Includes: ${SDL_INCLUDE_DIR}")
    message(STATUS " - Libraries: ${SDL_LIBRARIES}")
    set(SDL_FOUND True)
endif(SDL_LIBRARIES)