#include <KPresenterDocIface.h>

bool KPresenterDocIface::process(const QCString &fun, const QByteArray &data, QCString& replyType, QByteArray &replyData)
{
	if ( fun == "url()" )
	{
		replyType = "QString";
		QDataStream out( replyData, IO_WriteOnly );
		out << url( );
		return TRUE;
	}
	if ( fun == "firstView()" )
	{
		replyType = "DCOPRef";
		QDataStream out( replyData, IO_WriteOnly );
		out << firstView( );
		return TRUE;
	}
	if ( fun == "nextView()" )
	{
		replyType = "DCOPRef";
		QDataStream out( replyData, IO_WriteOnly );
		out << nextView( );
		return TRUE;
	}
	if ( fun == "getNumObjects()" )
	{
		replyType = "int";
		QDataStream out( replyData, IO_WriteOnly );
		out << getNumObjects( );
		return TRUE;
	}
	if ( fun == "getObject(int)" )
	{
		QDataStream str( data, IO_ReadOnly );
		int num;
		str >> num;
		replyType = "DCOPRef";
		QDataStream out( replyData, IO_WriteOnly );
		out << getObject(num );
		return TRUE;
	}
	if ( fun == "getNumPages()" )
	{
		replyType = "int";
		QDataStream out( replyData, IO_WriteOnly );
		out << getNumPages( );
		return TRUE;
	}
	if ( fun == "getPage(int)" )
	{
		QDataStream str( data, IO_ReadOnly );
		int num;
		str >> num;
		replyType = "DCOPRef";
		QDataStream out( replyData, IO_WriteOnly );
		out << getPage(num );
		return TRUE;
	}
	if ( DCOPObject::process( fun, data, replyType, replyData ) )
		return TRUE;
	return FALSE;
}

