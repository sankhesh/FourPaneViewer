SET (FourPaneViewer_ROOT
  "${CMAKE_BINARY_DIR}/FourPaneViewer"
  )

SET (FourPaneViewer_CMAKE_ARGS
  -DQtTesting_DIR:PATH=${QtTesting_ROOT}/bld
  -DVTK_DIR:PATH=${VTK_DIR}
  )

ExternalProject_Add (
  FourPaneViewer
  DEPENDS QtTesting
  PREFIX "${FourPaneViewer_ROOT}"
  STAMP_DIR "${FourPaneViewer_ROOT}/stamp"
  URL "${CMAKE_SOURCE_DIR}/../"
  BINARY_DIR "${FourPaneViewer_ROOT}/bld"
  CMAKE_ARGS "${FourPaneViewer_CMAKE_ARGS}"
  INSTALL_COMMAND ""
  )

