#include "kscript_qstructs.h"
#include "kscript_util.h"
#include "kscript.h"

/**************************************************
 *
 * QRect
 *
 **************************************************/

static bool Rect_bottomLeft( void* object, KSContext& context, const QValueList<KSValue::Ptr>& )
{
    QRect* value = (QRect*)object;
    context.setValue(  KSQt::Point::convert( context, value->bottomLeft( ) ) );
    return TRUE;
}

static bool Rect_bottomRight( void* object, KSContext& context, const QValueList<KSValue::Ptr>& )
{
    QRect* value = (QRect*)object;
    context.setValue(  KSQt::Point::convert( context, value->bottomRight( ) ) );
    return TRUE;
}

static bool Rect_moveBottomLeft( void* object, KSContext& context, const QValueList<KSValue::Ptr>& args )
{
    QRect* value = (QRect*)object;
    value->moveBottomLeft( KSQt::Point::convert( context, args[0] ) );
    context.setValue( 0 );
    return TRUE;
}

static bool Rect_moveBottomRight( void* object, KSContext& context, const QValueList<KSValue::Ptr>& args )
{
    QRect* value = (QRect*)object;
    value->moveBottomRight( KSQt::Point::convert( context, args[0] ) );
    context.setValue( 0 );
    return TRUE;
}

static bool Rect_moveBy( void* object, KSContext& context, const QValueList<KSValue::Ptr>& args )
{
    QRect* value = (QRect*)object;
    value->moveBy( args[0]->intValue(), args[1]->intValue() );
    context.setValue( 0 );
    return TRUE;
}

static bool Rect_moveCenter( void* object, KSContext& context, const QValueList<KSValue::Ptr>& args )
{
    QRect* value = (QRect*)object;
    value->moveCenter( KSQt::Point::convert( context, args[0] ) );
    context.setValue( 0 );
    return TRUE;
}

static bool Rect_moveTopLeft( void* object, KSContext& context, const QValueList<KSValue::Ptr>& args )
{
    QRect* value = (QRect*)object;
    value->moveTopLeft( KSQt::Point::convert( context, args[0] ) );
    context.setValue( 0 );
    return TRUE;
}

static bool Rect_moveTopRight( void* object, KSContext& context, const QValueList<KSValue::Ptr>& args )
{
    QRect* value = (QRect*)object;
    value->moveTopRight( KSQt::Point::convert( context, args[0] ) );
    context.setValue( 0 );
    return TRUE;
}

static bool Rect_setCoords( void* object, KSContext& context, const QValueList<KSValue::Ptr>& args )
{
    QRect* value = (QRect*)object;
    value->setCoords( args[0]->intValue(), args[1]->intValue(), args[2]->intValue(), args[3]->intValue() );
    context.setValue( 0 );
    return TRUE;
}

static bool Rect_setRect( void* object, KSContext& context, const QValueList<KSValue::Ptr>& args )
{
    QRect* value = (QRect*)object;
    value->setRect( args[0]->intValue(), args[1]->intValue(), args[2]->intValue(), args[3]->intValue() );
    context.setValue( 0 );
    return TRUE;
}

static bool Rect_topLeft( void* object, KSContext& context, const QValueList<KSValue::Ptr>& )
{
    QRect* value = (QRect*)object;
    context.setValue( KSQt::Point::convert( context, value->topLeft( ) ) );
    return TRUE;
}

static bool Rect_topRight( void* object, KSContext& context, const QValueList<KSValue::Ptr>& )
{
    QRect* value = (QRect*)object;
    context.setValue(  KSQt::Point::convert( context, value->topRight( ) ) );
    return TRUE;
}

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
    if ( KSUtil::checkArgs( context, "Sqt:QRect;|b", "contains", FALSE ) )
    {
	context.setValue( new KSValue( rect->contains( KSQt::Rect::convert( context, args[0] ),
						       args.count() == 1 ? FALSE : args[1]->boolValue() ) ) );
    }
    else if ( KSUtil::checkArgs( context, "Sqt:QPoint;|b", "contains", FALSE ) )
    {
	context.setValue( new KSValue( rect->contains( KSQt::Point::convert( context, args[0] ),
						       args.count() == 1 ? FALSE : args[1]->boolValue() ) ) );
    }
    else
    {
	KSUtil::argumentsMismatchError( context, "contains" );
	return FALSE;
    }

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
    context.setValue( new KSValue( rect->intersects( KSQt::Rect::convert( context, args[0] ) ) ) );
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
    addVariable( "size" );

    addMethod( "isNull", qrect_isNull, "" );
    addMethod( "isValid", qrect_isValid, "" );
    addMethod( "isEmpty", qrect_isEmpty, "" );
    addMethod( "normalize", qrect_normalize, "" );
    addMethod( "contains", qrect_contains, QCString() );
    addMethod( "unite", qrect_unite, "Sqt:QRect;" );
    addMethod( "intersect", qrect_intersect, "Sqt:QRect;" );
    addMethod( "intersects", qrect_intersects, "Sqt:QRect;" );
    addMethod( "bottomLeft", Rect_bottomLeft, "" );
    addMethod( "bottomRight", Rect_bottomRight, "" );
    addMethod( "moveBottomLeft", Rect_moveBottomLeft, "Sqt:QPoint;" );
    addMethod( "moveBottomRight", Rect_moveBottomRight, "Sqt:QPoint;" );
    addMethod( "moveBy", Rect_moveBy, "ii" );
    addMethod( "moveCenter", Rect_moveCenter, "Sqt:QPoint;" );
    addMethod( "moveTopLeft", Rect_moveTopLeft, "Sqt:QPoint;" );
    addMethod( "moveTopRight", Rect_moveTopRight, "Sqt:QPoint;" );
    addMethod( "setCoords", Rect_setCoords, "iiii" );
    addMethod( "setRect", Rect_setRect, "iiii" );
    addMethod( "topLeft", Rect_topLeft, "" );
    addMethod( "topRight", Rect_topRight, "" );
}

bool KSQt::Rect::constructor( KSContext& context )
{
    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( KSUtil::checkArgs( context, "", "QRect", FALSE ) )
        context.setValue( new KSValue( new KSBuiltinStruct( this, new QRect() ) ) );
    else if ( KSUtil::checkArgs( context, "iiii", "QRect", FALSE ) )
        context.setValue( new KSValue( new KSBuiltinStruct( this, new QRect( args[0]->intValue(), args[1]->intValue(), args[2]->intValue(), args[3]->intValue()) ) ) );
    else if ( KSUtil::checkArgs( context, "Sqt:QPoint;Sqt:QPoint;", "QRect", FALSE ) )
        context.setValue( new KSValue( new KSBuiltinStruct( this, new QRect( KSQt::Point::convert( context, args[0] ), KSQt::Point::convert( context, args[1] )) ) ) );
    else if ( KSUtil::checkArgs( context, "Sqt:QPoint;Sqt:QSize;", "QRect", FALSE ) )
        context.setValue( new KSValue( new KSBuiltinStruct( this, new QRect( KSQt::Point::convert( context, args[0] ), KSQt::Size::convert( context, args[1] )) ) ) );
    else
        KSUtil::argumentsMismatchError( context, "QRect" );

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

KSValue::Ptr KSQt::Rect::property( KSContext& context, void* object, const QString& name )
{
    QRect* value = (QRect*)object;
	
    if ( name == "left" )
	return new KSValue( value->left() );
    if ( name == "right" )
	return new KSValue( value->right() );
    if ( name == "top" )
	return new KSValue( value->top() );
    if ( name == "bottom" )
	return new KSValue( value->bottom() );
    if ( name == "width" )
	return new KSValue( value->width() );
    if ( name == "height" )
	return new KSValue( value->height() );
    if ( name == "size" )
        return KSQt::Size::convert( context, value->size() );

    return 0;
}

bool KSQt::Rect::setProperty( KSContext& context, void* object, const QString& name, const KSValue::Ptr value )
{
    QRect* v = (QRect*)object;

    if ( name == "left" )
    {
	if ( !KSUtil::checkType( context, value, KSValue::IntType ) )
	    return FALSE;
	v->setLeft( value->intValue() );
	return TRUE;
    }
    if ( name == "right" )
    {
	if ( !KSUtil::checkType( context, value, KSValue::IntType ) )
	    return FALSE;
	v->setRight( value->intValue() );
	return TRUE;
    }
    if ( name == "top" )
    {
	if ( !KSUtil::checkType( context, value, KSValue::IntType ) )
	    return FALSE;
	v->setTop( value->intValue() );
	return TRUE;
    }
    if ( name == "bottom" )
    {
	if ( !KSUtil::checkType( context, value, KSValue::IntType ) )
	    return FALSE;
	v->setBottom( value->intValue() );
	return TRUE;
    }
    if ( name == "width" )
    {
	if ( !KSUtil::checkType( context, value, KSValue::IntType ) )
	    return FALSE;
	v->setWidth( value->intValue() );
	return TRUE;
    }
    if ( name == "height" )
    {
	if ( !KSUtil::checkType( context, value, KSValue::IntType ) )
	    return FALSE;
	v->setHeight( value->intValue() );
	return TRUE;
    }
    if ( name == "size" )
    {
        if ( !KSUtil::checkType( context, value, KSValue::StructType ) )
            return FALSE;
        v->setSize( KSQt::Size::convert( context, value ) );
        return TRUE;
    }

    // No need to give an error here.
    return FALSE;
}


/*************************************************************
 *
 * QPoint
 *
 *************************************************************/

static bool Point_isNull( void* object, KSContext& context, const QValueList<KSValue::Ptr>& )
{
    QPoint* value = (QPoint*)object;
    context.setValue(  new KSValue( value->isNull( ) ) );
    return TRUE;
}

QPoint KSQt::Point::convert( KSContext&, const KSValue::Ptr& v )
{
    return *((const QPoint*)v->structValue()->object());
}

KSValue::Ptr KSQt::Point::convert( KSContext& context, const QPoint& value )
{
    KSValue::Ptr s = context.interpreter()->module( "qt" )->member( context, "QPoint" );
    ASSERT( !s.isNull() );

    return new KSValue( new KSBuiltinStruct( s->structClassValue(), new QPoint( value ) ) );
}

KSQt::Point::Point( KSModule* module, const QString& name )
    : KSBuiltinStructClass( module, name )
{
    addMethod( "isNull", Point_isNull, "" );
    addVariable( "x" );
    addVariable( "y" );
}

bool KSQt::Point::constructor( KSContext& context )
{
    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( KSUtil::checkArgs( context, "", "QPoint", FALSE ) )
        context.setValue( new KSValue( new KSBuiltinStruct( this, new QPoint() ) ) );
    else if ( KSUtil::checkArgs( context, "ii", "QPoint", FALSE ) )
        context.setValue( new KSValue( new KSBuiltinStruct( this, new QPoint( args[0]->intValue(), args[1]->intValue()) ) ) );
    else
        KSUtil::argumentsMismatchError( context, "QPoint" );

    return TRUE;
}

bool KSQt::Point::destructor( void* object )
{
    delete ( (QPoint*)object );

    return TRUE;
}

KSStruct* KSQt::Point::clone( KSBuiltinStruct* s )
{
    return new KSBuiltinStruct( this, new QPoint( *((QPoint*)s->object()) ) );
}

KSValue::Ptr KSQt::Point::property( KSContext&, void* object, const QString& name )
{
    QPoint* value = (QPoint*)object;

    if ( name == "x" )
        return new KSValue( value->x() );
    if ( name == "y" )
        return new KSValue( value->y() );

    return 0;
}

bool KSQt::Point::setProperty( KSContext& context, void* object, const QString& name, const KSValue::Ptr value )
{
    QPoint* v = (QPoint*)object;

    if ( name == "x" )
    {
        if ( !KSUtil::checkType( context, value, KSValue::IntType ) )
            return FALSE;
        v->setX( value->intValue() );
        return TRUE;
    }
    if ( name == "y" )
    {
        if ( !KSUtil::checkType( context, value, KSValue::IntType ) )
            return FALSE;
        v->setY( value->intValue() );
        return TRUE;
    }

    return FALSE;
}


/*************************************************************
 *
 * QSize
 *
 *************************************************************/

static bool Size_boundedTo( void* object, KSContext& context, const QValueList<KSValue::Ptr>& args )
{
    QSize* value = (QSize*)object;
    context.setValue(  KSQt::Size::convert( context, value->boundedTo( KSQt::Size::convert( context, args[0] ) ) ) );
    return TRUE;
}

static bool Size_expandedTo( void* object, KSContext& context, const QValueList<KSValue::Ptr>& args )
{
    QSize* value = (QSize*)object;
    context.setValue(  KSQt::Size::convert( context, value->expandedTo( KSQt::Size::convert( context, args[0] ) ) ) );
    return TRUE;
}

static bool Size_isEmpty( void* object, KSContext& context, const QValueList<KSValue::Ptr>& )
{
    QSize* value = (QSize*)object;
    context.setValue(  new KSValue( value->isEmpty( ) ) );
    return TRUE;
}

static bool Size_isNull( void* object, KSContext& context, const QValueList<KSValue::Ptr>& )
{
    QSize* value = (QSize*)object;
    context.setValue(  new KSValue( value->isNull( ) ) );
    return TRUE;
}

static bool Size_isValid( void* object, KSContext& context, const QValueList<KSValue::Ptr>& )
{
    QSize* value = (QSize*)object;
    context.setValue(  new KSValue( value->isValid( ) ) );
    return TRUE;
}

static bool Size_transpose( void* object, KSContext& context, const QValueList<KSValue::Ptr>& )
{
    QSize* value = (QSize*)object;
    context.setValue( 0 );
    value->transpose( );
    return TRUE;
}

QSize KSQt::Size::convert( KSContext&, const KSValue::Ptr& v )
{
    return *((const QSize*)v->structValue()->object());
}

KSValue::Ptr KSQt::Size::convert( KSContext& context, const QSize& value )
{
    KSValue::Ptr s = context.interpreter()->module( "qt" )->member( context, "QSize" );
    ASSERT( !s.isNull() );

    return new KSValue( new KSBuiltinStruct( s->structClassValue(), new QSize( value ) ) );
}

KSQt::Size::Size( KSModule* module, const QString& name )
    : KSBuiltinStructClass( module, name )
{
    addMethod( "boundedTo", Size_boundedTo, "Sqt:QSize;" );
    addMethod( "expandedTo", Size_expandedTo, "Sqt:QSize;" );
    addMethod( "isEmpty", Size_isEmpty, "" );
    addMethod( "isNull", Size_isNull, "" );
    addMethod( "isValid", Size_isValid, "" );
    addMethod( "transpose", Size_transpose, "" );
    addVariable( "height" );
    addVariable( "width" );
}

bool KSQt::Size::constructor( KSContext& context )
{
    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( KSUtil::checkArgs( context, "", "QSize", FALSE ) )
        context.setValue( new KSValue( new KSBuiltinStruct( this, new QSize() ) ) );
    else if ( KSUtil::checkArgs( context, "ii", "QSize", FALSE ) )
        context.setValue( new KSValue( new KSBuiltinStruct( this, new QSize( args[0]->intValue(), args[1]->intValue()) ) ) );
    else
        KSUtil::argumentsMismatchError( context, "QSize" );

    return TRUE;
}

bool KSQt::Size::destructor( void* object )
{
    delete ( (QSize*)object );

    return TRUE;
}

KSStruct* KSQt::Size::clone( KSBuiltinStruct* s )
{
    return new KSBuiltinStruct( this, new QSize( *((QSize*)s->object()) ) );
}

KSValue::Ptr KSQt::Size::property( KSContext&, void* object, const QString& name )
{
    QSize* value = (QSize*)object;

    if ( name == "height" )
        return new KSValue( value->height() );
    if ( name == "width" )
        return new KSValue( value->width() );

    return 0;
}

bool KSQt::Size::setProperty( KSContext& context, void* object, const QString& name, const KSValue::Ptr value )
{
    QSize* v = (QSize*)object;

    if ( name == "height" )
    {
        if ( !KSUtil::checkType( context, value, KSValue::IntType ) )
            return FALSE;
        v->setHeight( value->intValue() );
        return TRUE;
    }
    if ( name == "width" )
    {
        if ( !KSUtil::checkType( context, value, KSValue::IntType ) )
            return FALSE;
        v->setWidth( value->intValue() );
        return TRUE;
    }

    return FALSE;
}
