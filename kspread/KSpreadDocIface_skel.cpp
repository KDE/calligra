#include <KSpreadDocIface.h>

bool KSpreadDocIface::process(const QCString &fun, const QByteArray &data, QCString& replyType, QByteArray &replyData)
{
	if ( fun == "map()" )
	{
		replyType = "DCOPRef";
		QDataStream out( replyData, IO_WriteOnly );
		out << map( );
		return TRUE;
	}
	if ( fun == "url()" )
	{
		replyType = "QString";
		QDataStream out( replyData, IO_WriteOnly );
		out << url( );
		return TRUE;
	}
	if ( fun == "isModified()" )
	{
		replyType = "bool";
		QDataStream out( replyData, IO_WriteOnly );
		out << isModified( );
		return TRUE;
	}
	if ( fun == "save()" )
	{
		replyType = "bool";
		QDataStream out( replyData, IO_WriteOnly );
		out << save( );
		return TRUE;
	}
	if ( fun == "saveAs(QString)" )
	{
		QDataStream str( data, IO_ReadOnly );
		QString url;
		str >> url;
		replyType = "bool";
		QDataStream out( replyData, IO_WriteOnly );
		out << saveAs(url );
		return TRUE;
	}
	return FALSE;
}

