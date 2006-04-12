#include "KWMailMergeDataSource.h"
#include "KWMailMergeDataSource.moc"

KWMailMergeDataSource::KWMailMergeDataSource(KInstance* inst,QObject *parent): QObject(parent),
DCOPObject(DCOPCString(parent->name())+".plugin_loading"),m_instance(inst)
{
}
const QMap< QString, QString > &KWMailMergeDataSource::getRecordEntries() const {return sampleRecord;}
