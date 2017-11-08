CMAKE_MINIMUM_REQUIRED(VERSION 3.9)

PROJECT(FourPaneViewerSuperbuild)

SET (CMAKE_MODULE_PATH
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_MODULE_PATH}
  )

INCLUDE_DIRECTORIES (
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )

INCLUDE (ExternalProject)

SET(EP_BASE
  ${CMAKE_CURRENT_BINARY_DIR}
  )

find_package(Qt5 COMPONENTS Core Widgets REQUIRED QUIET)

# Build QtTesting
INCLUDE (QtTesting)

find_package(VTK COMPONENTS
  vtkCommonCore
  vtkFiltersSources
  vtkGUISupportQt
  vtkIOImage
  vtkInteractionImage
)
include(${VTK_USE_FILE})

# Build FourPaneViewer
INCLUDE (FourPaneViewer)
