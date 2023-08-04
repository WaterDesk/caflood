#  - find list of subdiretories without hidden one
MACRO(subdirlist result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/[^.]* 
       FOLLOW_SYMLINK)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY "${curdir}/${child}/")     
        SET(dirlist ${dirlist} ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()
