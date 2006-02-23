// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2005 Thorsten Zachmann <zachmann@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KPrGroupObject.h"

#include "KPrDocument.h"
#include "KPrLineObject.h"
#include "KPrRectObject.h"
#include "KPrEllipseObject.h"
#include "KPrAutoformObject.h"
#include "KPrTextObject.h"
#include "KPrPixmapObject.h"
#include "KPrPieObject.h"
#include "KPrFreehandObject.h"
#include "KPrPolylineObject.h"
#include "KPrBezierCurveObject.h"
#include "KPrPolygonObject.h"
#include "KPrClosedLineObject.h"
#include <kdebug.h>
#include <KoOasisContext.h>

#include <qpainter.h>
using namespace std;

KPrGroupObject::KPrGroupObject()
    : KPrObject(), objects( QPtrList<KPrObject>() ), updateObjs( false )
{
    objects.setAutoDelete( false );
}

KPrGroupObject::KPrGroupObject( const QPtrList<KPrObject> &objs )
    : KPrObject(), objects( objs ), updateObjs( false )
{
    objects.setAutoDelete( false );
}

KPrGroupObject &KPrGroupObject::operator=( const KPrGroupObject & )
{
    return *this;
}

void KPrGroupObject::selectAllObj()
{
    QPtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setSelected(true);
}

void KPrGroupObject::deSelectAllObj()
{
    QPtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setSelected(false);
}

void KPrGroupObject::setSize( double _width, double _height )
{
    KoSize origSize( ext );
    KPrObject::setSize( _width, _height );

    double fx = ext.width() / origSize.width();
    double fy = ext.height() / origSize.height();

    updateSizes( fx, fy );
}

void KPrGroupObject::setOrig( const KoPoint &_point )
{
    setOrig( _point.x(), _point.y() );
}

void KPrGroupObject::setOrig( double _x, double _y )
{
    double dx = 0;
    double dy = 0;
    if ( !orig.isNull() ) {
        dx = _x - orig.x();
        dy = _y - orig.y();
    }

    KPrObject::setOrig( _x, _y );

    if ( dx != 0 || dy != 0 )
        updateCoords( dx, dy );
}

void KPrGroupObject::moveBy( const KoPoint &_point )
{
    moveBy( _point.x(), _point.y() );
}

void KPrGroupObject::moveBy( double _dx, double _dy )
{
    KPrObject::moveBy( _dx, _dy );
    updateCoords( _dx, _dy );
}

QDomDocumentFragment KPrGroupObject::save( QDomDocument& doc, double offset )
{
    QDomDocumentFragment fragment=KPrObject::save(doc, offset);
    QDomElement objs=doc.createElement("OBJECTS");
    fragment.appendChild(objs);
    QPtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it )
    {
        if ( it.current()->getType() == OT_PART )
            continue;
        QDomElement object=doc.createElement("OBJECT");
        object.setAttribute("type", static_cast<int>( it.current()->getType() ));
        object.appendChild(it.current()->save( doc,offset ));
        objs.appendChild(object);
    }
    return fragment;
}


bool KPrGroupObject::saveOasisObjectAttributes( KPOasisSaveContext &sc ) const
{
    QPtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it )
    {
        //TODO what to do with parts?
        it.current()->saveOasisObject( sc );
    }
    return true;
}

void KPrGroupObject::saveOasisPosObject( KoXmlWriter &xmlWriter, int indexObj ) const
{
    xmlWriter.addAttribute( "draw:id", "object" + QString::number( indexObj ) );
}


const char * KPrGroupObject::getOasisElementName() const
{
    return "draw:g";
}


void KPrGroupObject::loadOasisGroupObject( KPrDocument *_doc, KPrPage * newpage, QDomNode &element, KoOasisContext & context, KPrLoadingInfo */*info*/ )
{
    //KPrObject::loadOasis( element, context, info );
    updateObjs = false;
    _doc->loadOasisObject( newpage,element, context, this);
    QPtrListIterator<KPrObject> it( objects );
    KoRect r=KoRect();
    for ( ; it.current() ; ++it )
    {
        r |= it.current()->getRealRect();
    }
    setOrig( r.x(), r.y() );
    setSize( r.width(), r.height() );
    updateObjs = true;
}

void KPrGroupObject::addObjects( KPrObject * obj )
{
    kdDebug()<<"add object to group object:"<<obj<<endl;
    objects.append( obj );
}

double KPrGroupObject::load( const QDomElement &element, KPrDocument *doc)
{
    //FIXME
    double offset=KPrObject::load(element);
    updateObjs = false;
    QDomElement group=element.namedItem("OBJECTS").toElement();
    if(!group.isNull()) {
        QDomElement current=group.firstChild().toElement();
        while(!current.isNull()) {
            ObjType t = OT_LINE;
            if(current.tagName()=="OBJECT") {
                if(current.hasAttribute("type"))
                    t=static_cast<ObjType>(current.attribute("type").toInt());
                double objOffset;
                switch ( t ) {
                case OT_LINE: {
                    KPrLineObject *kplineobject = new KPrLineObject();
                    objOffset = kplineobject->load(current);
                    kplineobject->setOrig(kplineobject->getOrig().x(),objOffset - offset);
                    objects.append( kplineobject );
                } break;
                case OT_RECT: {
                    KPrRectObject *kprectobject = new KPrRectObject();
                    objOffset = kprectobject->load(current);
                    kprectobject->setOrig(kprectobject->getOrig().x(),objOffset - offset);
                    objects.append( kprectobject );
                } break;
                case OT_ELLIPSE: {
                    KPrEllipseObject *kpellipseobject = new KPrEllipseObject();
                    objOffset = kpellipseobject->load(current);
                    kpellipseobject->setOrig(kpellipseobject->getOrig().x(),objOffset - offset);
                    objects.append( kpellipseobject );
                } break;
                case OT_PIE: {
                    KPrPieObject *kppieobject = new KPrPieObject();
                    objOffset = kppieobject->load(current);
                    kppieobject->setOrig(kppieobject->getOrig().x(),objOffset - offset);
                    objects.append( kppieobject );
                } break;
                case OT_AUTOFORM: {
                    KPrAutoformObject *kpautoformobject = new KPrAutoformObject();
                    objOffset = kpautoformobject->load(current);
                    kpautoformobject->setOrig(kpautoformobject->getOrig().x(),objOffset - offset);
                    objects.append( kpautoformobject );
                } break;
                case OT_TEXT: {
                    KPrTextObject *kptextobject = new KPrTextObject( doc );
                    objOffset = kptextobject->load(current);
                    kptextobject->setOrig(kptextobject->getOrig().x(),objOffset - offset);
                    objects.append( kptextobject );
                } break;
                case OT_CLIPART:
                case OT_PICTURE: {
                    KPrPixmapObject *kppixmapobject = new KPrPixmapObject( doc->pictureCollection() );
                    objOffset = kppixmapobject->load(current);
                    kppixmapobject->setOrig(kppixmapobject->getOrig().x(),objOffset - offset);
                    kppixmapobject->reload();
                    objects.append( kppixmapobject );
                } break;
                case OT_FREEHAND: {
                    KPrFreehandObject *kpfreehandobject = new KPrFreehandObject();
                    objOffset = kpfreehandobject->load( current );
                    kpfreehandobject->setOrig(kpfreehandobject->getOrig().x(),objOffset - offset);
                    objects.append( kpfreehandobject );
                } break;
                case OT_POLYLINE: {
                    KPrPolylineObject *kppolylineobject = new KPrPolylineObject();
                    objOffset = kppolylineobject->load( current );
                    kppolylineobject->setOrig(kppolylineobject->getOrig().x(),objOffset - offset);
                    objects.append( kppolylineobject );
                } break;
                case OT_QUADRICBEZIERCURVE: {
                    KPrQuadricBezierCurveObject *kpQuadricBezierCurveObject = new KPrQuadricBezierCurveObject();
                    objOffset = kpQuadricBezierCurveObject->load( current );
                    kpQuadricBezierCurveObject->setOrig(kpQuadricBezierCurveObject->getOrig().x(),objOffset - offset);
                    objects.append( kpQuadricBezierCurveObject );
                } break;
                case OT_CUBICBEZIERCURVE: {
                    KPrCubicBezierCurveObject *kpCubicBezierCurveObject = new KPrCubicBezierCurveObject();
                    objOffset = kpCubicBezierCurveObject->load( current );
                    kpCubicBezierCurveObject->setOrig(kpCubicBezierCurveObject->getOrig().x(),objOffset - offset);
                    objects.append( kpCubicBezierCurveObject );
                } break;
                case OT_POLYGON: {
                    KPrPolygonObject *kpPolygonObject = new KPrPolygonObject();
                    objOffset = kpPolygonObject->load( current );
                    kpPolygonObject->setOrig(kpPolygonObject->getOrig().x(),objOffset - offset);
                    objects.append( kpPolygonObject );
                } break;
                case OT_GROUP: {
                    KPrGroupObject *kpgroupobject = new KPrGroupObject();
                    objOffset = kpgroupobject->load(current, doc);
                    kpgroupobject->setOrig(kpgroupobject->getOrig().x(),objOffset - offset);
                    objects.append( kpgroupobject );
                } break;
                case OT_CLOSED_LINE: {
                    KPrClosedLineObject *kpClosedLinneObject = new KPrClosedLineObject();
                    objOffset = kpClosedLinneObject->load( current );
                    kpClosedLinneObject->setOrig(kpClosedLinneObject->getOrig().x(),objOffset - offset);
                    objects.append( kpClosedLinneObject );
                } break;
                default: break;
                }
            }
            current=current.nextSibling().toElement();
        }
    }
    updateObjs = true;
    return offset;
}

void KPrGroupObject::draw( QPainter *_painter,KoTextZoomHandler *_zoomhandler,
                          int pageNum, SelectionMode selectionMode, bool drawContour )
{
    QPtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->draw( _painter, _zoomhandler, pageNum, selectionMode, drawContour );

    KPrObject::draw( _painter, _zoomhandler, pageNum, selectionMode, drawContour );
}

void KPrGroupObject::updateSizes( double fx, double fy )
{
    if ( !updateObjs )
        return;
    KoRect r = KoRect();
    QPtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it )
    {
        double _x = ( it.current()->getOrig().x() - orig.x() ) * fx + orig.x();
        double _y = ( it.current()->getOrig().y() - orig.y() ) * fy + orig.y();
        it.current()->setOrig( _x, _y );

        double _w = it.current()->getSize().width() * fx;
        double _h = it.current()->getSize().height() * fy;
        it.current()->setSize( _w, _h );
    }
}

void KPrGroupObject::updateCoords( double dx, double dy )
{
    if ( !updateObjs )
        return;
    QPtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->moveBy( dx, dy );
}

void KPrGroupObject::rotate( float _angle )
{
    float oldAngle = angle;
    float diffAngle = _angle - angle;
    float angInRad = diffAngle * M_PI / 180;

    KPrObject::rotate( _angle );

    // find center of the group
    double centerx = orig.x() + ext.width() / 2.0;
    double centery = orig.y() + ext.height() / 2.0;

    if ( !updateObjs )
        return;
    QPtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it ) {
        // find distance of object center to group center
        double px = it.current()->getOrig().x() + it.current()->getSize().width() / 2.0 - centerx;
        double py = it.current()->getOrig().y() + it.current()->getSize().height() / 2.0 - centery;
        // find distance for move
        double mx = px * cos( angInRad ) - py * sin( angInRad ) - px;
        double my = px * sin( angInRad ) + py * cos( angInRad ) - py;
        double objAngle = it.current()->getAngle();
        // If part of group was already rotated the difference has to be added
        // to the angle
        if ( objAngle != oldAngle )
            it.current()->rotate( objAngle + diffAngle );
        else
            it.current()->rotate( _angle );
        it.current()->moveBy( mx, my );
    }
}

void KPrGroupObject::setShadowParameter( int _distance, ShadowDirection _direction, const QColor &_color )
{
    KPrObject::setShadowParameter( _distance, _direction, _color );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setShadowParameter( _distance, _direction, _color );
}

void KPrGroupObject::setShadowDistance( int _distance )
{
    KPrObject::setShadowDistance( _distance );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setShadowDistance( _distance );
}

void KPrGroupObject::setShadowDirection( ShadowDirection _direction )
{
    KPrObject::setShadowDirection( _direction );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setShadowDirection( _direction );
}

void KPrGroupObject::setShadowColor( const QColor &_color )
{
    KPrObject::setShadowColor( _color );
    kdDebug(33001) << "KPrGroupObject::setShadowColor"<<updateObjs << endl;
    if ( !updateObjs )
        return;
    QPtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setShadowColor( _color );
}

void KPrGroupObject::setEffect( Effect _effect )
{
    KPrObject::setEffect( _effect );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setEffect( _effect );
}

void KPrGroupObject::setEffect2( Effect2 _effect2 )
{
    KPrObject::setEffect2( _effect2 );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setEffect2( _effect2 );
}

void KPrGroupObject::setAppearStep( int _appearStep )
{
    KPrObject::setAppearStep( _appearStep );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setAppearStep( _appearStep );
}

void KPrGroupObject::setDisappear( bool b )
{
    KPrObject::setDisappear( b );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setDisappear( b );
}

void KPrGroupObject::setDisappearStep( int _disappearStep )
{
    KPrObject::setDisappearStep( _disappearStep );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setDisappearStep( _disappearStep );
}

void KPrGroupObject::setEffect3( Effect3 _effect3)
{
    KPrObject::setEffect3( _effect3 );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setEffect3( _effect3 );
}

void KPrGroupObject::setAppearTimer( int _appearTimer )
{
    KPrObject::setAppearTimer( _appearTimer );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setAppearTimer( _appearTimer );
}

void KPrGroupObject::setDisappearTimer( int _disappearTimer )
{
    KPrObject::setDisappearTimer( _disappearTimer );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setDisappearTimer( _disappearTimer );
}

void KPrGroupObject::setOwnClipping( bool _ownClipping )
{
    KPrObject::setOwnClipping( _ownClipping );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setOwnClipping( _ownClipping );
}

void KPrGroupObject::setSubPresStep( int _subPresStep )
{
    KPrObject::setSubPresStep( _subPresStep );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setSubPresStep( _subPresStep );
}

void KPrGroupObject::doSpecificEffects( bool _specEffects, bool _onlyCurrStep )
{
    KPrObject::doSpecificEffects( _specEffects, _onlyCurrStep );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->doSpecificEffects( _specEffects, _onlyCurrStep );
}

void KPrGroupObject::setAppearSoundEffect( bool b )
{
    KPrObject::setAppearSoundEffect( b );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setAppearSoundEffect( b );
}

void KPrGroupObject::setDisappearSoundEffect( bool b )
{
    KPrObject::setDisappearSoundEffect( b );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPrObject> it( objects);
    for ( ; it.current() ; ++it )
        it.current()->setDisappearSoundEffect( b );
}

void KPrGroupObject::setAppearSoundEffectFileName( const QString &_a_fileName )
{
    KPrObject::setAppearSoundEffectFileName( _a_fileName );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPrObject> it( objects);
    for ( ; it.current() ; ++it )
        it.current()->setAppearSoundEffectFileName( _a_fileName );
}

void KPrGroupObject::setDisappearSoundEffectFileName( const QString &_d_fileName )
{
    KPrObject::setDisappearSoundEffectFileName( _d_fileName );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPrObject> it( objects);
    for ( ; it.current() ; ++it )
        it.current()->setDisappearSoundEffectFileName( _d_fileName );
}

void KPrGroupObject::getAllObjectSelectedList(QPtrList<KPrObject> &lst,bool force )
{
    if ( selected || force)
    {
        QPtrListIterator<KPrObject> it( objects);
        for ( ; it.current() ; ++it )
            it.current()->getAllObjectSelectedList( lst, true );
    }
}

void KPrGroupObject::addTextObjects( QPtrList<KoTextObject> &lst ) const
{
    QPtrListIterator<KPrObject> it( objects);
    for ( ; it.current() ; ++it )
        it.current()->addTextObjects( lst );
}

void KPrGroupObject::flip( bool horizontal ) {
    QPtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it ) {
        it.current()->flip( horizontal );
        double mx = 0;
        double my = 0;
        if ( ! horizontal )
        {
            double disttop = it.current()->getOrig().y() - orig.y();
            double distbottom = ext.height() - disttop - it.current()->getSize().height();
            my = distbottom - disttop;
        }
        else
        {
            double distleft = it.current()->getOrig().x() - orig.x();
            double distright = ext.width() - distleft - it.current()->getSize().width();
            mx = distright - distleft;
        }
        it.current()->moveBy( mx, my );
    }
}

void KPrGroupObject::removeFromObjList()
{
    inObjList = false; 
    QPtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it ) 
        it.current()->removeFromObjList();
}

void KPrGroupObject::addToObjList()
{ 
    inObjList = true; 
    QPtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it ) 
        it.current()->addToObjList();
}

void KPrGroupObject::incCmdRef()
{ 
    QPtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it ) 
        it.current()->incCmdRef();

    cmds++; 
}

void KPrGroupObject::decCmdRef()
{ 
    QPtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it ) 
        it.current()->decCmdRef();

    cmds--; 
    doDelete(); 
}
