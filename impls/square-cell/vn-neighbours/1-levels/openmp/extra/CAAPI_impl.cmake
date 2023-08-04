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

if (WIN32 AND NOT MINGW)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /openmp")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /openmp")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")
endif()

message(STATUS "[${CAAPI_IMPL_NAME}]: Added compiler flag ${CMAKE_CXX_FLAGS}")

endmacro(CAAPI_impl)
