# Helper macro that create the installation set up for the implementation.
macro(CAAPI_install) 

# The name of the implementation.
set(CAAPI_IMPL_NAME "Simple 1L VN Square Grid")

# The short name of the implementation. This is used for the package name.
set(CAAPI_IMPL_SHORTNAME  "S1LVNSG")

# Read the CAAAPI version and implementation version number from file
file(STRINGS "${CAAPI_SPECIFIC_IMPL_DIR}/extra/CAAPIVersion.txt" CAAPI_VERSION)
file(STRINGS "${CAAPI_SPECIFIC_IMPL_DIR}/extra/CAAPIImplVersion.txt" CAAPI_IMPL_VERSION)

########################## INSTALL  ##########################

# Set the component name which will influence the archive name.
set(CAAPI_IMPL_COMPONENT_NAME "impl-${CAAPI_IMPL_SHORTNAME}-${CAAPI_IMPL_VERSION}")


# Find all the code files automatically.
set(CAAPI_IMPL_SOURCES "")
file(GLOB CAAPI_IMPL_SOURCES "${CAAPI_SPECIFIC_IMPL_DIR}/*.cpp")

set(CAAPI_IMPL_HEADERS "")
file(GLOB CAAPI_IMPL_HEADERS "${CAAPI_SPECIFIC_IMPL_DIR}/*.hpp")

set(CAAPI_IMPL_CAFUNCTIONS "")
file(GLOB CAAPI_IMPL_CAFUNCTIONS "${CAAPI_SPECIFIC_IMPL_DIR}/*.ca")


# The data to install
set(CAAPI_IMPL_DATA "")
list(APPEND CAAPI_IMPL_DATA "${CAAPI_SPECIFIC_IMPL_DIR}/NEWS")
list(APPEND CAAPI_IMPL_DATA "${CAAPI_SPECIFIC_IMPL_DIR}/LICENSE")
list(APPEND CAAPI_IMPL_DATA "${CAAPI_SPECIFIC_IMPL_DIR}/AUTHORS")

# The extra data to install
set(CAAPI_IMPL_EXTRA_DATA "")
list(APPEND CAAPI_IMPL_EXTRA_DATA "${CAAPI_SPECIFIC_IMPL_DIR}/extra/CAAPIVersion.txt")
list(APPEND CAAPI_IMPL_EXTRA_DATA "${CAAPI_SPECIFIC_IMPL_DIR}/extra/CAAPIImplVersion.txt")
list(APPEND CAAPI_IMPL_EXTRA_DATA "${CAAPI_SPECIFIC_IMPL_DIR}/extra/CAAPI_impl.cmake")

# Add the file that are needed to compile this application into its
# specific component.
install(FILES   
  ${CAAPI_IMPL_DATA}
  ${CAAPI_IMPL_SOURCES}
  ${CAAPI_IMPL_HEADERS}
  ${CAAPI_IMPL_CAFUNCTIONS}
  DESTINATION impls/square-cell/vn-neighbours/1-levels/simple
  COMPONENT ${CAAPI_IMPL_COMPONENT_NAME})

install(FILES   
  ${CAAPI_IMPL_EXTRA_DATA}
  DESTINATION impls/square-cell/vn-neighbours/1-levels/simple/extra/
  COMPONENT ${CAAPI_IMPL_COMPONENT_NAME})

# Add the newly created component
cpack_add_component(${CAAPI_IMPL_COMPONENT_NAME} DISPLAY_NAME "${CAAPI_IMPL_NAME} version ${CAAPI_IMPL_VERSION}"
  DESCRIPTION "Simple implementation of the CAAPI that uses a square
  grid with one single level of von Neumann neighbourhood that run on a single CPU " 
  GROUP impls)

########################## PACKAGE  ##########################


message("PIPPO")

endmacro(CAAPI_install)
