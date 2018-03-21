# Macro to check the existence of required ITK libraries
#   use this in subdirectory CMakeList to avoid expensive
#   `find_package` calls, which do a slow recursion over
#   all the ITK submodules.
macro(slicer_require_itk_libs libs)
  foreach(component ${${libs}})
    if(NOT ${component} IN_LIST ITK_MODULES_ENABLED)
      message(FATAL_ERROR "Required ITK component ${component} not found!")
    endif()
  endforeach()
endmacro()
