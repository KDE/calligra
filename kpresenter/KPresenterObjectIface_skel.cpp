#include <KPresenterObjectIface.h>

bool KPresenterObjectIface::process(const QCString &fun, const QByteArray &data, QCString& replyType, QByteArray &replyData)
{
	if ( fun == "getType()" )
	{
		replyType = "int";
		QDataStream out( replyData, IO_WriteOnly );
		out << getType( );
		return TRUE;
	}
	if ( fun == "getBoundingRect()" )
	{
		replyType = "QRect";
		QDataStream out( replyData, IO_WriteOnly );
		out << getBoundingRect( );
		return TRUE;
	}
	if ( fun == "isSelected()" )
	{
		replyType = "bool";
		QDataStream out( replyData, IO_WriteOnly );
		out << isSelected( );
		return TRUE;
	}
	if ( fun == "getAngle()" )
	{
		replyType = "float";
		QDataStream out( replyData, IO_WriteOnly );
		out << getAngle( );
		return TRUE;
	}
	if ( fun == "getShadowDistance()" )
	{
		replyType = "int";
		QDataStream out( replyData, IO_WriteOnly );
		out << getShadowDistance( );
		return TRUE;
	}
	if ( fun == "getShadowDirection()" )
	{
		replyType = "int";
		QDataStream out( replyData, IO_WriteOnly );
		out << getShadowDirection( );
		return TRUE;
	}
	if ( fun == "getShadowColor()" )
	{
		replyType = "QColor";
		QDataStream out( replyData, IO_WriteOnly );
		out << getShadowColor( );
		return TRUE;
	}
	if ( fun == "getSize()" )
	{
		replyType = "QSize";
		QDataStream out( replyData, IO_WriteOnly );
		out << getSize( );
		return TRUE;
	}
	if ( fun == "getOrig()" )
	{
		replyType = "QPoint";
		QDataStream out( replyData, IO_WriteOnly );
		out << getOrig( );
		return TRUE;
	}
	if ( fun == "getEffect()" )
	{
		replyType = "int";
		QDataStream out( replyData, IO_WriteOnly );
		out << getEffect( );
		return TRUE;
	}
	if ( fun == "getEffect2()" )
	{
		replyType = "int";
		QDataStream out( replyData, IO_WriteOnly );
		out << getEffect2( );
		return TRUE;
	}
	if ( fun == "getPresNum()" )
	{
		replyType = "int";
		QDataStream out( replyData, IO_WriteOnly );
		out << getPresNum( );
		return TRUE;
	}
	if ( fun == "getSubPresSteps()" )
	{
		replyType = "int";
		QDataStream out( replyData, IO_WriteOnly );
		out << getSubPresSteps( );
		return TRUE;
	}
	if ( fun == "getDisappear()" )
	{
		replyType = "bool";
		QDataStream out( replyData, IO_WriteOnly );
		out << getDisappear( );
		return TRUE;
	}
	if ( fun == "getDisappearNum()" )
	{
		replyType = "int";
		QDataStream out( replyData, IO_WriteOnly );
		out << getDisappearNum( );
		return TRUE;
	}
	if ( fun == "getEffect3()" )
	{
		replyType = "int";
		QDataStream out( replyData, IO_WriteOnly );
		out << getEffect3( );
		return TRUE;
	}
	if ( DCOPObject::process( fun, data, replyType, replyData ) )
		return TRUE;
	return FALSE;
}

