#include <KSpreadCellIface.h>

bool KSpreadCellIface::process(const QCString &fun, const QByteArray &data, QCString& replyType, QByteArray &replyData)
{
	if ( fun == "text()" )
	{
		replyType = "QString";
		QDataStream out( replyData, IO_WriteOnly );
		out << text( );
		return TRUE;
	}
	if ( fun == "setText(QString)" )
	{
		QDataStream str( data, IO_ReadOnly );
		QString text;
		str >> text;
		replyType = "void";
		setText(text );
		return TRUE;
	}
	if ( fun == "setValue(int)" )
	{
		QDataStream str( data, IO_ReadOnly );
		int value;
		str >> value;
		replyType = "void";
		setValue(value );
		return TRUE;
	}
	if ( fun == "setValue(double)" )
	{
		QDataStream str( data, IO_ReadOnly );
		double value;
		str >> value;
		replyType = "void";
		setValue(value );
		return TRUE;
	}
	if ( fun == "value()" )
	{
		replyType = "double";
		QDataStream out( replyData, IO_WriteOnly );
		out << value( );
		return TRUE;
	}
	return FALSE;
}

