/*=============================================================================
Copyright and License information
=============================================================================*/

//  includes
#include "QtVTKHotKeyEventPlayer.h"

// Qt includes
#include <QApplication>
#include <QEvent>
#include <QKeyEvent>
#include <QStringList>
#include <QWidget>

#include <pqEventTypes.h>

//-----------------------------------------------------------------------------
QtVTKHotKeyEventPlayer::QtVTKHotKeyEventPlayer(QObject* parent)
  : Superclass(parent)
{
}

//-----------------------------------------------------------------------------
QtVTKHotKeyEventPlayer::~QtVTKHotKeyEventPlayer()
{
}

//-----------------------------------------------------------------------------
bool QtVTKHotKeyEventPlayer::playEvent(QObject* object,
                                       const QString& command,
                                       const QString& arguments,
                                       int eventType,
                                       bool& error)
{
  QWidget* widget = qobject_cast<QWidget*>(object);
  if (!widget)
  {
    return false;
  }

  if (eventType == pqEventTypes::ACTION_EVENT)
  {
    if (command == "keyEvent")
    {
      QStringList data = arguments.split(':');
      QKeyEvent ke(static_cast<QEvent::Type>(data[0].toInt()),
                   data[1].toInt(),
                   static_cast<Qt::KeyboardModifiers>(data[2].toInt()),
                   data[3],
                   !!data[4].toInt(),
                   data[5].toInt());
      QCoreApplication::sendEvent(object, &ke);
    }
  }
  return this->Superclass::playEvent(
    object, command, arguments, eventType, error);
}
