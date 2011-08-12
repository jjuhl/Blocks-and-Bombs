cmake_minimum_required(VERSION 2.6)
project(BLOCKS_AND_BOMBS)

# Default to Debug build if not told otherwise with eg. '-D CMAKE_BUILD_TYPE=Release'
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif(NOT CMAKE_BUILD_TYPE)

find_package(SDL REQUIRED)
include_directories(${SDL_INCLUDE_DIR})

find_package(SDL_image REQUIRED)
include_directories(${SDLIMAGE_INCLUDE_DIR})

find_package(SDL_ttf REQUIRED)
include_directories(${SDLTTF_INCLUDE_DIR})

find_program(ETAGS NAMES etags etags.emacs)

# Extra flags for GCC
if(CMAKE_COMPILER_IS_GNUCC)
  add_definitions(-Wall -Wextra -Wabi -Wctor-dtor-privacy -Wnon-virtual-dtor -Wreorder -Weffc++ -Wold-style-cast -Wsign-promo -Wcast-qual)
  set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3 -ggdb")
  set(CMAKE_SHARED_LINKER_FLAGS "-O 1")
endif(CMAKE_COMPILER_IS_GNUCC)

# The C++ source code
set(SOURCES
  main.cc
  except.cc
  options.cc
  util.cc
  bbengine.cc
  menustate.cc
  playstate.cc
  helpstate.cc
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
  COMMAND mv TAGS ${CMAKE_SOURCE_DIR}
  )

link_libraries(
   ${SDL_LIBRARY}
   ${SDLIMAGE_LIBRARY}
   ${SDLTTF_LIBRARY}
   )

add_executable(
   blocks-and-bombs
   WIN32 # Avoid DOS prompt to appear in Windows
   ${SOURCES}
   )

mark_as_advanced(
  ETAGS
)
