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

#include "kppartobject.h"
#include "kpresenter_doc.h"
#include "kpresenter_view.h"
#include "kpgradient.h"
#include <kodom.h>
#include <koxmlns.h>
#include "kooasiscontext.h"
#include <kparts/partmanager.h>

#include <qpainter.h>
#include <kdebug.h>
using namespace std;

KPPartObject::KPPartObject( KPresenterChild *_child )
    : KP2DObject()
{
    child = _child;
    pen = QPen( Qt::black, 1, Qt::NoPen );
    _enableDrawing = true;
}

KPPartObject &KPPartObject::operator=( const KPPartObject & )
{
    return *this;
}

void KPPartObject::updateChildGeometry()
{
    KoZoomHandler* zh = child->parent()->zoomHandler();
    child->setGeometry( zh->zoomRect( getRect() ), true );
    child->setRotationPoint( QPoint( zh->zoomItX( getOrig().x() + getSize().width() / 2 ),
                                     zh->zoomItY( getOrig().y() + getSize().height() / 2 ) ) );
}

void KPPartObject::rotate( float _angle )
{
    KPObject::rotate( _angle );

    child->setRotation( _angle );
    KoZoomHandler* zh = child->parent()->zoomHandler();
    child->setRotationPoint( QPoint( zh->zoomItX( getOrig().x() + getSize().width() / 2 ),
                                     zh->zoomItY( getOrig().y() + getSize().height() / 2 ) ) );
}

bool KPPartObject::saveOasisObjectAttributes( KPOasisSaveContext &sc ) const
{
    kdDebug() << "KPPartObject::saveOasisPart " << sc.partIndexObj << endl;

    sc.xmlWriter.startElement( "draw:object" );
    const QString name = QString( "Object_%1" ).arg( sc.partIndexObj + 1 );
    ++sc.partIndexObj;
    child->saveOasisAttributes( sc.xmlWriter, name );

    sc.xmlWriter.endElement();
    return true;
}

const char * KPPartObject::getOasisElementName() const
{
    return "draw:frame";
}


void KPPartObject::loadOasis(const QDomElement &element, KoOasisContext&context, KPRLoadingInfo *info)
{
    kdDebug()<<"void KPPartObject::loadOasis(const QDomElement &element)******************\n";
    
    QDomElement objectElement = KoDom::namedItemNS( element, KoXmlNS::draw, "object" );
    child->loadOasis( element, objectElement );
    if(element.hasAttributeNS( KoXmlNS::draw, "name" ))
        objectName = element.attributeNS( KoXmlNS::draw, "name", QString::null);
    (void)child->loadOasisDocument( context.store(), context.manifestDocument() );
}

void KPPartObject::draw( QPainter *_painter, KoZoomHandler *_zoomhandler,
                         int pageNum, SelectionMode selectionMode, bool drawContour )
{
    updateChildGeometry();
    double ow = ext.width();
    double oh = ext.height();

    QSize size( _zoomhandler->zoomSize( ext ) );
    int penw = ( ( pen.style() == Qt::NoPen ) ? 1 : pen.width() ) / 2;

    QPen pen2;
    if ( drawContour )
        pen2 = QPen( Qt::black, 1, Qt::NoPen );
    else {
        pen2 = pen;
        pen2.setWidth( _zoomhandler->zoomItX( pen.width() ) );
    }
    _painter->save();
    child->transform( *_painter );
    _painter->setPen( Qt::NoPen );
    _painter->setBrush( getBrush() );

    if ( angle == 0 ) {
        if ( getFillType() == FT_BRUSH || !gradient )
            _painter->drawRect( penw, penw, _zoomhandler->zoomItX( ext.width() - 2 * penw ),
                                _zoomhandler->zoomItY( ext.height() - 2 * penw ) );
        else {
            gradient->setSize( size );
            _painter->drawPixmap( penw, penw, gradient->pixmap(), 0, 0,
                                  _zoomhandler->zoomItX( ow - 2 * penw ),
                                  _zoomhandler->zoomItY( oh - 2 * penw ) );
        }
    }
    else
    {
        if ( getFillType() == FT_BRUSH || !gradient )
            _painter->drawRect( _zoomhandler->zoomItX( penw ), _zoomhandler->zoomItY( penw ),
                                _zoomhandler->zoomItX( ext.width() - 2 * penw ),
                                _zoomhandler->zoomItY( ext.height() - 2 * penw ) );
        else {
            gradient->setSize( size );
            _painter->drawPixmap( penw, penw, gradient->pixmap(), 0, 0,
                                  _zoomhandler->zoomItX( ow - 2 * penw ),
                                  _zoomhandler->zoomItY( oh - 2 * penw ) );
        }
    }

    _painter->setPen( pen2 );
    _painter->setBrush( Qt::NoBrush );
    _painter->drawRect( _zoomhandler->zoomItX( penw ), _zoomhandler->zoomItY( penw ),
                        _zoomhandler->zoomItX( ow - 2 * penw ), _zoomhandler->zoomItY( oh - 2 * penw ) );
    paint( _painter, _zoomhandler, pageNum, selectionMode, drawContour );
    _painter->restore();

    KPObject::draw( _painter, _zoomhandler, pageNum, selectionMode, drawContour );
}

void KPPartObject::slot_changed( KoChild *_koChild )
{
    KoZoomHandler* zh = child->parent()->zoomHandler();
    KoRect g = zh->unzoomRect( _koChild->geometry() );
    KPObject::setOrig( g.x(), g.y() );
    KPObject::setSize( g.width(), g.height() );
}

void KPPartObject::paint( QPainter *_painter, KoZoomHandler *_zoomHandler,
                          int /* pageNum */, bool /*drawingShadow*/, bool drawContour )
{
    if ( !_enableDrawing ) return;

    if ( drawContour ) {
        QPen pen3( Qt::black, 1, Qt::DotLine );
        _painter->setPen( pen3 );
        _painter->setRasterOp( Qt::NotXorROP );
        _painter->drawRect( _zoomHandler->zoomRect( KoRect( KoPoint( 0.0, 0.0 ), getSize() ) ) );
        return;
    }

    if ( !child || !child->document() )
        return;

    int penw = pen.width();
    KoRect r( KoPoint( penw, penw ), KoPoint( getSize().width() - ( penw * 2.0 ),
              getSize().height() - ( penw * 2.0 ) ) );
    child->document()->paintEverything( *_painter,
                                        _zoomHandler->zoomRect( r ),
                                        true, // flicker?
                                        0 /* View isn't known from here - is that a problem? */,
                                        _zoomHandler->zoomedResolutionX(),
                                        _zoomHandler->zoomedResolutionY() );
}

void KPPartObject::activate( QWidget *_widget )
{
    KPresenterView *view = dynamic_cast<KPresenterView*>( _widget );
    KoDocument* part = child->document();
    if ( !part )
        return;
    view->partManager()->addPart( part, false );
    view->partManager()->setActivePart( part, view );
}

void KPPartObject::deactivate()
{
}

#include "kppartobject.moc"
