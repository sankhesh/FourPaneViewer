/*=============================================================================
Copyright and License information
=============================================================================*/

//  includes
#include "QtVTKHotKeyEventPlayer.h"

// Qt includes
#include <QApplication>
#include <QDebug>
#include <QEvent>
#include <QKeyEvent>
#include <QStringList>
#include <QWidget>
#include <QShortcutEvent>

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

  if (eventType == pqEventTypes::ACTION_EVENT && widget->isWindow())
  {
    if (command == "keyEvent")
    {
      QStringList data = arguments.split(':');
      //QKeyEvent* ke = new QKeyEvent(static_cast<QEvent::Type>(data[0].toInt()),
      //             Qt::Key(data[1].toInt()),
      //             Qt::KeyboardModifiers(data[2].toInt()),
      //             QString(data[3]),
      //             !!data[4].toInt(),
      //             data[5].toInt());
      //qDebug() << widget->metaObject()->className() <<  ke->type() << ke->key() <<  data[2].toInt() << ke->modifiers() << ke->text();
      //    if (ke->matches(QKeySequence::Save))
      //      {
      //      qDebug() << "Screenshot shortcut";
      //      }
      qDebug() << arguments << data.size() << data[6];
      widget->window()->activateWindow();
      QShortcutEvent* se = new QShortcutEvent(QKeySequence("Ctrl+S"), data[6].toInt());
      qApp->notify(object, se);
      return true;
    }
  }
  return this->Superclass::playEvent(
    object, command, arguments, eventType, error);
}
