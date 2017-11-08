#include <QXmlStreamWriter>
#include <QTextStream>
#include <QString>

#include <pqEventObserver.h>

class QtVTKXMLEventObserver : public pqEventObserver
{
  QXmlStreamWriter* XMLStream;
  QString XMLString;

public:
  QtVTKXMLEventObserver(QObject* p) : pqEventObserver(p)
  {
  this->XMLStream = NULL;
  }

  ~QtVTKXMLEventObserver() override
    {
    delete this->XMLStream;
    }

protected:
  void setStream(QTextStream* stream) override
    {
    if (this->XMLStream)
      {
      this->XMLStream->writeEndElement();
      this->XMLStream->writeEndDocument();
      delete this->XMLStream;
      this->XMLStream = NULL;
      }
    if (this->Stream)
      {
      *this->Stream << this->XMLString;
      }
    this->XMLString = QString();
    pqEventObserver::setStream(stream);
    if (this->Stream)
      {
      this->XMLStream = new QXmlStreamWriter(&this->XMLString);
      this->XMLStream->setAutoFormatting(true);
      this->XMLStream->writeStartDocument();
      this->XMLStream->writeStartElement("events");
      }
    }

  void onRecordEvent(const QString& widget, const QString& command,
    const QString& arguments, const int& eventType) override
    {
    if(this->XMLStream)
      {
      this->XMLStream->writeStartElement("event");
      this->XMLStream->writeAttribute("widget", widget);
      if (eventType == pqEventTypes::ACTION_EVENT)
        {
        this->XMLStream->writeAttribute("command", command);
        }
      else // if(eventType == pqEventTypes::CHECK_EVENT)
        {
        this->XMLStream->writeAttribute("property", command);
        }
      this->XMLStream->writeAttribute("arguments", arguments);
      this->XMLStream->writeEndElement();
      }
    }
};
