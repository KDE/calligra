#include "kscript_ext_qstructs.h"
#include "kscript_util.h"
#include "kscript.h"

/**************************************************
 *
 * QRect
 *
 **************************************************/

static bool qrect_isNull( void* object, KSContext& context, const QValueList<KSValue::Ptr>& )
{
    QRect* rect = (QRect*)object;
    context.setValue( new KSValue( rect->isNull() ) );
    return TRUE;
}

static bool qrect_isValid( void* object, KSContext& context, const QValueList<KSValue::Ptr>& )
{
    QRect* rect = (QRect*)object;
    context.setValue( new KSValue( rect->isValid() ) );
    return TRUE;
}

static bool qrect_isEmpty( void* object, KSContext& context, const QValueList<KSValue::Ptr>& )
{
    QRect* rect = (QRect*)object;
    context.setValue( new KSValue( rect->isEmpty() ) );
    return TRUE;
}

static bool qrect_normalize( void* object, KSContext& context, const QValueList<KSValue::Ptr>& )
{
    QRect* rect = (QRect*)object;
    context.setValue( KSQt::Rect::convert( context, rect->normalize() ) );
    return TRUE;
}

static bool qrect_contains( void* object, KSContext& context, const QValueList<KSValue::Ptr>& args )
{
    QRect* rect = (QRect*)object;
    context.setValue( new KSValue( rect->contains( KSQt::Rect::convert( context, args[0] ),
						   args.count() == 1 ? FALSE : args[1]->boolValue() ) ) );
    return TRUE;
}

static bool qrect_unite( void* object, KSContext& context, const QValueList<KSValue::Ptr>& args )
{
    QRect* rect = (QRect*)object;
    context.setValue( KSQt::Rect::convert( context, rect->unite( KSQt::Rect::convert( context, args[0] ) ) ) );
    return TRUE;
}

static bool qrect_intersect( void* object, KSContext& context, const QValueList<KSValue::Ptr>& args )
{
    QRect* rect = (QRect*)object;
    context.setValue( KSQt::Rect::convert( context, rect->intersect( KSQt::Rect::convert( context, args[0] ) ) ) );
    return TRUE;
}

static bool qrect_intersects( void* object, KSContext& context, const QValueList<KSValue::Ptr>& args )
{
    QRect* rect = (QRect*)object;
    context.setValue( new KSValue( rect->contains( KSQt::Rect::convert( context, args[0] ) ) ) );
    return TRUE;
}

QRect KSQt::Rect::convert( KSContext&, const KSValue::Ptr& v )
{
    return *((const QRect*)v->structValue()->object());
}

KSValue::Ptr KSQt::Rect::convert( KSContext& context, const QRect& rect )
{
    KSValue::Ptr s = context.interpreter()->module( "qt" )->member( context, "QRect" );
    ASSERT( !s.isNull() );

    return new KSValue( new KSBuiltinStruct( s->structClassValue(), new QRect( rect ) ) );
}

KSQt::Rect::Rect( KSModule* module, const QString& name )
    : KSBuiltinStructClass( module, name )
{
    addVariable( "left" );
    addVariable( "right" );
    addVariable( "top" );
    addVariable( "bottom" );
    addVariable( "width" );
    addVariable( "height" );

    addMethod( "isNull", qrect_isNull, "" );
    addMethod( "isValid", qrect_isValid, "" );
    addMethod( "isEmpty", qrect_isEmpty, "" );
    addMethod( "normalize", qrect_normalize, "" );
    addMethod( "contains", qrect_contains, "sqt:QRect;|b" );
    addMethod( "unite", qrect_unite, "sqt:QRect;" );
    addMethod( "intersect", qrect_intersect, "sqt:QRect;" );
    addMethod( "intersects", qrect_intersects, "sqt:QRect;" );
}

bool KSQt::Rect::constructor( KSContext& c )
{
    // qDebug("STRUCT constructor");
    QValueList<KSValue::Ptr>& args = c.value()->listValue();
    // TODO: check params

    c.setValue( new KSValue( new KSBuiltinStruct( this, new QRect() ) ) );

    return TRUE;
}

bool KSQt::Rect::destructor( void* object )
{
    // qDebug("STRUCT destr l=%i r=%i", ((QRect*)object)->left(),((QRect*)object)->right());
    delete ( (QRect*)object );

    return TRUE;
}

KSStruct* KSQt::Rect::clone( KSBuiltinStruct* s )
{
    // qDebug("STRUCT clone l=%i r=%i", ((QRect*)s->object())->left(),((QRect*)s->object())->right());
    return new KSBuiltinStruct( this, new QRect( *((QRect*)s->object()) ) );
}

KSValue::Ptr KSQt::Rect::property( void* object, const QString& name )
{
    QRect* rect = (QRect*)object;
	
    if ( name == "left" )
	return new KSValue( rect->left() );
    if ( name == "right" )
	return new KSValue( rect->right() );
    if ( name == "top" )
	return new KSValue( rect->top() );
    if ( name == "bottom" )
	return new KSValue( rect->bottom() );
    if ( name == "width" )
	return new KSValue( rect->width() );
    if ( name == "height" )
	return new KSValue( rect->height() );

    return 0;
}

bool KSQt::Rect::setProperty( KSContext& context, void* object, const QString& name, const KSValue::Ptr value )
{
    QRect* rect = (QRect*)object;

    if ( name == "left" )
    {
	if ( !KSUtil::checkType( context, value, KSValue::IntType ) )
	    return FALSE;
	rect->setLeft( value->intValue() );
	return TRUE;
    }
    if ( name == "right" )
    {
	if ( !KSUtil::checkType( context, value, KSValue::IntType ) )
	    return FALSE;
	rect->setRight( value->intValue() );
	return TRUE;
    }
    if ( name == "top" )
    {
	if ( !KSUtil::checkType( context, value, KSValue::IntType ) )
	    return FALSE;
	rect->setTop( value->intValue() );
	return TRUE;
    }
    if ( name == "bottom" )
    {
	if ( !KSUtil::checkType( context, value, KSValue::IntType ) )
	    return FALSE;
	rect->setBottom( value->intValue() );
	return TRUE;
    }
    if ( name == "width" )
    {
	if ( !KSUtil::checkType( context, value, KSValue::IntType ) )
	    return FALSE;
	rect->setWidth( value->intValue() );
	return TRUE;
    }
    if ( name == "height" )
    {
	if ( !KSUtil::checkType( context, value, KSValue::IntType ) )
	    return FALSE;
	rect->setHeight( value->intValue() );
	return TRUE;
    }

    // No need to give an error here.
    return FALSE;
}
