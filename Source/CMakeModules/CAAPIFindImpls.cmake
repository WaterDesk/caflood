# -helper macro that find CAAPI implementations from a basedir that have a specific file

macro(CAAPI_find_impls IMPLS BASEDIR FILE) 

  # Find all instance of the file in all the subdirectories from
  # the given directory.
  file(GLOB_RECURSE __tmp_impls RELATIVE ${BASEDIR} "${BASEDIR}/${FILE}")
  
  # Get only the path 
  foreach(__tmp_impl ${__tmp_impls})
    get_filename_component(__tmp_dir ${__tmp_impl} PATH)
    
    # Append into the output variable.
    list(APPEND ${IMPLS} ${__tmp_dir})
  endforeach()
  
endmacro(CAAPI_find_impls)

