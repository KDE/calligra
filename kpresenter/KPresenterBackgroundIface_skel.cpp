#include <KPresenterBackgroundIface.h>

bool KPresenterBackgroundIface::process(const QCString &fun, const QByteArray &data, QCString& replyType, QByteArray &replyData)
{
	if ( fun == "getBackType()" )
	{
		replyType = "int";
		QDataStream out( replyData, IO_WriteOnly );
		out << getBackType( );
		return TRUE;
	}
	if ( fun == "getBackView()" )
	{
		replyType = "int";
		QDataStream out( replyData, IO_WriteOnly );
		out << getBackView( );
		return TRUE;
	}
	if ( fun == "getBackColor1()" )
	{
		replyType = "QColor";
		QDataStream out( replyData, IO_WriteOnly );
		out << getBackColor1( );
		return TRUE;
	}
	if ( fun == "getBackColor2()" )
	{
		replyType = "QColor";
		QDataStream out( replyData, IO_WriteOnly );
		out << getBackColor2( );
		return TRUE;
	}
	if ( fun == "getBackColorType()" )
	{
		replyType = "int";
		QDataStream out( replyData, IO_WriteOnly );
		out << getBackColorType( );
		return TRUE;
	}
	if ( fun == "getBackPixFilename()" )
	{
		replyType = "QString";
		QDataStream out( replyData, IO_WriteOnly );
		out << getBackPixFilename( );
		return TRUE;
	}
	if ( fun == "getBackClipFilename()" )
	{
		replyType = "QString";
		QDataStream out( replyData, IO_WriteOnly );
		out << getBackClipFilename( );
		return TRUE;
	}
	if ( fun == "getPageEffect()" )
	{
		replyType = "int";
		QDataStream out( replyData, IO_WriteOnly );
		out << getPageEffect( );
		return TRUE;
	}
	if ( fun == "getBackUnbalanced()" )
	{
		replyType = "bool";
		QDataStream out( replyData, IO_WriteOnly );
		out << getBackUnbalanced( );
		return TRUE;
	}
	if ( fun == "getBackXFactor()" )
	{
		replyType = "int";
		QDataStream out( replyData, IO_WriteOnly );
		out << getBackXFactor( );
		return TRUE;
	}
	if ( fun == "getBackYFactor()" )
	{
		replyType = "int";
		QDataStream out( replyData, IO_WriteOnly );
		out << getBackYFactor( );
		return TRUE;
	}
	if ( fun == "getSize()" )
	{
		replyType = "QSize";
		QDataStream out( replyData, IO_WriteOnly );
		out << getSize( );
		return TRUE;
	}
	if ( DCOPObject::process( fun, data, replyType, replyData ) )
		return TRUE;
	return FALSE;
}

