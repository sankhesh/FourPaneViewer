/*=============================================================================
Copyright and License information
=============================================================================*/

//  includes
#include "QtVTKHotKeyEventTranslator.h"

#include <pqEventTypes.h>

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

  if (eventType == pqEventTypes::ACTION_EVENT && widget->isWindow())
  {
    switch (event->type())
    {
      case QEvent::Shortcut:
        qDebug() << "Shortcut found.";
        break;
      case QEvent::ShortcutOverride:
//      case QEvent::KeyPress:
        // case QEvent::KeyRelease:
        {
          QShortcutEvent* se = static_cast<QShortcutEvent*>(event);
//          QKeyEvent* ke = static_cast<QKeyEvent*> (event);
//          QString keys = QString(ke->key());
//          if (ke->modifiers() & Qt::ShiftModifier)
//            {
//            keys += "+ Shift";
//            }
//          if (ke->modifiers() & Qt::ControlModifier)
//            {
//            keys += "+ Ctrl";
//            }
//
//          QString info = QString("%1:%2")
//            .arg(keys)
//            .arg(se->shortcutId());
          //qDebug() << se << keys << se->shortcutId();
          QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
//          if (keyEvent->matches(QKeySequence::Save))
//            {
//            qDebug() << "Screenshot shortcut";
//            }
          QString info = QString("%1:%2:%3:%4:%5:%6:%7")
                           .arg(QEvent::Shortcut)
                           .arg(keyEvent->key())
                           .arg(keyEvent->modifiers())
                           .arg(keyEvent->text())
                           .arg(keyEvent->isAutoRepeat())
                           .arg(keyEvent->count())
                           .arg(se->shortcutId());
          qDebug() << widget->metaObject()->className() << keyEvent->type()
                   << keyEvent->key() << keyEvent->modifiers()
                   << keyEvent->text() << se->shortcutId();
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
        return this->Superclass::translateEvent(
          object, event, eventType, error);
      }
    }
  }
  return this->Superclass::translateEvent(object, event, eventType, error);
}
