#include "kscript_proxy.h"
#include "kscript_struct.h"
#include "kscript_util.h"
#include "kscript_object.h"

#include <dcopclient.h>
#include <kapp.h>

#include <qcstring.h>
#include <qstring.h>
#include <qdatastream.h>

/***************************************************
 *
 * KSProxy
 *
 ***************************************************/

KSProxy::KSProxy( const QCString& app, const QCString& obj )
    : QShared(), m_app( app ), m_obj( obj )
{
}

KSProxy::~KSProxy()
{
}

KSValue::Ptr KSProxy::member( KSContext& context, const QString& name )
{
    if ( context.leftExpr() )
    {
	return KSValue::Ptr( 0 );
    }

    KSValue* v = new KSValue( (KSProxyBuiltinMethod)&KSProxy::call );

    return KSValue::Ptr( v );
}

bool KSProxy::setMember( KSContext&, const QString&, const KSValue::Ptr& )
{
    return FALSE;
}

bool KSProxy::call( KSContext& context, const QString& name )
{
    qDebug("Call %s\n",name.latin1() );

    QByteArray data;
    QByteArray reply;
    QString func( name );
    func += "(";
    QValueList<KSValue::Ptr>& args = context.value()->listValue();
    {
	bool first = TRUE;
	QDataStream str( data, IO_WriteOnly );
	QValueList<KSValue::Ptr>::Iterator it = args.begin();
	for( ; it != args.end(); ++it )
        {
	    if ( !first )
		func += ",";
	    func += pack( context, str, *it );
	    if ( context.exception() )
		return FALSE;
	    first = FALSE;
	}
    }
    func += ")";

    qDebug("Calling %s", func.latin1() );

    QCString replyType;
    if ( !kapp->dcopClient()->call( m_app, m_obj, func.latin1(), data, replyType, reply ) )
    {
	QString tmp( "Could not call method %1 in app %2, object %3" );
	context.setException( new KSException( "UnknownName", tmp.arg( func.latin1() ).arg( m_app ).arg( m_obj ) ) );
	return FALSE;
    }

    if ( replyType != "void" )
    {
	QDataStream str( reply, IO_ReadOnly );
	KSValue::Ptr ptr = unpack( context, str, replyType );
	context.setValue( ptr );
    }
    else
	context.setValue( 0 );

    return TRUE;
}

QString KSProxy::pack( KSContext& context, QDataStream& str, KSValue::Ptr& v )
{
    switch( v->type() )
    {
    case KSValue::StringType:
	str << v->stringValue();
	return "QString";
    case KSValue::ListType:
	// TODO
      break;
    case KSValue::MapType:
	// TODO
      break;
    case KSValue::CharRefType:
    case KSValue::CharType:
	str << (Q_INT8)v->charValue();
	return "char";
    case KSValue::ProxyType:
	str << v->proxyValue()->appId() << v->proxyValue()->objId();
	return "DCOPRef";
    case KSValue::IntType:
	str << (Q_INT32)v->intValue();
	return "int";	
    case KSValue::BoolType:
	str << (Q_INT8)v->boolValue();
	return "bool";
    case KSValue::DoubleType:
	str << v->doubleValue();
	return "double";
    case KSValue::StructType:
	{
	    KSStruct* s = v->structValue();
	    if ( s->getClass()->name() == "QRect" )
            {
		str << KSUtil::toQRect( context, s );
		return "QRect";
	    }
	}
	// TODO: Give error
	break;
    case KSValue::FunctionType:
    case KSValue::ClassType:
    case KSValue::ObjectType:
    case KSValue::MethodType:
    case KSValue::PropertyType:
    case KSValue::ModuleType:
    case KSValue::StructClassType:
    case KSValue::TypeCodeType:
    case KSValue::InterfaceType:
    case KSValue::AttributeType:
    case KSValue::BuiltinMethodType:
    case KSValue::ProxyBuiltinMethodType:
    case KSValue::StructBuiltinMethodType:
    case KSValue::Empty:
      // TODO: Give error
      break;
    case KSValue::NTypes:
      ASSERT( 0 );
    }

    return QString::null;
}

KSValue::Ptr KSProxy::unpack( KSContext&, QDataStream& str, const QCString& type )
{
    if ( type == "QString" )
    {
	QString x;
	str >> x;
	return KSValue::Ptr( new KSValue( x ) );
    }
    if ( type == "QCString" )
    {
	QCString x;
	str >> x;
	return new KSValue( QString( x.data() ) );
    }
    if ( type == "char" )
    {
	Q_INT8 x;
	str >> x;
	return new KSValue( (KScript::Char)x );
    }
    if ( type == "int" )
    {
	Q_INT32 x;
	str >> x;
	return new KSValue( (KScript::Long)x );
    }
    if ( type == "double" )
    {
	double x;
	str >> x;
	return new KSValue( (KScript::Double)x );
    }
    if ( type == "float" )
    {
	float x;
	str >> x;
	return new KSValue( (KScript::Double)x );
    }
    if ( type == "bool" )
    {
	Q_INT8 x;
	str >> x;
	return new KSValue( (KScript::Boolean)x );
    }
    if ( type == "DCOPRef" )
    {
	QCString app, obj;
	str >> app >> obj;
	return new KSValue( new KSProxy( app, obj ) );
    }
    if ( type == "QStringList" )
    {
	QStringList lst;
	str >> lst;
	KSValue* v = new KSValue( KSValue::ListType );
	QStringList::ConstIterator it = lst.begin();
	for( ; it != lst.end(); ++it )
	    v->listValue().append( new KSValue( *it ) );
	return v;
    }

    return KSValue::Ptr( 0 );
}

QCString KSProxy::appId() const
{
    return m_app;
}

QCString KSProxy::objId() const
{
    return m_obj;
}
