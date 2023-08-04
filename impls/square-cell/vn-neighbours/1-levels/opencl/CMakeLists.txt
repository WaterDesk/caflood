########### ADD IMPLEMENTATION ###############

# The name of the implementation.
file(STRINGS "./extra/ImplName.txt" CAAPI_IMPL_NAME)

# The short name of the implementation. This is used for the package name.
file(STRINGS "./extra/ImplShortName.txt" CAAPI_IMPL_SHORTNAME)

# Read the CAAAPI version and implementation version number from file
file(STRINGS "./extra/CAAPIVersion.txt" CAAPI_VERSION)
file(STRINGS "./extra/CAAPIImplVersion.txt" CAAPI_IMPL_VERSION)

########################## INSTALL  ##########################

# Set the component name which will influence the archive name.
set(CAAPI_IMPL_COMPONENT_NAME "CAAPI-impl-${CAAPI_IMPL_SHORTNAME}-${CAAPI_IMPL_VERSION}")

# Find all the code files automatically.
set(CAAPI_IMPL_SOURCES "")
file(GLOB CAAPI_IMPL_SOURCES "*.cpp")

set(CAAPI_IMPL_HEADERS "")
file(GLOB CAAPI_IMPL_HEADERS "*.hpp")

set(CAAPI_IMPL_CLFUNCTIONS "")
file(GLOB CAAPI_IMPL_CLFUNCTIONS "*.cl")


# The data to install
set(CAAPI_IMPL_DATA "")
list(APPEND CAAPI_IMPL_DATA "NEWS")
list(APPEND CAAPI_IMPL_DATA "LICENSE")
list(APPEND CAAPI_IMPL_DATA "AUTHORS")
list(APPEND CAAPI_IMPL_DATA "CMakeLists.txt")

# The extra data to install
set(CAAPI_IMPL_EXTRA_DATA "")
list(APPEND CAAPI_IMPL_EXTRA_DATA "./extra/ImplName.txt")
list(APPEND CAAPI_IMPL_EXTRA_DATA "./extra/ImplShortName.txt")
list(APPEND CAAPI_IMPL_EXTRA_DATA "./extra/CAAPIVersion.txt")
list(APPEND CAAPI_IMPL_EXTRA_DATA "./extra/CAAPIImplVersion.txt")
list(APPEND CAAPI_IMPL_EXTRA_DATA "./extra/CAAPI_impl.cmake")
list(APPEND CAAPI_IMPL_EXTRA_DATA "./extra/CAAPI_ca.cmake")
list(APPEND CAAPI_IMPL_EXTRA_DATA "./extra/CAAPI_ca.cmake")
list(APPEND CAAPI_IMPL_EXTRA_DATA "./extra/FindOpenCL.cmake")
list(APPEND CAAPI_IMPL_EXTRA_DATA "./extra/convertCA2HPP.cpp")
list(APPEND CAAPI_IMPL_EXTRA_DATA "./extra/config_INTEL_CPU.csv")
list(APPEND CAAPI_IMPL_EXTRA_DATA "./extra/config_NVIDIA_GPU.csv")
list(APPEND CAAPI_IMPL_EXTRA_DATA "./extra/clinfo.cpp")

# Add the file that are needed to compile this application into its
# specific component.
install(FILES   
  ${CAAPI_IMPL_DATA}
  ${CAAPI_IMPL_SOURCES}
  ${CAAPI_IMPL_HEADERS}
  ${CAAPI_IMPL_CLFUNCTIONS}
  DESTINATION impls/square-cell/vn-neighbours/1-levels/opencl
  COMPONENT ${CAAPI_IMPL_COMPONENT_NAME})

install(FILES   
  ${CAAPI_IMPL_EXTRA_DATA}
  DESTINATION impls/square-cell/vn-neighbours/1-levels/opencl/extra/
  COMPONENT ${CAAPI_IMPL_COMPONENT_NAME})

install(FILES   
  "./extra/CL/cl.hpp"
  DESTINATION impls/square-cell/vn-neighbours/1-levels/opencl/extra/CL/
  COMPONENT ${CAAPI_IMPL_COMPONENT_NAME})

########################## PACKAGE  ##########################

# Add the newly created component
cpack_add_component(${CAAPI_IMPL_COMPONENT_NAME} DISPLAY_NAME "${CAAPI_IMPL_NAME} version ${CAAPI_IMPL_VERSION}"
  DESCRIPTION "OpenCL implementation of the CAAPI that uses a square
  grid with one single level of von Neumann neighbourhood that can run on a multi-core CPU or GPU" 
  GROUP impls)



