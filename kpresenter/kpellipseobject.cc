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

#include <kpellipseobject.h>
#include <kpgradient.h>

#include <kdebug.h>
#include <qbitmap.h>
#include <qregion.h>
#include <qdom.h>
#include <qpicture.h>
#include <qpainter.h>
#include <kozoomhandler.h>
using namespace std;

/******************************************************************/
/* Class: KPEllipseObject                                         */
/******************************************************************/

/*================ default constructor ===========================*/
KPEllipseObject::KPEllipseObject()
    : KP2DObject()
{
    redrawPix = false;
}

/*================== overloaded constructor ======================*/
KPEllipseObject::KPEllipseObject( const QPen &_pen, const QBrush &_brush, FillType _fillType,
                                  const QColor &_gColor1, const QColor &_gColor2, BCType _gType,
                                  bool _unbalanced, int _xfactor, int _yfactor)
    : KP2DObject( _pen, _brush, _fillType, _gColor1, _gColor2, _gType, _unbalanced, _xfactor, _yfactor )
{
    redrawPix = false;

    if ( fillType == FT_GRADIENT )
    {
        gradient = new KPGradient( gColor1, gColor2, gType, KoSize( 1, 1 ), unbalanced, xfactor, yfactor );
        redrawPix = true;
        pix.resize( getSize().toQSize() );
    }
    else
        gradient = 0;
}

/*================================================================*/
KPEllipseObject &KPEllipseObject::operator=( const KPEllipseObject & )
{
    return *this;
}

/*================================================================*/
void KPEllipseObject::setSize( double _width, double _height )
{
    KPObject::setSize( _width, _height );

    if ( fillType == FT_GRADIENT && gradient )
    {
        gradient->setSize( getSize() );
        redrawPix = true;
    }
}

/*================================================================*/
void KPEllipseObject::resizeBy( double _dx, double _dy )
{
    KPObject::resizeBy( _dx, _dy );

    if ( fillType == FT_GRADIENT && gradient )
    {
        gradient->setSize( getSize() );
        redrawPix = true;
    }
}

/*================================================================*/
void KPEllipseObject::setFillType( FillType _fillType )
{
    fillType = _fillType;

    if ( fillType == FT_BRUSH && gradient )
    {
        delete gradient;
        gradient = 0;
    }
    if ( fillType == FT_GRADIENT && !gradient )
    {
        gradient = new KPGradient( gColor1, gColor2, gType, getSize(), unbalanced, xfactor, yfactor );
        redrawPix = true;
    }
}

/*======================== paint =================================*/
void KPEllipseObject::paint( QPainter* _painter, KoZoomHandler *_zoomHandler )
{
    double ow = ext.width();
    double oh = ext.height();
    double pw = pen.width() / 2;
    QPen pen2(pen);
    pen2.setWidth(_zoomHandler->zoomItX( pen2.width()));

    if ( drawShadow || fillType == FT_BRUSH || !gradient )
    {
        _painter->setPen( pen2 );
        _painter->setBrush( brush );
        _painter->drawEllipse( _zoomHandler->zoomItX(pw), _zoomHandler->zoomItY(pw), _zoomHandler->zoomItX(ow - 2 * pw), _zoomHandler->zoomItY(oh - 2 * pw) );
    }
    else
    {
        if ( redrawPix )
        {
            redrawPix = false;
            QRegion clipregion( 0, 0, _zoomHandler->zoomItX(ow - 2 * pw), _zoomHandler->zoomItY(oh - 2 * pw), QRegion::Ellipse );
            pix.resize ( _zoomHandler->zoomItX(ow),_zoomHandler->zoomItY(oh) );
            pix.fill( Qt::white );
            QPainter p;
            p.begin( &pix );
            p.setClipRegion( clipregion );
            p.drawPixmap( 0, 0, *gradient->getGradient() );
            p.end();

            pix.setMask( pix.createHeuristicMask() );
        }

        _painter->drawPixmap( _zoomHandler->zoomItX(pw), _zoomHandler->zoomItY(pw), pix, 0, 0, _zoomHandler->zoomItX(ow - 2 * pw), _zoomHandler->zoomItY(oh - 2 * pw) );

        _painter->setPen( pen2 );
        _painter->setBrush( Qt::NoBrush );
        _painter->drawEllipse( _zoomHandler->zoomItX(pw), _zoomHandler->zoomItY(pw), _zoomHandler->zoomItX(ow - 2 * pw), _zoomHandler->zoomItY(oh - 2 * pw) );

    }
}
