# -helper macro to add a "sub-directory" into the source files list.

macro(CAAPI_add_source_dir DIR_LEVEL FILES VAR) 

	# Find all the given files from the given directory.
	file(GLOB __TMP_SOURCES "${FILES}")


	# Append into the given source variable.
	list(APPEND ${VAR} ${__TMP_SOURCES})


	# Set the directory of the given files into Visual
	# Studio. Without the source_group command the files will be
	# under a generic "Sources Files" and "Headers Files"
	# directories.	
	if(MSVC)
		source_group(${DIR_LEVEL} FILES ${__TMP_SOURCES})
	endif(MSVC)

endmacro(CAAPI_add_source_dir)

