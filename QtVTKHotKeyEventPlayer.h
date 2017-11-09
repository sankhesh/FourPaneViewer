/*=============================================================================
Copyright and License information
=============================================================================*/

#ifndef QtVTKHotKeyEventPlayer_h
#define QtVTKHotKeyEventPlayer_h

// Qt includes
#include <pqBasicWidgetEventPlayer.h>

///
class QtVTKHotKeyEventPlayer : public pqBasicWidgetEventPlayer
{
  Q_OBJECT
  typedef pqBasicWidgetEventPlayer Superclass;

public:
  QtVTKHotKeyEventPlayer(QObject* parent = 0);
  ~QtVTKHotKeyEventPlayer() override;

  using Superclass::playEvent;
  bool playEvent(QObject* object,
                 const QString& command,
                 const QString& arguments,
                 int eventType,
                 bool& error) override;

private:
  Q_DISABLE_COPY(QtVTKHotKeyEventPlayer);
};

#endif // QtVTKHotKeyEventPlayer_h
