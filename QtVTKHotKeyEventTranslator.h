/*=============================================================================
Copyright and License information
=============================================================================*/

#ifndef QtVTKHotKeyEventTranslator_h
#define QtVTKHotKeyEventTranslator_h

// Qt includes
#include <pqBasicWidgetEventTranslator.h>
#include <QObject>
#include <QEvent>

///
class QtVTKHotKeyEventTranslator : public pqBasicWidgetEventTranslator
{
  Q_OBJECT
  typedef pqBasicWidgetEventTranslator Superclass;

public:
  QtVTKHotKeyEventTranslator(QObject* parent = 0);
  ~QtVTKHotKeyEventTranslator() override;

  using Superclass::translateEvent;
  bool translateEvent(QObject* object,
                      QEvent* event,
                      int eventType,
                      bool& error) override;

private:
  QtVTKHotKeyEventTranslator(const QtVTKHotKeyEventTranslator&);
  QtVTKHotKeyEventTranslator& operator=(const QtVTKHotKeyEventTranslator&);
};

#endif // QtVTKHotKeyEventTranslator_h
