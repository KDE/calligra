#include "kscript_value.h"
#include "kscript_context.h"
#include "kscript_qobject.h"
#include "kscript_util.h"
#include "kscript_qstructs.h"
#include "kscript_object.h"

#include <qmetaobject.h>
#include <qvariant.h>

/***************************************************
 *
 * KSQObject
 *
 ***************************************************/

KSQObject::KSQObject( QObject* obj )
    : QShared(), m_ptr( obj )
{
}

KSQObject::~KSQObject()
{
}

KSValue::Ptr KSQObject::member( KSContext& context, const QString& prop )
{
    if ( m_ptr.isNull() )
    {
	context.setException( new KSException( "NullPointer", "QObject already dead" ) );
	return KSValue::Ptr( 0 );
    }

    QMetaObject* meta = m_ptr->metaObject();
    ASSERT( meta );
    const QMetaProperty *property = meta->property( prop, TRUE );
    if ( !property )
    {
	QString tmp( "Unknown property '%1' in object of class '%2'" );
	context.setException( new KSException( "UnknownName", tmp.arg( prop ).arg( m_ptr->className() ) ) );
	return KSValue::Ptr( 0 );
    }

    if ( context.leftExpr() )
    {
	this->ref();
	KSValue::Ptr ptr( new KSValue( new KSProperty( this, prop ) ) );
	ptr->setMode( KSValue::LeftExpr );
	return ptr;
    }

    QVariant var = m_ptr->property( prop );
    return unpack( context, var );
}

bool KSQObject::setMember( KSContext& context, const QString& prop, const KSValue::Ptr& v )
{
    if ( m_ptr.isNull() )
    {
	context.setException( new KSException( "NullPointer", "QObject already dead" ) );
	return FALSE;
    }

    QMetaObject* meta = m_ptr->metaObject();
    ASSERT( meta );
    const QMetaProperty *property = meta->property( prop, TRUE );
    if ( property )
    {
	QVariant var;
	if ( !pack( context, var, v ) )
        {
	    QString tmp( "Unknown property '%1' in object of class '%2'" );
	    context.setException( new KSException( "UnknownName", tmp.arg( prop ).arg( m_ptr->className() ) ) );
	    return FALSE;
	}
	m_ptr->setProperty( prop, var );
	return TRUE;
    }
    else
    {
	QString tmp( "Unknown property '%1' in object of class '%2'" );
	context.setException( new KSException( "UnknownName", tmp.arg( prop ).arg( m_ptr->className() ) ) );
	return FALSE;
    }

    return TRUE;
}

bool KSQObject::pack( KSContext& context, QVariant& var, const KSValue::Ptr& v )
{
    switch( v->type() )
    {
    case KSValue::StringType:
	var = QVariant( v->stringValue() );
	return TRUE;
    case KSValue::IntType:
	var = QVariant( (int)v->intValue() );
	return TRUE;
    case KSValue::BoolType:
	var = QVariant( v->boolValue() );
	return TRUE;
    case KSValue::DoubleType:
	var = QVariant( v->doubleValue() );
	return TRUE;
    case KSValue::StructType:
	{
	    const KSStruct* s = v->structValue();
	    if ( s->getClass()->fullName() == "qt:QRect" )
            {
		var = QVariant( KSQt::Rect::convert( context, v ) );
		return TRUE;
	    }
	    if ( s->getClass()->fullName() == "qt:QPoint" )
            {
		var = QVariant( KSQt::Point::convert( context, v ) );
		return TRUE;
	    }
	    if ( s->getClass()->fullName() == "qt:QSize" )
            {
		var = QVariant( KSQt::Size::convert( context, v ) );
		return TRUE;
	    }
	}
	// TODO: Give error
	break;
    case KSValue::ProxyType:
    case KSValue::CharRefType:
    case KSValue::CharType:
    case KSValue::ListType:
    case KSValue::MapType:
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
      ASSERT( 0 );
    }

    return FALSE;
}

KSValue::Ptr KSQObject::unpack( KSContext& context, QVariant& var )
{
    if ( var.type() == QVariant::String )
    {
	return new KSValue( var.toString() );
    }
    if ( var.type() == QVariant::Int )
    {
	return new KSValue( var.toInt() );
    }
    if ( var.type() == QVariant::Double )
    {
	return new KSValue( var.toDouble() );
    }
    if ( var.type() == QVariant::Bool )
    {
	return new KSValue( var.toBool() );
    }
    if ( var.type() == QVariant::StringList )
    {
	QStringList lst = var.toStringList();
	KSValue* v = new KSValue( KSValue::ListType );
	QStringList::ConstIterator it = lst.begin();
	for( ; it != lst.end(); ++it )
	    v->listValue().append( new KSValue( *it ) );
	return v;
    }
    if ( var.type() == QVariant::Rect )
    {
	return KSQt::Rect::convert( context, var.toRect() );
    }
    if ( var.type() == QVariant::Size )
    {
	return KSQt::Size::convert( context, var.toSize() );
    }
    if ( var.type() == QVariant::Point )
    {
	return KSQt::Point::convert( context, var.toPoint() );
    }

    QString e( "KScript does not understand the property type %1" );
    e = e.arg( var.typeName() );
    context.setException( new KSException( "Unsupported", e ) );
	
    return KSValue::Ptr( 0 );
}

const char* KSQObject::className() const
{
    if ( m_ptr.isNull() )
	return 0;

    return m_ptr->className();
}

/*
bool KSQObject::call( KSContext& context, const QString& name )
{
    qDebug("Call %s\n",name.latin1() );

    return TRUE;
}
*/
