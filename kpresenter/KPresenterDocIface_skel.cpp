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
	if ( DCOPObject::process( fun, data, replyType, replyData ) )
		return TRUE;
	return FALSE;
}

