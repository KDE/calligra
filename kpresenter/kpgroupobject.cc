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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kpgroupobject.h"

#include "kpresenter_doc.h"
#include "kplineobject.h"
#include "kprectobject.h"
#include "kpellipseobject.h"
#include "kpautoformobject.h"
#include "kptextobject.h"
#include "kppixmapobject.h"
#include "kppieobject.h"
#include "kpfreehandobject.h"
#include "kppolylineobject.h"
#include "kpquadricbeziercurveobject.h"
#include "kpcubicbeziercurveobject.h"
#include "kppolygonobject.h"
#include "kpclosedlineobject.h"
#include <kdebug.h>
#include <kooasiscontext.h>

#include <qpainter.h>
using namespace std;

KPGroupObject::KPGroupObject()
    : KPObject(), objects( QPtrList<KPObject>() ), updateObjs( false )
{
    objects.setAutoDelete( false );
}

KPGroupObject::KPGroupObject( const QPtrList<KPObject> &objs )
    : KPObject(), objects( objs ), updateObjs( false )
{
    objects.setAutoDelete( false );
}

KPGroupObject &KPGroupObject::operator=( const KPGroupObject & )
{
    return *this;
}

void KPGroupObject::selectAllObj()
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setSelected(true);
}

void KPGroupObject::deSelectAllObj()
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setSelected(false);
}

void KPGroupObject::setSize( double _width, double _height )
{
    KoSize origSize( ext );
    KPObject::setSize( _width, _height );

    double fx = ext.width() / origSize.width();
    double fy = ext.height() / origSize.height();

    updateSizes( fx, fy );
}

void KPGroupObject::setOrig( const KoPoint &_point )
{
    setOrig( _point.x(), _point.y() );
}

void KPGroupObject::setOrig( double _x, double _y )
{
    double dx = 0;
    double dy = 0;
    if ( !orig.isNull() ) {
        dx = _x - orig.x();
        dy = _y - orig.y();
    }

    KPObject::setOrig( _x, _y );

    if ( dx != 0 || dy != 0 )
        updateCoords( dx, dy );
}

void KPGroupObject::moveBy( const KoPoint &_point )
{
    moveBy( _point.x(), _point.y() );
}

void KPGroupObject::moveBy( double _dx, double _dy )
{
    KPObject::moveBy( _dx, _dy );
    updateCoords( _dx, _dy );
}

QDomDocumentFragment KPGroupObject::save( QDomDocument& doc, double offset )
{
    QDomDocumentFragment fragment=KPObject::save(doc, offset);
    QDomElement objs=doc.createElement("OBJECTS");
    fragment.appendChild(objs);
    QPtrListIterator<KPObject> it( objects );
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


bool KPGroupObject::saveOasisObjectAttributes( KPOasisSaveContext &sc ) const
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
    {
        //TODO what to do with parts?
        it.current()->saveOasisObject( sc );
    }
    return true;
}

const char * KPGroupObject::getOasisElementName() const
{
    return "draw:g";
}


void KPGroupObject::loadOasisGroupObject( KPresenterDoc *_doc, KPrPage * newpage, QDomNode &element, KoOasisContext & context, KPRLoadingInfo *info)
{
    //KPObject::loadOasis( element, context, info );
    updateObjs = false;
    _doc->loadOasisObject( newpage,element, context, this);
    QPtrListIterator<KPObject> it( objects );
    KoRect r=KoRect();
    for ( ; it.current() ; ++it )
    {
        r |= it.current()->getRealRect();
    }
    setOrig( r.x(), r.y() );
    setSize( r.width(), r.height() );
    updateObjs = true;
}

void KPGroupObject::addObjects( KPObject * obj )
{
    kdDebug()<<"add object to group object:"<<obj<<endl;
    objects.append( obj );
}

double KPGroupObject::load( const QDomElement &element, KPresenterDoc *doc)
{
    //FIXME
    double offset=KPObject::load(element);
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
                    KPLineObject *kplineobject = new KPLineObject();
                    objOffset = kplineobject->load(current);
                    kplineobject->setOrig(kplineobject->getOrig().x(),objOffset - offset);
                    objects.append( kplineobject );
                } break;
                case OT_RECT: {
                    KPRectObject *kprectobject = new KPRectObject();
                    objOffset = kprectobject->load(current);
                    kprectobject->setOrig(kprectobject->getOrig().x(),objOffset - offset);
                    objects.append( kprectobject );
                } break;
                case OT_ELLIPSE: {
                    KPEllipseObject *kpellipseobject = new KPEllipseObject();
                    objOffset = kpellipseobject->load(current);
                    kpellipseobject->setOrig(kpellipseobject->getOrig().x(),objOffset - offset);
                    objects.append( kpellipseobject );
                } break;
                case OT_PIE: {
                    KPPieObject *kppieobject = new KPPieObject();
                    objOffset = kppieobject->load(current);
                    kppieobject->setOrig(kppieobject->getOrig().x(),objOffset - offset);
                    objects.append( kppieobject );
                } break;
                case OT_AUTOFORM: {
                    KPAutoformObject *kpautoformobject = new KPAutoformObject();
                    objOffset = kpautoformobject->load(current);
                    kpautoformobject->setOrig(kpautoformobject->getOrig().x(),objOffset - offset);
                    objects.append( kpautoformobject );
                } break;
                case OT_TEXT: {
                    KPTextObject *kptextobject = new KPTextObject( doc );
                    objOffset = kptextobject->load(current);
                    kptextobject->setOrig(kptextobject->getOrig().x(),objOffset - offset);
                    objects.append( kptextobject );
                } break;
                case OT_CLIPART:
                case OT_PICTURE: {
                    KPPixmapObject *kppixmapobject = new KPPixmapObject( doc->pictureCollection() );
                    objOffset = kppixmapobject->load(current);
                    kppixmapobject->setOrig(kppixmapobject->getOrig().x(),objOffset - offset);
                    kppixmapobject->reload();
                    objects.append( kppixmapobject );
                } break;
                case OT_FREEHAND: {
                    KPFreehandObject *kpfreehandobject = new KPFreehandObject();
                    objOffset = kpfreehandobject->load( current );
                    kpfreehandobject->setOrig(kpfreehandobject->getOrig().x(),objOffset - offset);
                    objects.append( kpfreehandobject );
                } break;
                case OT_POLYLINE: {
                    KPPolylineObject *kppolylineobject = new KPPolylineObject();
                    objOffset = kppolylineobject->load( current );
                    kppolylineobject->setOrig(kppolylineobject->getOrig().x(),objOffset - offset);
                    objects.append( kppolylineobject );
                } break;
                case OT_QUADRICBEZIERCURVE: {
                    KPQuadricBezierCurveObject *kpQuadricBezierCurveObject = new KPQuadricBezierCurveObject();
                    objOffset = kpQuadricBezierCurveObject->load( current );
                    kpQuadricBezierCurveObject->setOrig(kpQuadricBezierCurveObject->getOrig().x(),objOffset - offset);
                    objects.append( kpQuadricBezierCurveObject );
                } break;
                case OT_CUBICBEZIERCURVE: {
                    KPCubicBezierCurveObject *kpCubicBezierCurveObject = new KPCubicBezierCurveObject();
                    objOffset = kpCubicBezierCurveObject->load( current );
                    kpCubicBezierCurveObject->setOrig(kpCubicBezierCurveObject->getOrig().x(),objOffset - offset);
                    objects.append( kpCubicBezierCurveObject );
                } break;
                case OT_POLYGON: {
                    KPPolygonObject *kpPolygonObject = new KPPolygonObject();
                    objOffset = kpPolygonObject->load( current );
                    kpPolygonObject->setOrig(kpPolygonObject->getOrig().x(),objOffset - offset);
                    objects.append( kpPolygonObject );
                } break;
                case OT_GROUP: {
                    KPGroupObject *kpgroupobject = new KPGroupObject();
                    objOffset = kpgroupobject->load(current, doc);
                    kpgroupobject->setOrig(kpgroupobject->getOrig().x(),objOffset - offset);
                    objects.append( kpgroupobject );
                } break;
                case OT_CLOSED_LINE: {
                    KPClosedLineObject *kpClosedLinneObject = new KPClosedLineObject();
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

void KPGroupObject::draw( QPainter *_painter,KoZoomHandler *_zoomhandler,
                          int pageNum, SelectionMode selectionMode, bool drawContour )
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->draw( _painter, _zoomhandler, pageNum, selectionMode, drawContour );

    KPObject::draw( _painter, _zoomhandler, pageNum, selectionMode, drawContour );
}

void KPGroupObject::updateSizes( double fx, double fy )
{
    if ( !updateObjs )
        return;
    KoRect r = KoRect();
    QPtrListIterator<KPObject> it( objects );
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

void KPGroupObject::updateCoords( double dx, double dy )
{
    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->moveBy( dx, dy );
}

void KPGroupObject::rotate( float _angle )
{
    float oldAngle = angle;
    float diffAngle = _angle - angle;
    float angInRad = diffAngle * M_PI / 180;

    KPObject::rotate( _angle );

    // find center of the group
    double centerx = orig.x() + ext.width() / 2.0;
    double centery = orig.y() + ext.height() / 2.0;

    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects );
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

void KPGroupObject::setShadowParameter( int _distance, ShadowDirection _direction, const QColor &_color )
{
    KPObject::setShadowParameter( _distance, _direction, _color );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setShadowParameter( _distance, _direction, _color );
}

void KPGroupObject::setShadowDistance( int _distance )
{
    KPObject::setShadowDistance( _distance );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setShadowDistance( _distance );
}

void KPGroupObject::setShadowDirection( ShadowDirection _direction )
{
    KPObject::setShadowDirection( _direction );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setShadowDirection( _direction );
}

void KPGroupObject::setShadowColor( const QColor &_color )
{
    KPObject::setShadowColor( _color );
    kdDebug(33001) << "KPGroupObject::setShadowColor"<<updateObjs << endl;
    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setShadowColor( _color );
}

void KPGroupObject::setEffect( Effect _effect )
{
    KPObject::setEffect( _effect );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setEffect( _effect );
}

void KPGroupObject::setEffect2( Effect2 _effect2 )
{
    KPObject::setEffect2( _effect2 );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setEffect2( _effect2 );
}

void KPGroupObject::setAppearStep( int _appearStep )
{
    KPObject::setAppearStep( _appearStep );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setAppearStep( _appearStep );
}

void KPGroupObject::setDisappear( bool b )
{
    KPObject::setDisappear( b );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setDisappear( b );
}

void KPGroupObject::setDisappearStep( int _disappearStep )
{
    KPObject::setDisappearStep( _disappearStep );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setDisappearStep( _disappearStep );
}

void KPGroupObject::setEffect3( Effect3 _effect3)
{
    KPObject::setEffect3( _effect3 );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setEffect3( _effect3 );
}

void KPGroupObject::setAppearTimer( int _appearTimer )
{
    KPObject::setAppearTimer( _appearTimer );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setAppearTimer( _appearTimer );
}

void KPGroupObject::setDisappearTimer( int _disappearTimer )
{
    KPObject::setDisappearTimer( _disappearTimer );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setDisappearTimer( _disappearTimer );
}

void KPGroupObject::setOwnClipping( bool _ownClipping )
{
    KPObject::setOwnClipping( _ownClipping );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setOwnClipping( _ownClipping );
}

void KPGroupObject::setSubPresStep( int _subPresStep )
{
    KPObject::setSubPresStep( _subPresStep );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setSubPresStep( _subPresStep );
}

void KPGroupObject::doSpecificEffects( bool _specEffects, bool _onlyCurrStep )
{
    KPObject::doSpecificEffects( _specEffects, _onlyCurrStep );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->doSpecificEffects( _specEffects, _onlyCurrStep );
}

void KPGroupObject::setAppearSoundEffect( bool b )
{
    KPObject::setAppearSoundEffect( b );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setAppearSoundEffect( b );
}

void KPGroupObject::setDisappearSoundEffect( bool b )
{
    KPObject::setDisappearSoundEffect( b );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects);
    for ( ; it.current() ; ++it )
        it.current()->setDisappearSoundEffect( b );
}

void KPGroupObject::setAppearSoundEffectFileName( const QString &_a_fileName )
{
    KPObject::setAppearSoundEffectFileName( _a_fileName );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects);
    for ( ; it.current() ; ++it )
        it.current()->setAppearSoundEffectFileName( _a_fileName );
}

void KPGroupObject::setDisappearSoundEffectFileName( const QString &_d_fileName )
{
    KPObject::setDisappearSoundEffectFileName( _d_fileName );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects);
    for ( ; it.current() ; ++it )
        it.current()->setDisappearSoundEffectFileName( _d_fileName );
}

void KPGroupObject::getAllObjectSelectedList(QPtrList<KPObject> &lst,bool force )
{
    if ( selected || force)
    {
        QPtrListIterator<KPObject> it( objects);
        for ( ; it.current() ; ++it )
            it.current()->getAllObjectSelectedList( lst, true );
    }
}

void KPGroupObject::addTextObjects( QPtrList<KoTextObject> &lst ) const
{
    QPtrListIterator<KPObject> it( objects);
    for ( ; it.current() ; ++it )
        it.current()->addTextObjects( lst );
}

void KPGroupObject::flip( bool horizontal ) {
    QPtrListIterator<KPObject> it( objects );
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

void KPGroupObject::removeFromObjList()
{
    inObjList = false; 
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it ) 
        it.current()->removeFromObjList();
}

void KPGroupObject::addToObjList()
{ 
    inObjList = true; 
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it ) 
        it.current()->addToObjList();
}

void KPGroupObject::incCmdRef()
{ 
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it ) 
        it.current()->incCmdRef();

    cmds++; 
}

void KPGroupObject::decCmdRef()
{ 
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it ) 
        it.current()->decCmdRef();

    cmds--; 
    doDelete(); 
}
