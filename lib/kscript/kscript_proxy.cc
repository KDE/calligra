#include "kscript_proxy.h"
#include "kscript_struct.h"
#include "kscript_util.h"
#include "kscript_object.h"
#include "kscript_ext_qstructs.h"

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
      {
	  QString ret = "QValueList<";
	  uint count = v->listValue().count();
	  str << count;
	  if ( count )
          {
	      QValueList<KSValue::Ptr>::Iterator begin = v->listValue().begin();
	      QValueList<KSValue::Ptr>::Iterator end = v->listValue().end();
	
	      QString ret2 = pack( context, str, *begin );
	      if ( ret2.isEmpty() )
		  return QString::null;
	      ret += ret2;
	      ++begin;
	      for( ; begin != end; ++begin )
		  pack( context, str, *begin );
	      ret += ">";
	  }
	  else
	      ret += "*>";
	  return ret;
      }
      break;
    case KSValue::MapType:
      {
	  QString ret = "QMap<QString,";
	  uint count = v->mapValue().count();
	  str << count;
	  if ( count )
          {
	      QMap<QString,KSValue::Ptr>::Iterator begin = v->mapValue().begin();
	      QMap<QString,KSValue::Ptr>::Iterator end = v->mapValue().end();
	
	      str << begin.key();
	      QString ret2 = pack( context, str, begin.data() );
	      if ( ret2.isEmpty() )
		  return QString::null;
	      ret += ret2;
	      ++begin;
	      for( ; begin != end; ++begin )
	      {
		  str << begin.key();
		  pack( context, str, begin.data() );
	      }
	      ret += ">";
	  }
	  else
	      ret += "*>";
	  return ret;
      }
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
	    if ( s->getClass()->fullName() == "qt:QRect" )
            {
		str << KSQt::Rect::convert( context, v );
		return "QRect";
	    }
	    if ( s->getClass()->fullName() == "qt:QPoint" )
            {
		str << KSQt::Point::convert( context, v );
		return "QPoint";
	    }
	    if ( s->getClass()->fullName() == "qt:QSize" )
            {
		str << KSQt::Size::convert( context, v );
		return "QSize";
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

KSValue::Ptr KSProxy::unpack( KSContext& context, QDataStream& str, const QCString& type )
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
    if ( type == "QRect" )
    {
	QRect rect;
	str >> rect;
	return KSQt::Rect::convert( context, rect );
    }
    if ( type == "QSize" )
    {
	QSize size;
	str >> size;
	return KSQt::Size::convert( context, size );
    }
    if ( type == "QPoint" )
    {
	QPoint p;
	str >> p;
	return KSQt::Point::convert( context, p );
    }
    if ( type.left( 11 ) == "QValueList<" )
    {
	KSValue* v = new KSValue( KSValue::ListType );
	uint count;
	str >> count;
	QCString t = type.mid( 11, type.length() - 12 );
	for( uint i = 0; i < count; ++i )
	    v->listValue().append( unpack( context, str, t ) );
	return v;
    }
    if ( type.left( 13 ) == "QMap<QString," )
    {
	KSValue* v = new KSValue( KSValue::MapType );
	uint count;
	str >> count;
	QCString t = type.mid( 13, type.length() - 14 );
	for( uint i = 0; i < count; ++i )
        {
	    QString key;
	    str >> key;
	    v->mapValue().insert( key, unpack( context, str, t ) );
	}
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
