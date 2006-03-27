#include "KWMailMergeDataSource.h"
#include "KWMailMergeDataSource.moc"
//Added by qt3to4:
#include <Q3CString>

KWMailMergeDataSource::KWMailMergeDataSource(KInstance* inst,QObject *parent): QObject(parent),
DCOPObject(Q3CString(parent->name())+".plugin_loading"),m_instance(inst)
{
}
const QMap< QString, QString > &KWMailMergeDataSource::getRecordEntries() const {return sampleRecord;}
