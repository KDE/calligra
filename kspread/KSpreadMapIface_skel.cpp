#include <KSpreadMapIface.h>

bool KSpreadMapIface::process(const QCString &fun, const QByteArray &data, QCString& replyType, QByteArray &replyData)
{
	if ( fun == "table(QString)" )
	{
		QDataStream str( data, IO_ReadOnly );
		QString name;
		str >> name;
		replyType = "DCOPRef";
		QDataStream out( replyData, IO_WriteOnly );
		out << table(name );
		return TRUE;
	}
	if ( fun == "table(int)" )
	{
		QDataStream str( data, IO_ReadOnly );
		int index;
		str >> index;
		replyType = "DCOPRef";
		QDataStream out( replyData, IO_WriteOnly );
		out << table(index );
		return TRUE;
	}
	if ( fun == "tableCount()" )
	{
		replyType = "int";
		QDataStream out( replyData, IO_WriteOnly );
		out << tableCount( );
		return TRUE;
	}
	if ( fun == "tableNames()" )
	{
		replyType = "QStringList";
		QDataStream out( replyData, IO_WriteOnly );
		out << tableNames( );
		return TRUE;
	}
	if ( fun == "tables()" )
	{
		replyType = "QValueList<DCOPRef>";
		QDataStream out( replyData, IO_WriteOnly );
		out << tables( );
		return TRUE;
	}
	return FALSE;
}

