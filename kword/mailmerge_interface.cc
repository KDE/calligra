#include "mailmerge_interface.h"
#include "mailmerge_interface.moc"

KWMailMergeDataSource::KWMailMergeDataSource(KInstance* inst,QObject *parent): QObject(parent),
DCOPObject(QCString(parent->name())+".plugin_loading"),m_instance(inst)
{
}
const QMap< QString, QString > &KWMailMergeDataSource::getRecordEntries() const {return sampleRecord;}
