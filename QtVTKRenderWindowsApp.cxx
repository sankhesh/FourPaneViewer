#include <QApplication>
#include <QSurfaceFormat>
#include "QtVTKRenderWindows.h"

#include <QVTKOpenGLWidget.h>

int main( int argc, char** argv )
{
  QSurfaceFormat::setDefaultFormat(QVTKOpenGLWidget::defaultFormat());
  // QT Stuff
  QApplication app( argc, argv );

  QtVTKRenderWindows myQtVTKRenderWindows(argc, argv);
  myQtVTKRenderWindows.show();

  return app.exec();
}
