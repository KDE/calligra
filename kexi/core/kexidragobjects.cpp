#include "kexidragobjects.h"

#include <qcstring.h>
#include <qdatastream.h>
#include <kdebug.h>

/// implementation of KexiFieldDrag
              
KexiFieldDrag::KexiFieldDrag(const QString& source,const QString& field, QWidget *parent)
 : QStoredDrag("kexi/field", parent)
{
       QByteArray data;
//       QDataStream stream1(data,IO_WriteOnly);
 //      stream1 << source << field;
	data = field.utf8();

	setEncodedData(data);
 
	kdDebug() << "KexiFieldDrag::KexiFieldDrag() data: " << QString(data) << endl;
       
	m_field = field;
}


bool
KexiFieldDrag::canDecode(QDragMoveEvent *e)
{
        return e->provides("kexi/field");
}

bool
KexiFieldDrag::decode( QDropEvent* e, QString& sourceType)
{
	kdDebug() << "KexiFieldDrag::decode()" << endl;
	QCString tmp;
        QByteArray payload = e->data("kexi/field");
        if(payload.size())
        {
                e->accept();
//		QDataStream stream1(payload,IO_WriteOnly);
//		stream1>>sourceType;
//		stream1>>source;
//		stream1>>field;
//              kdDebug() << "KexiFieldDrag::decode() decoded: " << sourceType<<"/"<<source<<"/"<<field << endl;
//                return true;
		sourceType = QString(payload);
        	return true;
	}
        return false;
}
