# Base framework of the CADDIES project. 

# This framework is used to indentify multiple implementations of the
# CAAPI and to compile the available applications using a specifily
# selected CAAPI implementation.

########################## PRE-SET ##########################

# Sometime, Visual Studio Compiler fails the C/C++ tests with the
# error "C1050: compiler is out of heap space". A possible solution is
# to remove the option '/Zm1000' from the flags. Thus, the following
# two lines should remove the specific flags when CMake is configured
# multiple time and thus make Visual Studio pass the tests.

if (WIN32)
  if(CMAKE_C_FLAGS) 
    string (REPLACE "/Zm1000" " " CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
  endif()

  if(CMAKE_CXX_FLAGS) 
    string (REPLACE "/Zm1000" " " CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  endif()
endif()


########################## START ##########################

# Version of CMake that this project needs.
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

cmake_policy(SET CMP0054 NEW)


# A variable which controls the type of build generated
# Options are:
#   Debug Release RelWithDebInfo MinSizeRel
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING  
    "Set the type of build generated. Options are:  Debug Release RelWithDebInfo MinSizeRel")
endif()

# Project name.
project(CADDIES)
message(STATUS "Project name: ${PROJECT_NAME}")

# Project build
message(STATUS "Project build type: ${CMAKE_BUILD_TYPE}")

# Set CADDIES source dir
set(CAAPI_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

set(CAAPI_BASE_VERSION "0")
# Read the version of the CAAPI base framework.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/CAAPIBaseVersion.txt" CAAPI_BASE_VERSION)

message(STATUS "CAAPI base framework version: ${CAAPI_BASE_VERSION}")

# Include the special modules
include("${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/CAAPIAddSrcDir.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/CAAPIFindImpls.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/SubDirList.cmake")

########################## CONFIGURATION PRECISION ##########################

# Allow the developer to chose the specific precision of the real type.
set(CAAPI_REAL_PRECISION "float" CACHE STRING "The precision of the Real type: float | double" )

#DEFINITION
string( TOLOWER "${CAAPI_REAL_PRECISION}" caapi_real_precision )

if( caapi_real_precision STREQUAL "float" )
  if (WIN32)
    add_definitions("/DCA_REAL_PRECISION=0")
  else()
    add_definitions("-DCA_REAL_PRECISION=0")
  endif()
  message(STATUS "Real precision: float")
elseif( caapi_real_precision STREQUAL "double" )
  if (WIN32)
    add_definitions("/DCA_REAL_PRECISION=1")
  else()
    add_definitions("-DCA_REAL_PRECISION=1")
  endif()
  message(STATUS "Real precision: double")
endif()

########################## C++11 ##########################

message(STATUS "COMPILER = ${CMAKE_CXX_COMPILER_ID}")
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  message(STATUS "C++11 activated.")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -stdlib=libc++")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  # using GCC
   execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
   if (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7)
        message(STATUS "C++11 activated.")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
   elseif(GCC_VERSION VERSION_GREATER 4.3 OR GCC_VERSION VERSION_EQUAL 4.3)
        message(STATUS "C++0x activated. If you get any errors update to a compiler which fully supports C++11")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")
   else ()
        message(FATAL_ERROR "C++11 needed. Therefore a gcc compiler with a version higher than 4.3 is needed.")   
   endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")

endif()


########################## SOURCES COMMON ##########################

# Set source files
set(CAAPI_HEADERS)
set(CAAPI_SOURCES)
set(CAAPI_CACOMMON)
set(CAAPI_CAHEADERS)


set(CAAPI_COMMON_FOLDER "/impls/common/")

# From common directory
CAAPI_add_source_dir(impls\\\\common      "${CMAKE_CURRENT_SOURCE_DIR}${CAAPI_COMMON_FOLDER}*.hpp" CAAPI_HEADERS)
CAAPI_add_source_dir(impls\\\\common      "${CMAKE_CURRENT_SOURCE_DIR}${CAAPI_COMMON_FOLDER}*.cpp" CAAPI_SOURCES)
CAAPI_add_source_dir(impls\\\\common      "${CMAKE_CURRENT_SOURCE_DIR}${CAAPI_COMMON_FOLDER}*.ca" CAAPI_CACOMMON)

# Add the needed directories into the compilation
include_directories("${CMAKE_CURRENT_SOURCE_DIR}${CAAPI_COMMON_FOLDER}")

########################## INSTALL  ##########################

# This part is needed to have the right files into the base package 
### ATTENTION!!!!! -- Remeber to add new files here!!!!

# Set the name of the component
set(CAAPI_BASE_COMPONENT_NAME "base-${CAAPI_BASE_VERSION}")

# Add top directory files
install(FILES 
  CMakeLists.txt 
  AUTHORS
  CAAPIBaseVersion.txt
  LICENSE  
  NEWS  
  README  
  DESTINATION ./  
  COMPONENT ${CAAPI_BASE_COMPONENT_NAME})

# Add common implementation files
install(FILES 
  ${CAAPI_HEADERS}
  ${CAAPI_SOURCES}
  ${CAAPI_CACOMMON}
  DESTINATION ./impls/common/  
  COMPONENT ${CAAPI_BASE_COMPONENT_NAME})

# Add CMakeModules files
install(FILES 
  ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/CAAPIFindImpls.cmake
  ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/CAAPIAddSrcDir.cmake
  ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/SubDirList.cmake
  DESTINATION ./CMakeModules/  
  COMPONENT ${CAAPI_BASE_COMPONENT_NAME})

# Add base app files
install(FILES 
  ${CMAKE_CURRENT_SOURCE_DIR}/apps/README
  ${CMAKE_CURRENT_SOURCE_DIR}/apps/CMakeLists.txt
  DESTINATION ./apps/  
  COMPONENT ${CAAPI_BASE_COMPONENT_NAME})

########################## PRE PACKAGE  ##########################

include (InstallRequiredSystemLibraries)

# Set the generators
set(CPACK_GENERATOR         ZIP)
set(CPACK_SOURCE_GENERATOR  ZIP)

set(CPACK_PACKAGE_NAME "CADDIES")
set(CPACK_PACKAGE_CONTACT "m.guidolin@exeter.ac.uk")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "CADDIES Framework")
set(CPACK_PACKAGE_VENDOR "CWS: Centre for Water Systems, University of Exeter")
set(CPACK_RESOURCE_FILE_README  "${CMAKE_CURRENT_SOURCE_DIR}/README")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION "${CAAPI_BASE_VERSION}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "CAAPI")
set(CPACK_PACKAGE_FILE_NAME  "${CPACK_PACKAGE_NAME}")
set(CPACK_COMPONENT_INCLUDE_TOPLEVEL_DIRECTORY ON)

# We want the source packege to be empty in order to be sure that is
# not created by mistake.
set(CPACK_SOURCE_IGNORE_FILES "/*/")

# Activate components and create one package per component
set(CPACK_COMPONENTS_IGNORE_GROUPS 1)

# Withoutr this, only a single archive would be generated.
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)

# Include all the macros used to generate packages.
include(CPack)

# Create the various groups
cpack_add_component_group(framework 
  DISPLAY_NAME "Framework" 
  DESCRIPTION "Installs the CAAPI framework needed to create CAAPI applications")
cpack_add_component_group(impls 
  DISPLAY_NAME "CAAPI-Impls"      
  DESCRIPTION "Installs the available implementations of CAAPI")
cpack_add_component_group(apps 
  DISPLAY_NAME "Apps"      
  DESCRIPTION "Installs the available apps")

# Create the base component.
cpack_add_component(${CAAPI_BASE_COMPONENT_NAME} DISPLAY_NAME "Base version ${CAAPI_BASE_VERSION}"
  DESCRIPTION "Basic files needed to create CAAPI applications"
  REQUIRED
  GROUP  framework)


########################## IMPLEMENTATIONS ##########################

# Search for possible implementations
set(CAAPI_IMPLS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/impls/")
CAAPI_find_impls(CAAPI_IMPLS ${CAAPI_IMPLS_DIR} "ca2D.hpp")

# Number of implementatios
list(LENGTH CAAPI_IMPLS __num_impls)
message(STATUS "Found ${__num_impls} CAAPI implementations")

# Show the available implementations and prepare the possible
# installation by visiting the directory.
message(STATUS "CAAPI available implementations:")
set(__var "0")
foreach(__impl ${CAAPI_IMPLS})
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/impls/${__impl}/CMakeLists.txt")
    MATH(EXPR __var "${__var}+1")
    message(STATUS "${__var}) ${__impl}")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/impls/${__impl}")
  endif()
endforeach()

########################## SOURCES SPECIFIC IMPLEMENTATION ##########################

# Allow the developer to chose the specific implementation of the
# CADDIES API by choseing the specific directory
# The default a simple regular sqare grid.
set(CAAPI_SPECIFIC_IMPL_DIR 
  "${CMAKE_CURRENT_SOURCE_DIR}/impls/square-cell/vn-neighbours/1-levels/simple/"
  CACHE PATH "The directory of the specific CAAPI implementation" )

# Get the name of the implementation selected
string(REPLACE ${CAAPI_IMPLS_DIR} "" CAAPI_IMPL_NAME ${CAAPI_SPECIFIC_IMPL_DIR})
message(STATUS "CAAPI implementation selected: ${CAAPI_IMPL_NAME}")

# From specific directory
CAAPI_add_source_dir(impls\\\\specific    "${CAAPI_SPECIFIC_IMPL_DIR}/*.hpp"              CAAPI_HEADERS)
CAAPI_add_source_dir(impls\\\\specific    "${CAAPI_SPECIFIC_IMPL_DIR}/*.cpp"              CAAPI_SOURCES)
CAAPI_add_source_dir(impls\\\\specific    "${CAAPI_SPECIFIC_IMPL_DIR}/*.ca"               CAAPI_CACOMMON)

# Define the environmental variables which contains extra includes, headers
# libraries needed by the specific implementation.
set(CAAPI_IMPL_INCLUDE_DIRS)
set(CAAPI_IMPL_LIBRARIES)


# Execute the module  for the specific implementation
if(EXISTS "${CAAPI_SPECIFIC_IMPL_DIR}/extra/CAAPI_impl.cmake")

  include("${CAAPI_SPECIFIC_IMPL_DIR}/extra/CAAPI_impl.cmake")

  CAAPI_impl("${CAAPI_SPECIFIC_IMPL_DIR}" CAAPI_IMPL_INCLUDE_DIRS CAAPI_IMPL_LIBRARIES)
endif()


# Add the implementation dir to the includ dirs
list(APPEND CAAPI_IMPL_INCLUDE_DIRS ${CAAPI_SPECIFIC_IMPL_DIR})

# Execute the module that manage the common ca for the specific implementation
if(EXISTS "${CAAPI_SPECIFIC_IMPL_DIR}/extra/CAAPI_ca.cmake")

  include("${CAAPI_SPECIFIC_IMPL_DIR}/extra/CAAPI_ca.cmake")

  CAAPI_ca("${CAAPI_SPECIFIC_IMPL_DIR}" CAAPI_CAHEADERS CAAPI_CACOMMON)
endif()

# Create the target that make the eventual implementation headers
# necessary.
add_custom_target(caapi_do_impl_headers ALL DEPENDS ${CAAPI_CAHEADERS} ${CAAPI_HEADERS})

# Add the needed directories into the compilation
include_directories(${CAAPI_IMPL_INCLUDE_DIRS})

message(STATUS "Include dir: ${CAAPI_IMPL_INCLUDE_DIRS}")


########################## CAPI DEVICE common files ##########################

#message(STATUS "${CAAPI_DEVICE}")


if(NOT DEFINED CAAPI_DEVICE)
	message(STATUS "Device: CPU")
	set(CAAPI_DEVICE_COMMON_FOLDER "${CAAPI_COMMON_FOLDER}CPU/")
elseif(CAAPI_DEVICE STREQUAL "GPU")
	set(CAAPI_DEVICE_COMMON_FOLDER "${CAAPI_COMMON_FOLDER}GPU/")
	message(STATUS "Device: GPU")
else()
	message(STATUS "Device: UNKNOWN")
endif()

if(DEFINED CAAPI_DEVICE_COMMON_FOLDER)
	CAAPI_add_source_dir(impls\\\\common      "${CMAKE_CURRENT_SOURCE_DIR}${CAAPI_DEVICE_COMMON_FOLDER}*.hpp" CAAPI_HEADERS)
	CAAPI_add_source_dir(impls\\\\common      "${CMAKE_CURRENT_SOURCE_DIR}${CAAPI_DEVICE_COMMON_FOLDER}*.cpp" CAAPI_SOURCES)
	CAAPI_add_source_dir(impls\\\\common      "${CMAKE_CURRENT_SOURCE_DIR}${CAAPI_DEVICE_COMMON_FOLDER}*.ca" CAAPI_CACOMMON)

	include_directories("${CMAKE_CURRENT_SOURCE_DIR}${CAAPI_DEVICE_COMMON_FOLDER}")
	
endif()



########################## CAPI APPS ##########################


# Visit the CAAPI apps directory.
# This directory should contain a sub-directory for each application that 
# uses the CAAPI code.
add_subdirectory(apps)


########################## END ##########################



