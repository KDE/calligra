#include "kexidragobjects.h"

#include <qcstring.h>
#include <qdatastream.h>
#include <kdebug.h>

/// implementation of KexiFieldDrag
              
KexiFieldDrag::KexiFieldDrag(const QString& sourceType, const QString& source,const QString& field,
                QWidget *parent, const char *name)
 : QStoredDrag("kexi/field", parent, name)
{
       QByteArray data;
       QDataStream stream1(data,IO_WriteOnly);
       stream1<<sourceType<<source<<field;		
       setEncodedData(data);
}


bool
KexiFieldDrag::canDecode(QDragMoveEvent *e)
{
        return e->provides("kexi/field");
}

bool
KexiFieldDrag::decode( QDropEvent* e, QString& sourceType, QString& source, QString& field )
{
	QCString tmp;
        QByteArray payload = e->data("kexi/field");
        if(payload.size())
        {
                e->accept();
		QDataStream stream1(payload,IO_ReadOnly);
		stream1>>sourceType;
		stream1>>source;
		stream1>>field;
                kdDebug() << "KexiFieldDrag::decode() decoded: " << sourceType<<"/"<<source<<"/"<<field << endl;
                return true;
        }
        return false;
}


/// implementation of KexiDataProviderDrag
              
KexiDataProviderDrag::KexiDataProviderDrag(const QString& sourceType, const QString& source,
                QWidget *parent, const char *name)
 : QStoredDrag("kexi/dataprovider", parent, name)
{
       QByteArray data;
       QDataStream stream1(data,IO_WriteOnly);
       stream1<<sourceType<<source;		
       setEncodedData(data);
}


bool
KexiDataProviderDrag::canDecode(QDragMoveEvent *e)
{
        return e->provides("kexi/dataprovider");
}

bool
KexiDataProviderDrag::decode( QDropEvent* e, QString& sourceType, QString& source)
{
	QCString tmp;
        QByteArray payload = e->data("kexidataprovider");
        if(payload.size())
        {
                e->accept();
		QDataStream stream1(payload,IO_ReadOnly);
		stream1>>sourceType;
		stream1>>source;
                kdDebug() << "KexiDataProviderDrag::decode() decoded: " << sourceType<<"/"<<source<< endl;
                return true;
        }
        return false;
}

