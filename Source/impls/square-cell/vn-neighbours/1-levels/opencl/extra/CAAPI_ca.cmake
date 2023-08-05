# Helper macro that process the ca file with the CA FUNCTIONS.
macro(CAAPI_ca __TMP_IMPL_DIR __TMP_HEADERS __TMP_CAFUNCTIONS) 

# Get the las direcotory of the path
get_filename_component(LASTDIR ${CMAKE_CURRENT_SOURCE_DIR} NAME)

# Add the common cl source 
CAAPI_add_source_dir(cl "${__TMP_IMPL_DIR}/*.cl" ${__TMP_CAFUNCTIONS})

# Loop through the ca files.
foreach(__file_ca ${${__TMP_CAFUNCTIONS}})


  # Get the ca filename without path
  get_filename_component(__file_ca_wp ${__file_ca} NAME)


  # Set the output file to be inputfile.ca.hpp
  set(__file_hpp "${CMAKE_CURRENT_BINARY_DIR}/${__file_ca_wp}.hpp")


  # Get the filename without extension, i.e. the CA function name.
  get_filename_component(__file_we ${__file_ca} NAME_WE)


  # Add the command that generate the .hpp file from .ca file.
  add_custom_command(
    OUTPUT ${__file_hpp} 
    COMMAND convertCA2HPP "${__file_we}" "${__file_ca}" "${__file_hpp}"
    DEPENDS ${__file_ca}
    ) 
   

  # Add the newly create .hpp file into a list.
  list(APPEND ${__TMP_HEADERS} ${__file_hpp})


endforeach()


# Add the current binary directory into the include path.
include_directories(${CMAKE_CURRENT_BINARY_DIR})


endmacro(CAAPI_ca)
