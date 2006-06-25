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

#include "KPrPartObject.h"
#include "KPrDocument.h"
#include "KPrView.h"
#include "KPrGradient.h"
#include <KoDom.h>
#include <KoXmlNS.h>
#include "KoOasisContext.h"
#include <kparts/partmanager.h>

#include <qpainter.h>
#include <kdebug.h>
using namespace std;

KPrPartObject::KPrPartObject( KPrChild *_child )
    : KPr2DObject()
{
    child = _child;
    pen = KoPen( Qt::black, 1.0, Qt::NoPen );
    _enableDrawing = true;
}

KPrPartObject &KPrPartObject::operator=( const KPrPartObject & )
{
    return *this;
}

void KPrPartObject::updateChildGeometry()
{
    KoTextZoomHandler* zh = child->parent()->zoomHandler();
    child->setGeometry( zh->zoomRectOld( getRect() ), true );
    child->setRotationPoint( QPoint( zh->zoomItXOld( getOrig().x() + getSize().width() / 2 ),
                                     zh->zoomItYOld( getOrig().y() + getSize().height() / 2 ) ) );
}

void KPrPartObject::rotate( float _angle )
{
    KPrObject::rotate( _angle );

    child->setRotation( _angle );
    KoTextZoomHandler* zh = child->parent()->zoomHandler();
    child->setRotationPoint( QPoint( zh->zoomItXOld( getOrig().x() + getSize().width() / 2 ),
                                     zh->zoomItYOld( getOrig().y() + getSize().height() / 2 ) ) );
}

bool KPrPartObject::saveOasisObjectAttributes( KPOasisSaveContext &sc ) const
{
    kDebug() << "KPrPartObject::saveOasisPart " << sc.partIndexObj << endl;

    sc.xmlWriter.startElement( "draw:object" );
    const QString name = QString( "Object_%1" ).arg( sc.partIndexObj + 1 );
    ++sc.partIndexObj;
    child->saveOasisAttributes( sc.xmlWriter, name );

    sc.xmlWriter.endElement();
    return true;
}

const char * KPrPartObject::getOasisElementName() const
{
    return "draw:frame";
}


void KPrPartObject::loadOasis(const QDomElement &element, KoOasisContext&context, KPrLoadingInfo */*info*/)
{
    kDebug()<<"void KPrPartObject::loadOasis(const QDomElement &element)******************\n";

    QDomElement objectElement = KoDom::namedItemNS( element, KoXmlNS::draw, "object" );
    child->loadOasis( element, objectElement );
    if(element.hasAttributeNS( KoXmlNS::draw, "name" ))
        m_objectName = element.attributeNS( KoXmlNS::draw, "name", QString::null);
    (void)child->loadOasisDocument( context.store(), context.manifestDocument() );
}

void KPrPartObject::draw( QPainter *_painter, KoTextZoomHandler *_zoomhandler,
                         int pageNum, SelectionMode selectionMode, bool drawContour )
{
    updateChildGeometry();
    double ow = ext.width();
    double oh = ext.height();

    QSize size( _zoomhandler->zoomSizeOld( ext ) );
    int penw = ( ( pen.style() == Qt::NoPen ) ? 1 : int( pen.pointWidth() ) ) / 2;

    QPen pen2;
    if ( drawContour )
        pen2 = QPen( Qt::black, 1, Qt::NoPen );
    else {
        pen2 = pen.zoomedPen( _zoomhandler );
    }
    _painter->save();
    child->transform( *_painter );
    _painter->setPen( Qt::NoPen );
    _painter->setBrush( getBrush() );

    if ( angle == 0 ) {
        if ( getFillType() == FT_BRUSH || !gradient )
            _painter->drawRect( penw, penw, _zoomhandler->zoomItXOld( ext.width() - 2 * penw ),
                                _zoomhandler->zoomItYOld( ext.height() - 2 * penw ) );
        else {
            gradient->setSize( size );
            _painter->drawPixmap( penw, penw, gradient->pixmap(), 0, 0,
                                  _zoomhandler->zoomItXOld( ow - 2 * penw ),
                                  _zoomhandler->zoomItYOld( oh - 2 * penw ) );
        }
    }
    else
    {
        if ( getFillType() == FT_BRUSH || !gradient )
            _painter->drawRect( _zoomhandler->zoomItXOld( penw ), _zoomhandler->zoomItYOld( penw ),
                                _zoomhandler->zoomItXOld( ext.width() - 2 * penw ),
                                _zoomhandler->zoomItYOld( ext.height() - 2 * penw ) );
        else {
            gradient->setSize( size );
            _painter->drawPixmap( penw, penw, gradient->pixmap(), 0, 0,
                                  _zoomhandler->zoomItXOld( ow - 2 * penw ),
                                  _zoomhandler->zoomItYOld( oh - 2 * penw ) );
        }
    }

    _painter->setPen( pen2 );
    _painter->setBrush( Qt::NoBrush );
    _painter->drawRect( _zoomhandler->zoomItXOld( penw ), _zoomhandler->zoomItYOld( penw ),
                        _zoomhandler->zoomItXOld( ow - 2 * penw ), _zoomhandler->zoomItYOld( oh - 2 * penw ) );
    paint( _painter, _zoomhandler, pageNum, selectionMode, drawContour );
    _painter->restore();

    KPrObject::draw( _painter, _zoomhandler, pageNum, selectionMode, drawContour );
}

void KPrPartObject::slot_changed( KoChild *_koChild )
{
    KoTextZoomHandler* zh = child->parent()->zoomHandler();
    KoRect g = zh->unzoomRectOld( _koChild->geometry() );
    KPrObject::setOrig( g.x(), g.y() );
    KPrObject::setSize( g.width(), g.height() );
}

void KPrPartObject::paint( QPainter *_painter, KoTextZoomHandler *_zoomHandler,
                          int /* pageNum */, bool /*drawingShadow*/, bool drawContour )
{
    if ( !_enableDrawing ) return;

    if ( drawContour ) {
        QPen pen3( Qt::black, 1, Qt::DotLine );
        _painter->setPen( pen3 );
#warning "kde4: port it"		
        //_painter->setRasterOp( Qt::NotXorROP );
        _painter->drawRect( _zoomHandler->zoomRectOld( KoRect( KoPoint( 0.0, 0.0 ), getSize() ) ) );
        return;
    }

    if ( !child || !child->document() )
        return;

    int penw = ( pen.style() == Qt::NoPen ) ? 0 : int( pen.pointWidth() );
    KoRect r( KoPoint( penw, penw ), KoPoint( getSize().width() - ( penw * 2.0 ),
              getSize().height() - ( penw * 2.0 ) ) );
    double zoomX, zoomY;
    _zoomHandler->zoom(&zoomX, &zoomY);
    child->document()->paintEverything( *_painter,
                                        _zoomHandler->zoomRectOld( r ),
                                        true, // flicker?
                                        0 /* View isn't known from here - is that a problem? */,
                                        zoomX,
                                        zoomY );
}

void KPrPartObject::activate( QWidget *_widget )
{
    KPrView *view = dynamic_cast<KPrView*>( _widget );
    KoDocument* part = child->document();
    if ( !part )
        return;
    view->partManager()->addPart( part, false );
    view->partManager()->setActivePart( part, view );
}

void KPrPartObject::deactivate()
{
}

#include "KPrPartObject.moc"
