# This directory should contain a sub-directory for each application that 
# uses the CAAPI code.

# Retrieve the list of subdirectory
subdirlist(CAAPI_APPS ${CMAKE_CURRENT_SOURCE_DIR})

# Show available apps.
list(LENGTH CAAPI_APPS __num_apps)
message(STATUS "Found ${__num_apps} possible CAAPI applications")

message(STATUS "CAAPI available applications:")
set(__var "0")
foreach(__app ${CAAPI_APPS})
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${__app}/CMakeLists.txt")
    MATH(EXPR __var "${__var}+1")
    message(STATUS "${__var}) ${__app}")
  endif()
endforeach()


# Visit the subdirectory if contains CMakeLists.txt
foreach (__app ${CAAPI_APPS})
  # Set the name of the app
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${__app}/CMakeLists.txt")
    set(CAAPI_APP_NAME "${__app}")
    add_subdirectory(${__app})
  endif()
endforeach ()
