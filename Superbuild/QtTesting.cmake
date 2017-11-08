SET (QtTesting_ROOT
  "${CMAKE_BINARY_DIR}/QtTesting"
  )

SET (QtTesting_CMAKE_ARGS
  -DQtTesting_QT_VERSION:STRING=5
  -DBUILD_TESTING:BOOL=OFF
  -DBUILD_SHARED_LIBS:BOOL=ON
  -DQt5Widgets_DIR:PATH=${Qt5Widgets_DIR}
  -DQt5Core_DIR:PATH=${Qt5Core_DIR}
  -DQt5_DIR:PATH=${Qt5_DIR}
  )

ExternalProject_Add (
  QtTesting
  PREFIX "${QtTesting_ROOT}"
  STAMP_DIR "${QtTesting_ROOT}/stamp"
  GIT_REPOSITORY "https://github.com/sankhesh/QtTesting.git"
  GIT_TAG "master"
  SOURCE_DIR "${QtTesting_ROOT}/src"
  BINARY_DIR "${QtTesting_ROOT}/bld"
  CMAKE_ARGS "${QtTesting_CMAKE_ARGS}"
  )
