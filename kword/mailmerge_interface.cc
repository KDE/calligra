#include "serialletter_interface.h"
#include "serialletter_interface.moc"

KWSerialLetterDataSource::KWSerialLetterDataSource(KInstance* inst,QObject *parent): QObject(parent),
DCOPObject(QCString(parent->name())+".plugin_loading"),m_instance(inst)
{
}
const QMap< QString, QString > &KWSerialLetterDataSource::getRecordEntries() const {return sampleRecord;}
