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
#include <qwmatrix.h>
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
KPEllipseObject::KPEllipseObject( QPen _pen, QBrush _brush, FillType _fillType,
                                  QColor _gColor1, QColor _gColor2, BCType _gType,
                                  bool _unbalanced, int _xfactor, int _yfactor)
    : KP2DObject( _pen, _brush, _fillType, _gColor1, _gColor2, _gType, _unbalanced, _xfactor, _yfactor )
{
    redrawPix = false;

    if ( fillType == FT_GRADIENT )
    {
        gradient = new KPGradient( gColor1, gColor2, gType, QSize( 1, 1 ), unbalanced, xfactor, yfactor );
        redrawPix = true;
        pix.resize( getSize() );
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
void KPEllipseObject::setSize( int _width, int _height )
{
    KPObject::setSize( _width, _height );
    if ( move ) return;

    if ( fillType == FT_GRADIENT && gradient )
    {
        gradient->setSize( getSize() );
        redrawPix = true;
        pix.resize( getSize() );
    }
}

/*================================================================*/
void KPEllipseObject::resizeBy( int _dx, int _dy )
{
    KPObject::resizeBy( _dx, _dy );
    if ( move ) return;

    if ( fillType == FT_GRADIENT && gradient )
    {
        gradient->setSize( getSize() );
        redrawPix = true;
        pix.resize( getSize() );
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
        pix.resize( getSize() );
    }
}

/*======================== paint =================================*/
void KPEllipseObject::paint( QPainter* _painter )
{
    int ow = ext.width();
    int oh = ext.height();
    int pw = pen.width() / 2;

    if ( drawShadow || fillType == FT_BRUSH || !gradient )
    {
        _painter->setPen( pen );
        _painter->setBrush( brush );
        _painter->drawEllipse( pw, pw, ow - 2 * pw, oh - 2 * pw );
    }
    else
    {
        if ( redrawPix )
        {
            redrawPix = false;
            QRegion clipregion( 0, 0, ow - 2 * pw, oh - 2 * pw, QRegion::Ellipse );

            pix.fill( Qt::white );

            QPainter p;
            p.begin( &pix );
            p.setClipRegion( clipregion );
            p.drawPixmap( 0, 0, *gradient->getGradient() );
            p.end();

            pix.setMask( pix.createHeuristicMask() );
        }

        _painter->drawPixmap( pw, pw, pix, 0, 0, ow - 2 * pw, oh - 2 * pw );

        _painter->setPen( pen );
        _painter->setBrush( Qt::NoBrush );
        _painter->drawEllipse( pw, pw, ow - 2 * pw, oh - 2 * pw );

    }
}
