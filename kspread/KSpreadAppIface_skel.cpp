#include <KSpreadAppIface.h>

bool KSpreadAppIface::process(const QCString &fun, const QByteArray &data, QCString& replyType, QByteArray &replyData)
{
	if ( fun == "createDoc()" )
	{
		replyType = "DCOPRef";
		QDataStream out( replyData, IO_WriteOnly );
		out << createDoc( );
		return TRUE;
	}
	if ( fun == "createDoc(QString)" )
	{
		QDataStream str( data, IO_ReadOnly );
		QString name;
		str >> name;
		replyType = "DCOPRef";
		QDataStream out( replyData, IO_WriteOnly );
		out << createDoc(name );
		return TRUE;
	}
	if ( fun == "documents()" )
	{
		replyType = "QMap<QString,DCOPRef>";
		QDataStream out( replyData, IO_WriteOnly );
		out << documents( );
		return TRUE;
	}
	if ( fun == "document(QString)" )
	{
		QDataStream str( data, IO_ReadOnly );
		QString name;
		str >> name;
		replyType = "DCOPRef";
		QDataStream out( replyData, IO_WriteOnly );
		out << document(name );
		return TRUE;
	}
	if ( DCOPObject::process( fun, data, replyType, replyData ) )
		return TRUE;
	return FALSE;
}

