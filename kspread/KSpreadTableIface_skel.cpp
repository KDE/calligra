#include <KSpreadTableIface.h>

bool KSpreadTableIface::process(const QCString &fun, const QByteArray &data, QCString& replyType, QByteArray &replyData)
{
	if ( fun == "cell(int,int)" )
	{
		QDataStream str( data, IO_ReadOnly );
		int x;
		str >> x;
		int y;
		str >> y;
		replyType = "DCOPRef";
		QDataStream out( replyData, IO_WriteOnly );
		out << cell(x, y );
		return TRUE;
	}
	if ( fun == "cell(QString)" )
	{
		QDataStream str( data, IO_ReadOnly );
		QString name;
		str >> name;
		replyType = "DCOPRef";
		QDataStream out( replyData, IO_WriteOnly );
		out << cell(name );
		return TRUE;
	}
	if ( fun == "selection()" )
	{
		replyType = "QRect";
		QDataStream out( replyData, IO_WriteOnly );
		out << selection( );
		return TRUE;
	}
	if ( fun == "setSelection(QRect)" )
	{
		QDataStream str( data, IO_ReadOnly );
		QRect selection;
		str >> selection;
		replyType = "void";
		setSelection(selection );
		return TRUE;
	}
	if ( fun == "name()" )
	{
		replyType = "QString";
		QDataStream out( replyData, IO_WriteOnly );
		out << name( );
		return TRUE;
	}
	if ( DCOPObject::process( fun, data, replyType, replyData ) )
		return TRUE;
	return FALSE;
}

