
#-----------------------------------------------------------------------------
set(MODULE_NAME DiffusionTensorTest)

#-----------------------------------------------------------------------------

#
# SlicerExecutionModel
#
find_package(SlicerExecutionModel REQUIRED)
include(${SlicerExecutionModel_USE_FILE})

#
# ITK
#
set(ITK_COMPONENTS
  ITKCommon
  ITKIOImageBase
  )
slicer_require_itk_libs(ITK_COMPONENTS)

#-----------------------------------------------------------------------------
SEMMacroBuildCLI(
  NAME ${MODULE_NAME}
  LOGO_HEADER ${Slicer_SOURCE_DIR}/Resources/ITKLogo.h
  TARGET_LIBRARIES ${ITK_COMPONENTS}
  NO_INSTALL
  )

#-----------------------------------------------------------------------------
if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()

