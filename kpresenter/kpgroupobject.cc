/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#include <kpgroupobject.h>

#include <kpresenter_doc.h>
#include <kplineobject.h>
#include <kprectobject.h>
#include <kpellipseobject.h>
#include <kpautoformobject.h>
#include <kpclipartobject.h>
#include <kptextobject.h>
#include <kppixmapobject.h>
#include <kppieobject.h>
#include <kdebug.h>

#include <qpainter.h>
using namespace std;

/******************************************************************/
/* Class: KPGroupObject                                           */
/******************************************************************/

/*================================================================*/
KPGroupObject::KPGroupObject()
    : KPObject(), objects(), updateObjs( false )
{
    objects.setAutoDelete( false );
}

/*================================================================*/
KPGroupObject::KPGroupObject( const QList<KPObject> &objs )
    : KPObject(), objects( objs ), updateObjs( false )
{
    objects.setAutoDelete( false );
}

/*================================================================*/
KPGroupObject &KPGroupObject::operator=( const KPGroupObject & )
{
    return *this;
}

/*================================================================*/
void KPGroupObject::setSize( int _width, int _height )
{
    float fx = (float)_width / (float)ext.width();
    float fy = (float)_height / (float)ext.height();
    KPObject::setSize( _width, _height );
    updateSizes( fx, fy );
}

/*================================================================*/
void KPGroupObject::setOrig( QPoint _point )
{
    int dx = orig.x() - _point.x();
    int dy = orig.y() - _point.y();
    KPObject::setOrig( _point );
    updateCoords( dx, dy );
}

/*================================================================*/
void KPGroupObject::setOrig( int _x, int _y )
{
    int dx = orig.x() - _x;
    int dy = orig.y() - _y;
    KPObject::setOrig( _x, _y );
    updateCoords( dx, dy );
}

/*================================================================*/
void KPGroupObject::moveBy( QPoint _point )
{
    KPObject::moveBy( _point );
    updateCoords( _point.x(), _point.y() );
}

/*================================================================*/
void KPGroupObject::moveBy( int _dx, int _dy )
{
    KPObject::moveBy( _dx, _dy );
    updateCoords( _dx, _dy );
}

/*================================================================*/
void KPGroupObject::resizeBy( QSize _size )
{
    float fx = (float)( (float)ext.width() + (float)_size.width() ) / (float)ext.width();
    float fy = (float)( (float)ext.height() + (float)_size.height() ) / (float)ext.height();
    KPObject::resizeBy( _size );
    updateSizes( fx, fy );
}

/*================================================================*/
void KPGroupObject::resizeBy( int _dx, int _dy )
{
    float fx = (float)( (float)ext.width() + (float)_dx ) / (float)ext.width();
    float fy = (float)( (float)ext.height() + (float)_dy ) / (float)ext.height();
    KPObject::resizeBy( _dx, _dy );
    updateSizes( fx, fy );
}

/*================================================================*/
QDomDocumentFragment KPGroupObject::save( QDomDocument& doc )
{
    QDomDocumentFragment fragment=KPObject::save(doc);
    QDomElement objs=doc.createElement("OBJECTS");
    fragment.appendChild(objs);

    KPObject *kpobject = 0;
    for ( unsigned int i = 0; i < objects.count() ; ++i ) {
        kpobject = objects.at( i );
        if ( kpobject->getType() == OT_PART )
            continue;
        QDomElement object=doc.createElement("OBJECT");
        object.setAttribute("type", static_cast<int>( kpobject->getType() ));
        object.appendChild(kpobject->save( doc ));
        objs.appendChild(object);
    }
    return fragment;
}

/*================================================================*/
void KPGroupObject::load(const QDomElement &element, KPresenterDoc *doc)
{
    KPObject::load(element);
    updateObjs = false;
    QDomElement group=element.namedItem("OBJECTS").toElement();
    if(!group.isNull()) {
        QDomElement current=group.firstChild().toElement();
        while(!current.isNull()) {
            ObjType t = OT_LINE;
            if(current.tagName()=="OBJECT") {
                if(current.hasAttribute("type"))
                    t=static_cast<ObjType>(current.attribute("type").toInt());
                switch ( t ) {
                    case OT_LINE: {
                        KPLineObject *kplineobject = new KPLineObject();
                        kplineobject->load(current);
                        objects.append( kplineobject );
                    } break;
                    case OT_RECT: {
                        KPRectObject *kprectobject = new KPRectObject();
                        kprectobject->load(current);
                        objects.append( kprectobject );
                    } break;
                    case OT_ELLIPSE: {
                        KPEllipseObject *kpellipseobject = new KPEllipseObject();
                        kpellipseobject->load(current);
                        objects.append( kpellipseobject );
                    } break;
                    case OT_PIE: {
                        KPPieObject *kppieobject = new KPPieObject();
                        kppieobject->load(current);
                        objects.append( kppieobject );
                    } break;
                    case OT_AUTOFORM: {
                        KPAutoformObject *kpautoformobject = new KPAutoformObject();
                        kpautoformobject->load(current);
                        objects.append( kpautoformobject );
                    } break;
                    case OT_CLIPART: {
                        KPClipartObject *kpclipartobject = new KPClipartObject( doc->getClipartCollection() );
                        kpclipartobject->load(current);
                        objects.append( kpclipartobject );
                    } break;
                    case OT_TEXT: {
                        KPTextObject *kptextobject = new KPTextObject( doc );
                        kptextobject->load(current);
                        objects.append( kptextobject );
                    } break;
                    case OT_PICTURE: {
                        KPPixmapObject *kppixmapobject = new KPPixmapObject( doc->getImageCollection() );
                        kppixmapobject->load(current);
                        objects.append( kppixmapobject );
                    } break;
                    case OT_GROUP: {
                        KPGroupObject *kpgroupobject = new KPGroupObject();
                        kpgroupobject->load(current, doc);
                        objects.append( kpgroupobject );
                    } break;
                    default: break;
                }
            }
            current=current.nextSibling().toElement();
        }
    }
    updateObjs = true;
}

/*================================================================*/
void KPGroupObject::draw( QPainter *_painter, int _diffx, int _diffy )
{
    if ( move ) {
        KPObject::draw( _painter, _diffx, _diffy );
        return;
    }

    KPObject *kpobject = 0;
    for ( unsigned int i = 0; i < objects.count(); i++ ) {
        kpobject = objects.at( i );
        kpobject->draw( _painter, _diffy, _diffy );
    }

    KPObject::draw( _painter, _diffx, _diffy );
}

/*================================================================*/
void KPGroupObject::updateSizes( float fx, float fy )
{
    if ( !updateObjs )
        return;

    KPObject *kpobject = 0;
    for ( unsigned int i = 0; i < objects.count(); i++ ) {
        kpobject = objects.at( i );
        int w = (int)( (float)kpobject->getSize().width() * fx );
        int h = (int)( (float)kpobject->getSize().height() * fy );
        kpobject->setSize( w, h );
    }
}

/*================================================================*/
void KPGroupObject::updateCoords( int dx, int dy )
{
    if ( !updateObjs )
        return;

    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->moveBy( dx, dy );
}

/*================================================================*/
void KPGroupObject::rotate( float _angle )
{
    KPObject::rotate( _angle );

    if ( !updateObjs )
        return;

    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->rotate( _angle );
}

/*================================================================*/
void KPGroupObject::setShadowDistance( int _distance )
{
    KPObject::setShadowDistance( _distance );

    if ( !updateObjs )
        return;

    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->setShadowDistance( _distance );
}

/*================================================================*/
void KPGroupObject::setShadowDirection( ShadowDirection _direction )
{
    KPObject::setShadowDirection( _direction );

    if ( !updateObjs )
        return;

    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->setShadowDirection( _direction );
}

/*================================================================*/
void KPGroupObject::setShadowColor( QColor _color )
{
    KPObject::setShadowColor( _color );

    if ( !updateObjs )
        return;

    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->setShadowColor( _color );
}

/*================================================================*/
void KPGroupObject::setEffect( Effect _effect )
{
    KPObject::setEffect( _effect );

    if ( !updateObjs )
        return;

    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->setEffect( _effect );
}

/*================================================================*/
void KPGroupObject::setEffect2( Effect2 _effect2 )
{
    KPObject::setEffect2( _effect2 );

    if ( !updateObjs )
        return;

    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->setEffect2( _effect2 );
}

/*================================================================*/
void KPGroupObject::setPresNum( int _presNum )
{
    KPObject::setPresNum( _presNum );

    if ( !updateObjs )
        return;

    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->setPresNum( _presNum );
}

/*================================================================*/
void KPGroupObject::setDisappear( bool b )
{
    KPObject::setDisappear( b );

    if ( !updateObjs )
        return;

    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->setDisappear( b );
}

/*================================================================*/
void KPGroupObject::setDisappearNum( int num )
{
    KPObject::setDisappearNum( num );

    if ( !updateObjs )
        return;

    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->setDisappearNum( num );
}

/*================================================================*/
void KPGroupObject::setEffect3( Effect3 _effect3)
{
    KPObject::setEffect3( _effect3 );

    if ( !updateObjs )
        return;

    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->setEffect3( _effect3 );
}

/*================================================================*/
void KPGroupObject::zoom( float _fakt )
{
    KPObject::zoom( _fakt );

    if ( !updateObjs )
        return;

    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->zoom( _fakt );
}

/*================================================================*/
void KPGroupObject::zoomOrig()
{
    KPObject::zoomOrig();

    if ( !updateObjs )
        return;

    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->zoomOrig();
}

/*================================================================*/
void KPGroupObject::setOwnClipping( bool _ownClipping )
{
    KPObject::setOwnClipping( _ownClipping );

    if ( !updateObjs )
        return;

    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->setOwnClipping( _ownClipping );
}

/*================================================================*/
void KPGroupObject::setSubPresStep( int _subPresStep )
{
    KPObject::setSubPresStep( _subPresStep );

    if ( !updateObjs )
        return;

    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->setSubPresStep( _subPresStep );
}

/*================================================================*/
void KPGroupObject::doSpecificEffects( bool _specEffects, bool _onlyCurrStep )
{
    KPObject::doSpecificEffects( _specEffects, _onlyCurrStep );

    if ( !updateObjs )
        return;

    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->doSpecificEffects( _specEffects, _onlyCurrStep );
}
