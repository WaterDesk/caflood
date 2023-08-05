# Helper macro that add the libraries and includes necessary to build
# the implementation of this directory.
macro(CAAPI_impl __TMP_IMPL_DIR __TMP_INCLUDE_DIRS __TMP_LIBRARIES) 

# The name of the implementation.
file(STRINGS "${CAAPI_SPECIFIC_IMPL_DIR}/extra/ImplName.txt" CAAPI_IMPL_NAME)

# The short name of the implementation. This is used for the package name.
file(STRINGS "${CAAPI_SPECIFIC_IMPL_DIR}/extra/ImplShortName.txt" CAAPI_IMPL_SHORTNAME)

# Read the CAAAPI version and implementation version number from file
file(STRINGS "${CAAPI_SPECIFIC_IMPL_DIR}/extra/CAAPIVersion.txt" CAAPI_VERSION)
file(STRINGS "${CAAPI_SPECIFIC_IMPL_DIR}/extra/CAAPIImplVersion.txt" CAAPI_IMPL_VERSION)

# Show the version number
message(STATUS "[${CAAPI_IMPL_NAME}]: CAAPI Version ${CAAPI_VERSION}")
message(STATUS "[${CAAPI_IMPL_NAME}]: CAAPI Implementation Version ${CAAPI_IMPL_VERSION}")

if (WIN32)
  add_definitions( /DAsciiGrid=ESRI_ASCIIGrid )
  add_definitions( /DCAAPI_IMPL_NAME=${CAAPI_IMPL_NAME} )
  add_definitions( /DCAAPI_IMPL_SHORTNAME=${CAAPI_IMPL_SHORTNAME} )
  add_definitions( /DCAAPI_VERSION=${CAAPI_VERSION} )
  add_definitions( /DCAAPI_IMPL_VERSION=${CAAPI_IMPL_VERSION} )
else()
  add_definitions( -DAsciiGrid=ESRI_ASCIIGrid )
  add_definitions( -DCAAPI_IMPL_NAME=${CAAPI_IMPL_NAME} )
  add_definitions( -DCAAPI_IMPL_SHORTNAME=${CAAPI_IMPL_SHORTNAME} )
  add_definitions( -DCAAPI_VERSION=${CAAPI_VERSION} )
  add_definitions( -DCAAPI_IMPL_VERSION=${CAAPI_IMPL_VERSION} )
endif()

########################## OPENCL  ##########################

# Include the special module file that find OpenCL
set(CMAKE_MODULE_PATH "${__TMP_IMPL_DIR}/extra/")

# Find the required OpenCL library.
find_package( OpenCL REQUIRED )

# Add the directory with the OpenCL includes
#set(${__TMP_INCLUDE_DIRS} "${${__TMP_INCLUDE_DIRS}} ${}")
list(APPEND ${__TMP_INCLUDE_DIRS} ${OPENCL_INCLUDE_DIRS})

# Add the OpenCL libraries 
list(APPEND ${__TMP_LIBRARIES} ${OPENCL_LIBRARIES})

# Check that the cpp binding is there. If it is not the case use the provided implementation
if(NOT OPENCL_HAS_CPP_BINDINGS )
  list(APPEND ${__TMP_INCLUDE_DIRS} "${__TMP_IMPL_DIR}/extra/")
endif()

# Add the exacutable that convert .ca files into .hpp files which
# contain the CA function in a string, which can be read from OpenCL.
add_executable( convertCA2HPP "${__TMP_IMPL_DIR}/extra/convertCA2HPP.cpp" )



# Add the exacutable that retrive information abour OpenCL
include_directories(${OPENCL_INCLUDE_DIRS})
add_executable( clinfo "${__TMP_IMPL_DIR}/extra/clinfo.cpp")
target_link_libraries(clinfo ${OPENCL_LIBRARIES} )

# Chose between constant memory or global memory for CA::Table.
set(CAAPI_OCL_TABLE "constant" CACHE STRING "The type of memory for table object: constant | global" )

#DEFINITION
string( TOLOWER "${CAAPI_OCL_TABLE}" caapi_ocl_table )

if( caapi_ocl_table STREQUAL "constant" )
  if (WIN32)
    add_definitions("/DCA_OCL_TABLE=0")
  else()
    add_definitions("-DCA_OCL_TABLE=0")
  endif()
  message(STATUS "OpenCL Table: constant")
elseif( caapi_ocl_table STREQUAL "global" )
  if (WIN32)
    add_definitions("/DCA_OCL_TABLE=1")
  else()
    add_definitions("-DCA_OCL_TABLE=1")
  endif()
  message(STATUS "OpenCL Table: global")
endif()

# Definitions, which are used to take advantage of different code segments,
# in order to make this GPU compatible
set(CAAPI_DEVICE "GPU")
add_definitions(-DCA2D_GPU)

# Chose between to use event or not to use event (event has tendency
# to leak memory in NVIDIA drivers.)
set(CAAPI_OCL_EVENTS "enable" CACHE STRING "Use events to queue OpenCL commands: enable | disable" )

#DEFINITION
string( TOLOWER "${CAAPI_OCL_EVENTS}" caapi_ocl_events )

if( caapi_ocl_events STREQUAL "enable" )
  if (WIN32)
    add_definitions("/DCA_OCL_EVENTS=1")
  else()
    add_definitions("-DCA_OCL_EVENTS=1")
  endif()
  message(STATUS "OpenCL Events: enabled")
else()
  if (WIN32)
    add_definitions("/DCA_OCL_EVENTS=0")
  else()
    add_definitions("-DCA_OCL_EVENTS=0")
  endif()
  message(STATUS "OpenCL Events: disabled")
endif()


# Chose between to use event or not to use event (event has tendency
# to leak memory in NVIDIA drivers.)
set(CAAPI_OCL_NATIVE "disable" CACHE STRING "Use native math functions (Pow/Sqrt) in OpenCL: enable | disable")

#DEFINITION
string( TOLOWER "${CAAPI_OCL_NATIVE}" caapi_ocl_events )

if( caapi_ocl_events STREQUAL "enable" )
  if (WIN32)
    add_definitions("/DCA_OCL_NATIVE=1")
  else()
    add_definitions("-DCA_OCL_NATIVE=1")
  endif()
  message(STATUS "OpenCL Native Math: enabled")
else()
  if (WIN32)
    add_definitions("/DCA_OCL_NATIVE=0")
  else()
    add_definitions("-DCA_OCL_NATIVE=0")
  endif()
  message(STATUS "OpenCL Native Math: disabled")
endif()


endmacro(CAAPI_impl)
