cmake_minimum_required(VERSION 2.6)
project(BLOCKS_AND_BOMBS)

# Default to Debug build if not told otherwise with eg. '-D CMAKE_BUILD_TYPE=Release'
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif(NOT CMAKE_BUILD_TYPE)

# Set this to be a development build. Resources will be found relative to the source dir.
set(DEVELOPMENT_BUILD ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(SDL REQUIRED)
include_directories(${SDL_INCLUDE_DIR})

find_package(SDL_image REQUIRED)
include_directories(${SDLIMAGE_INCLUDE_DIR})

find_package(SDL_ttf REQUIRED)
include_directories(${SDLTTF_INCLUDE_DIR})

find_package(LibConfig++ REQUIRED)
include_directories(${LIBCONFIG++_INCLUDE_DIR})

find_program(ETAGS NAMES etags etags.emacs)

# Extra flags for GCC
if(CMAKE_COMPILER_IS_GNUCC)
  add_definitions(-Wall -Wextra -Wabi -Wctor-dtor-privacy -Wnon-virtual-dtor -Wreorder -Weffc++ -Wold-style-cast -Wsign-promo -Wcast-qual)
endif(CMAKE_COMPILER_IS_GNUCC)

include_directories(${CMAKE_BINARY_DIR}/)

# The C++ source code
set(SOURCES
  main.cc
  except.cc
  util.cc
  bbengine.cc
  menustate.cc
  playstate.cc
  helpstate.cc
  textdisplaystate.cc
  highscorestate.cc
  textwriter.cc
  resources.cc
  effects.cc
  )

if(WIN32 AND NOT UNIX)
  # We do not care about Microsofts secure implementations of standard library functions
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif(WIN32 AND NOT UNIX)

# Create etags for emacs
add_custom_target(etags
  COMMAND ${ETAGS} --declarations --ignore-indentation ${CMAKE_SOURCE_DIR}/*
  COMMAND mv -f TAGS ${CMAKE_SOURCE_DIR}
  )

link_libraries(
   ${SDL_LIBRARY}
   ${SDLIMAGE_LIBRARY}
   ${SDLTTF_LIBRARY}
   ${LIBCONFIG_LIBRARY}
   )

add_executable(
   blocks-and-bombs
   WIN32 # Avoid DOS prompt to appear in Windows
   ${SOURCES}
   )

# Installation rules
if(WIN32 AND NOT UNIX)

  set(INSTALL_SUBDIR_BIN "." CACHE STRING "Installation subdir for binaries")
  set(INSTALL_SUBDIR_SHARE "resources" CACHE STRING "Installation subdir for resources")
  set(INSTALL_SUBDIR_DOC "." CACHE STRING "Installation subdir for docs")

  # We need to ship the requred dll's for Windows
  install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/SDL.dll
                ${CMAKE_CURRENT_SOURCE_DIR}/SDL_image.dll
                ${CMAKE_CURRENT_SOURCE_DIR}/SDL_ttf.dll DESTINATION ${INSTALL_SUBDIR_BIN})

else(WIN32 AND NOT UNIX)

  set(INSTALL_SUBDIR_BIN "bin" CACHE STRING "Installation subdir for binaries")
  set(INSTALL_SUBDIR_SHARE "share/blocks-and-bombs" CACHE STRING "Installation subdir for resources")
  set(INSTALL_SUBDIR_DOC "share/doc/blocks-and-bombs" CACHE STRING "Installation subdir for docs")

endif(WIN32 AND NOT UNIX)

if(DEVELOPMENT_BUILD)
  set(RESOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources/)
else(DEVELOPMENT_BUILD)
  set(RESOURCES_DIR ${CMAKE_INSTALL_PREFIX}/${INSTALL_SUBDIR_SHARE}/)
endif(DEVELOPMENT_BUILD)

install(TARGETS blocks-and-bombs DESTINATION ${INSTALL_SUBDIR_BIN})

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/README
  ${CMAKE_CURRENT_SOURCE_DIR}/COPYING
  ${CMAKE_CURRENT_SOURCE_DIR}/TODO
  DESTINATION ${INSTALL_SUBDIR_DOC})

install(DIRECTORY
  resources/images
  resources/fonts
  resources/levels
  resources/animations
  DESTINATION ${RESOURCES_DIR})

## Create config.h
configure_file(config.h.in ${CMAKE_BINARY_DIR}/config.h)

mark_as_advanced(
  ETAGS
)
