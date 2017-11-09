/*=============================================================================
Copyright and License information
=============================================================================*/

//  includes
#include "QtVTKHotKeyEventTranslator.h"

// Qt includes
#include <QDebug>
#include <QEvent>
#include <QKeyEvent>
#include <QString>
#include <QWidget>

//-----------------------------------------------------------------------------
QtVTKHotKeyEventTranslator::QtVTKHotKeyEventTranslator(QObject* parent)
  : Superclass(parent)
{
}

//-----------------------------------------------------------------------------
QtVTKHotKeyEventTranslator::~QtVTKHotKeyEventTranslator()
{
}

//-----------------------------------------------------------------------------
bool QtVTKHotKeyEventTranslator::translateEvent(QObject* object,
                                                QEvent* event,
                                                int eventType,
                                                bool& error)
{
  QWidget* widget = qobject_cast<QWidget*>(object);
  if (!widget)
  {
    return false;
  }

  switch (event->type())
  {
    case QEvent::KeyPress:
    //case QEvent::KeyRelease:
    {
      QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
      QString info = QString("%1:%2:%3:%4:%5:%6")
                       .arg(keyEvent->type())
                       .arg(keyEvent->key())
                       .arg(keyEvent->modifiers())
                       .arg(keyEvent->text())
                       .arg(keyEvent->isAutoRepeat())
                       .arg(keyEvent->count());
      qDebug() << keyEvent->modifiers();
      emit recordEvent(widget, "keyEvent", info);
      //      if (event->type() == QEvent::KeyPress)
      //      {
      //        emit recordEvent(widget, "keyPress", info);
      //      }
      //      else if (event->type() == QEvent::KeyRelease)
      //      {
      //        emit recordEvent(widget, "keyRelease", info);
      //      }
      return true;
      break;
    }
    default:
    {
      return this->Superclass::translateEvent(object, event, eventType, error);
    }
  }
  return this->Superclass::translateEvent(object, event, eventType, error);
}
