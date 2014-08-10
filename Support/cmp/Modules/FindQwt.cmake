
# - Find QWT library
# - Derived from the FindQwt.cmake that is included with cmake
# Find the native QWT includes and library
# This module defines
#  QWT_INCLUDE_DIR, where to find qwt.h, etc.
#  QWT_LIBRARIES, libraries to link against to use QWT.
#  QWT_FOUND, If false, do not try to use QWT.
# also defined, but not for general use are
#  QWT_LIBRARY, where to find the QWT library.
#  QWT_LIBRARY_DEBUG - Debug version of qwt library
#  QWT_LIBRARY_RELEASE - Release Version of qwt library


set(QWT_CMAKE_DEBUG 0)
if(QWT_CMAKE_DEBUG)
    MESSAGE (STATUS "Finding Qwt library and headers..." )
endif()

# Only set QWT_INSTALL to the environment variable if it is blank
if("${QWT_INSTALL}" STREQUAL "")
  set(QWT_INSTALL  $ENV{QWT_INSTALL})
endif()

# Look for the header file.
SET(QWT_INCLUDE_SEARCH_DIRS
  ${QWT_INSTALL}/include/qwt
  ${QWT_INSTALL}/include/qwt5
  ${QWT_INSTALL}/include
  /usr/include/qwt5
  ${QWT_INSTALL}/lib/qwt.framework/Headers
)

set(QWT_LIB_SEARCH_DIRS
  ${QWT_INSTALL}/lib
  ${QWT_INSTALL}/lib/qwt.framework/
  )

set(QWT_BIN_SEARCH_DIRS
  ${QWT_INSTALL}/bin
)

FIND_PATH(QWT_INCLUDE_DIR
  NAMES qwt.h
  PATHS ${QWT_INCLUDE_SEARCH_DIRS}
)

if(WIN32 AND NOT MINGW)
    set(QWT_SEARCH_DEBUG_NAMES "qwt_debug;libqwt_debug;qwtd")
    set(QWT_SEARCH_RELEASE_NAMES "qwt;libqwt")
ELSE (WIN32 AND NOT MINGW)
    set(QWT_SEARCH_DEBUG_NAMES "qwt_debug")
    set(QWT_SEARCH_RELEASE_NAMES "qwt")
ENDif(WIN32 AND NOT MINGW)

# Look for the library.
FIND_LIBRARY(QWT_LIBRARY_DEBUG
  NAMES ${QWT_SEARCH_DEBUG_NAMES}
  PATHS ${QWT_LIB_SEARCH_DIRS}
  NO_DEFAULT_PATH
)

FIND_LIBRARY(QWT_LIBRARY_RELEASE
  NAMES ${QWT_SEARCH_RELEASE_NAMES}
  PATHS ${QWT_LIB_SEARCH_DIRS}
  NO_DEFAULT_PATH
)

if( QWT_CMAKE_DEBUG )
  message(STATUS "QWT_INCLUDE_SEARCH_DIRS: ${QWT_INCLUDE_SEARCH_DIRS}")
  message(STATUS "QWT_LIB_SEARCH_DIRS: ${QWT_LIB_SEARCH_DIRS}")
  message(STATUS "QWT_BIN_SEARCH_DIRS: ${QWT_BIN_SEARCH_DIRS}")
  message(STATUS "QWT_SEARCH_DEBUG_NAMES: ${QWT_SEARCH_DEBUG_NAMES}")
  message(STATUS "QWT_SEARCH_RELEASE_NAMES: ${QWT_SEARCH_RELEASE_NAMES}")

  MESSAGE(STATUS "QWT_INCLUDE_DIR: ${QWT_INCLUDE_DIR}")
  MESSAGE(STATUS "QWT_LIBRARY_DEBUG: ${QWT_LIBRARY_DEBUG}")
  MESSAGE(STATUS "QWT_LIBRARY_RELEASE: ${QWT_LIBRARY_RELEASE}")
  MESSAGE(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
endif()

# include the macro to adjust libraries
INCLUDE (${CMP_MODULES_SOURCE_DIR}/cmpAdjustLibVars.cmake)
cmp_ADJUST_LIB_VARS(QWT)

if(QWT_INCLUDE_DIR AND QWT_LIBRARY)
  SET(QWT_FOUND 1)
  SET(QWT_LIBRARIES ${QWT_LIBRARY})
  SET(QWT_INCLUDE_DIRS ${QWT_INCLUDE_DIR})
  if(QWT_LIBRARY_DEBUG)
    GET_FILENAME_COMPONENT(QWT_LIBRARY_PATH ${QWT_LIBRARY_DEBUG} PATH)
    SET(QWT_LIB_DIR  ${QWT_LIBRARY_PATH})
  ELSEif(QWT_LIBRARY_RELEASE)
    GET_FILENAME_COMPONENT(QWT_LIBRARY_PATH ${QWT_LIBRARY_RELEASE} PATH)
    SET(QWT_LIB_DIR  ${QWT_LIBRARY_PATH})
  ENDif(QWT_LIBRARY_DEBUG)

  if(QWT_DUMP_PROG)
    GET_FILENAME_COMPONENT(QWT_BIN_PATH ${QWT_DUMP_PROG} PATH)
    SET(QWT_BIN_DIR  ${QWT_BIN_PATH})
  endif(QWT_DUMP_PROG)
ELSE(QWT_INCLUDE_DIR AND QWT_LIBRARY)
  SET(QWT_FOUND 0)
  SET(QWT_LIBRARIES)
  SET(QWT_INCLUDE_DIRS)
ENDif(QWT_INCLUDE_DIR AND QWT_LIBRARY)

# Report the results.
if(NOT QWT_FOUND)
  SET(QWT_DIR_MESSAGE
    "Qwt was not found. Make sure QWT_LIBRARY and QWT_INCLUDE_DIR are set or set the QWT_INSTALL environment variable.")
  if(NOT QWT_FIND_QUIETLY)
    MESSAGE(STATUS "${QWT_DIR_MESSAGE}")
  ELSE(NOT QWT_FIND_QUIETLY)
    if(QWT_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Qwt was NOT found and is Required by this project")
    ENDif(QWT_FIND_REQUIRED)
  ENDif(NOT QWT_FIND_QUIETLY)
ENDif(NOT QWT_FOUND)

if(QWT_FOUND)
  message(STATUS "Found Qwt Library ${QWT_INSTALL}")
  #############################################
  # Find out if QWT was build using dll's
  #############################################
  if(WIN32)
    if(NOT ${QWT_LIBRARY_DLL_DEBUG} STREQUAL "" OR NOT ${QWT_LIBRARY_DLL_RELEASE} STREQUAL "")
      set(QWT_IS_SHARED 1 CACHE INTERNAL "Qwt Built as DLL or Shared Library")
    endif()   
  endif()

  if(NOT WIN32) # Apple and Linux
    GET_FILENAME_COMPONENT(QWT_LIB_EXT ${QWT_LIBRARY_DEBUG} EXT)
    if(${QWT_LIB_EXT} STREQUAL ".dylib" OR ${QWT_LIB_EXT} STREQUAL ".so")
      set(QWT_IS_SHARED 1 CACHE INTERNAL "Qwt Built as DLL or Shared Library")
    endif()
  endif()

  #
  #############################################
endif()
