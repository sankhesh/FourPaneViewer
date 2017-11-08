#include <QString>
#include <QXmlStreamReader>
#include <QByteArray>
#include <QDebug>
#include <QFile>

#include <pqEventSource.h>

class QtVTKXMLEventSource : public pqEventSource
{
  typedef pqEventSource Superclass;
  QXmlStreamReader *XMLStream;

public:
  QtVTKXMLEventSource(QObject* p): Superclass(p) { this->XMLStream = NULL;}
  ~QtVTKXMLEventSource() override { delete this->XMLStream; }

protected:
  void setContent(const QString& xmlfilename) override
    {
    delete this->XMLStream;
    this->XMLStream = NULL;

    QFile xml(xmlfilename);
    if (!xml.open(QIODevice::ReadOnly))
      {
      qDebug() << "Failed to load " << xmlfilename;
      return;
      }
    QByteArray data = xml.readAll();
    this->XMLStream = new QXmlStreamReader(data);
    /* This checked for valid event objects, but also caused the first event
     * to get dropped. Commenting this out in the example. If you wish to report
     * empty XML test files a flag indicating whether valid events were found is
     * probably the best way to go.
    while (!this->XMLStream->atEnd())
      {
      QXmlStreamReader::TokenType token = this->XMLStream->readNext();
      if (token == QXmlStreamReader::StartElement)
        {
        if (this->XMLStream->name() == "event")
          {
          break;
          }
        }
      } */
    if (this->XMLStream->atEnd())
      {
      qDebug() << "Invalid xml" << endl;
      }
    return;
    }

  int getNextEvent(QString& widget, QString& command, QString&
    arguments,int& eventType) override
    {
    if (this->XMLStream->atEnd())
      {
      return DONE;
      }
    while (!this->XMLStream->atEnd())
      {
      QXmlStreamReader::TokenType token = this->XMLStream->readNext();
      if (token == QXmlStreamReader::StartElement)
        {
        if (this->XMLStream->name() == "event")
          {
          break;
          }
        }
      }
    if (this->XMLStream->atEnd())
      {
      return DONE;
      }
    eventType = pqEventTypes::ACTION_EVENT;
    widget = this->XMLStream->attributes().value("widget").toString();
    command = this->XMLStream->attributes().value("command").toString();
    arguments = this->XMLStream->attributes().value("arguments").toString();
    return SUCCESS;
    }
};
