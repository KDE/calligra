/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <kprectobject.h>
#include <kpgradient.h>
#include "KPRectObjectIface.h"

#include <kdebug.h>
#include <qregion.h>
#include <qdom.h>
#include <qpainter.h>
#include <kozoomhandler.h>

/******************************************************************/
/* Class: KPRectObject                                            */
/******************************************************************/

/*================ default constructor ===========================*/
KPRectObject::KPRectObject()
    : KP2DObject()
{
    xRnd = 0;
    yRnd = 0;
}

DCOPObject* KPRectObject::dcopObject()
{
    if ( !dcop )
	dcop = new KPRectObjectIface( this );
    return dcop;
}

/*================== overloaded constructor ======================*/
KPRectObject::KPRectObject( const QPen &_pen, const QBrush &_brush, FillType _fillType,
                            const QColor &_gColor1, const QColor &_gColor2, BCType _gType, int _xRnd, int _yRnd,
                            bool _unbalanced, int _xfactor, int _yfactor)
    : KP2DObject( _pen, _brush, _fillType, _gColor1, _gColor2, _gType,
                  _unbalanced, _xfactor, _yfactor )
{
    xRnd = _xRnd;
    yRnd = _yRnd;
}

/*================================================================*/
KPRectObject &KPRectObject::operator=( const KPRectObject & )
{
    return *this;
}

/*========================= save =================================*/
QDomDocumentFragment KPRectObject::save( QDomDocument& doc, double offset )
{
    QDomDocumentFragment fragment=KP2DObject::save(doc, offset);
    if (xRnd!=0 || yRnd!=0) {
        QDomElement elem=doc.createElement("RNDS");
        elem.setAttribute("x", xRnd);
        elem.setAttribute("y", yRnd);
        fragment.appendChild(elem);
    }
    return fragment;
}

/*========================== load ================================*/
double KPRectObject::load(const QDomElement &element)
{
    double offset=KP2DObject::load(element);
    QDomElement e=element.namedItem("RNDS").toElement();
    if(!e.isNull()) {
        int tmp=0;
        if(e.hasAttribute("x"))
            tmp=e.attribute("x").toInt();
        xRnd=tmp;
        tmp=0;
        if(e.hasAttribute("y"))
            tmp=e.attribute("y").toInt();
        yRnd=tmp;
    }
    return offset;
}

/*======================== paint =================================*/
void KPRectObject::paint( QPainter* _painter,KoZoomHandler*_zoomHandler,
			  bool drawingShadow, bool drawContour )
{
    int ow = _zoomHandler->zoomItX( ext.width() );
    int oh = _zoomHandler->zoomItY( ext.height() );

    if ( drawContour ) {
	QPen pen3( Qt::black, 1, Qt::DotLine );
	_painter->setPen( pen3 );
        _painter->setRasterOp( Qt::NotXorROP );

	_painter->drawRoundRect( 0, 0, ow, oh,
				 _zoomHandler->zoomItX( xRnd ),_zoomHandler->zoomItY( yRnd) );
	return;
    }

    QPen pen2(pen);
    pen2.setWidth(_zoomHandler->zoomItX(pen.width()));
    int pw = pen2.width();
    _painter->setPen( pen2 );

    if ( drawingShadow || fillType == FT_BRUSH || !gradient ) {
        _painter->setPen( pen2 );
	_painter->setBrush( brush );

    }
    else {
	QSize size( _zoomHandler->zoomSize( ext ) );
        gradient->setSize( size );
        if ( angle == 0 || angle==360 )
            _painter->drawPixmap( 0, 0, gradient->pixmap(), 0, 0, 
                                  ow - pw + 1, oh - pw + 1);
        else {
            QPixmap pix( ow - pw + 1, oh - pw + 1);
            QPainter p;
            p.begin( &pix );
            p.drawPixmap( 0, 0, gradient->pixmap() );
            p.end();
            _painter->drawPixmap( pw / 2, pw / 2, pix );
        }

	_painter->setBrush( Qt::NoBrush );
    }

    _painter->drawRoundRect( pw / 2, pw / 2, ow - pw + 1, oh - pw + 1,
			     _zoomHandler->zoomItX( xRnd ),_zoomHandler->zoomItY( yRnd) );
}
