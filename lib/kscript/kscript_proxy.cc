/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kscript_proxy.h"
#include "kscript_struct.h"
#include "kscript_util.h"
#include "kscript_object.h"
#include "kscript_qstructs.h"
#include "kscript_ext_qt.h"

#include <dcopclient.h>
#include <kapplication.h>

#include <qcstring.h>
#include <qstring.h>
#include <qdatastream.h>
#include <qvariant.h>
#include <klocale.h>
#include <kglobal.h>
#include <kdebug.h>

/***************************************************
 *
 * KSProxy
 *
 ***************************************************/

KSProxy::KSProxy( const QCString& app, const QCString& obj )
    : QShared(), m_app( app ), m_obj( obj )
{
  m_propertyProxyCheckDone = false;
  m_supportsPropertyProxy = false;

  // Check for wildcards in the app or the obj name
  if ( m_app.find('*') != -1 || m_obj.find('*') != -1 )
  {
    QByteArray data;
    if ( ! kapp->dcopClient()->findObject( app, obj, "", data, m_app, m_obj ) )
      kdWarning() << "Can't find Application '" << m_app << "' Object '" << m_obj << "'" << endl;
    else
      kdDebug() << " Application=" << m_app << " Object=" << m_obj << endl;
  }
}

KSProxy::~KSProxy()
{
}

KSValue::Ptr KSProxy::member( KSContext& context, const QString& /*name*/ )
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

    static const QString &setPropMethod = KGlobal::staticQString( "setProperty" );
    static const QString &getPropMethod = KGlobal::staticQString( "property" );

    if ( ( name == setPropMethod ||
	 name == getPropMethod ) && !m_propertyProxyCheckDone )
    {
      m_supportsPropertyProxy = false;
      QByteArray data;
      QByteArray reply;
      QCString replyType;
      if ( kapp->dcopClient()->call( m_app, m_obj, QCString( "functions()" ), data, replyType, reply ) &&
	   replyType == "QCString" )
      {
        QCString functions;
	QDataStream stream( reply, IO_ReadOnly );
	stream >> functions;

	m_supportsPropertyProxy = functions.contains( "property(QCString);" ) &&
				  functions.contains( "setProperty(QCString,QVariant);" ) &&
				  functions.contains( "propertyNames(bool);" );
      }

      m_propertyProxyCheckDone = true;
    }

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

	    if ( m_supportsPropertyProxy && first && ( name == getPropMethod || name == setPropMethod ) )
	    {
	      func += "QCString";
	      str << QCString( (*it)->stringValue().latin1() );
	    }
	    else if ( m_supportsPropertyProxy && !first && name == setPropMethod )
	    {
	      func += "QVariant";
	      QVariant var;
	      KS_Qt_Object::pack( context, var, *it );
	      str << var;
	    }
	    else
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
	QString tmp( i18n("Could not call method %1 in app %2, object %3") );
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
    case KSValue::TimeType:
	str << v->timeValue();
	return "QTime";
    case KSValue::DateType:
	str << v->timeValue();
	return "QDate";
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
    case KSValue::BuiltinMethodType:
    case KSValue::ProxyBuiltinMethodType:
    case KSValue::StructBuiltinMethodType:
    case KSValue::Empty:
      // TODO: Give error
      break;
    case KSValue::NTypes:
      Q_ASSERT( 0 );
    }

    return QString::null;
}

KSValue::Ptr KSProxy::unpack( KSContext& context, QDataStream& str, const QCString& type )
{
    if ( type == "QDate" )
    {
	QDate x;
	str >> x;
	return KSValue::Ptr( new KSValue( x ) );
    }
    if ( type == "QTime" )
    {
	QTime x;
	str >> x;
	return KSValue::Ptr( new KSValue( x ) );
    }
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
    if ( type == "QCStringList" )
    {
        QValueList<QCString> lst;
	str >> lst;
	KSValue *v = new KSValue( KSValue::ListType );
	QValueList<QCString>::ConstIterator it = lst.begin();
	QValueList<QCString>::ConstIterator end = lst.end();
	for (; it != end; ++it )
  	  v->listValue().append( new KSValue( QString::fromLatin1( *it ) ) );
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
    if ( type == "QVariant" )
    {
        QVariant var;
	str >> var;
	return KS_Qt_Object::unpack( context, var );
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
