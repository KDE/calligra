// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2005 Thorsten Zachmann <zachmann@kde.org>

   The code is based on work of
   Copyright (C) 2002 Toshitaka Fujioka <fujioka@kde.org>

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

#include "KPrPicturePreview.h"

#include <qimage.h>
#include <qpainter.h>
//Added by qt3to4:
#include <QPixmap>
#include <Q3Frame>


KPrPicturePreview::KPrPicturePreview( QWidget* parent)
: Q3Frame( parent )
, mirrorType( PM_NORMAL )
, depth( 0 )
, swapRGB( false )
, bright( 0 )
, grayscal( false )
{
    setFrameStyle( WinPanel | Sunken );
    setMinimumSize( 300, 200 );
}


void KPrPicturePreview::drawContents( QPainter *painter )
{
    QSize ext = contentsRect().size();

    QPixmap _pix = origPixmap;
    QImage img( _pix.convertToImage().smoothScale( ext.width(),ext.height(), Qt::KeepAspectRatio ) );

    bool _horizontal = false;
    bool _vertical = false;
    if ( mirrorType == PM_HORIZONTAL )
        _horizontal = true;
    else if ( mirrorType == PM_VERTICAL )
        _vertical = true;
    else if ( mirrorType == PM_HORIZONTALANDVERTICAL ) {
        _horizontal = true;
        _vertical = true;
    }

    img = img.mirror( _horizontal, _vertical );

    if ( depth != 0 ) {
        QImage tmpImg = img.convertDepth( depth );
        if ( !tmpImg.isNull() )
            img = tmpImg;
    }

    if ( swapRGB )
        img = img.swapRGB();


    if ( grayscal ) {
        if ( depth == 1 || depth == 8 ) {
            for ( int i = 0; i < img.numColors(); ++i ) {
                QRgb rgb = img.color( i );
                int gray = qGray( rgb );
                rgb = qRgb( gray, gray, gray );
                img.setColor( i, rgb );
            }
        }
        else {
            int _width = img.width();
            int _height = img.height();
            int _x = 0;
            int _y = 0;

            for ( _x = 0; _x < _width; ++_x ) {
                for ( _y = 0; _y < _height; ++_y ) {
                    if ( img.valid( _x, _y ) ) {
                        QRgb rgb = img.pixel( _x, _y );
                        int gray = qGray( rgb );
                        rgb = qRgb( gray, gray, gray );
                        img.setPixel( _x, _y, rgb );
                    }
                }
            }
        }
    }


    if ( bright != 0 ) {
        if ( depth == 1 || depth == 8 ) {
            for ( int i = 0; i < img.numColors(); ++i ) {
                QRgb rgb = img.color( i );
                QColor c( rgb );

                if ( bright > 0 )
                    rgb = c.light( 100 + bright ).rgb();
                else
                    rgb = c.dark( 100 + abs( bright ) ).rgb();

                img.setColor( i, rgb );
            }
        }
        else {
            int _width = img.width();
            int _height = img.height();
            int _x = 0;
            int _y = 0;

            for ( _x = 0; _x < _width; ++_x ) {
                for ( _y = 0; _y < _height; ++_y ) {
                    if ( img.valid( _x, _y ) ) {
                        QRgb rgb = img.pixel( _x, _y );
                        QColor c( rgb );

                        if ( bright > 0 )
                            rgb = c.light( 100 + bright ).rgb();
                        else
                            rgb = c.dark( 100 + abs( bright ) ).rgb();

                        img.setPixel( _x, _y, rgb );
                    }
                }
            }
        }
    }

    _pix.convertFromImage( img );

    QPixmap tmpPix( _pix.size() );
    tmpPix.fill( Qt::white );

    QPainter _p;
    _p.begin( &tmpPix );
    _p.drawPixmap( 0, 0, _pix );
    _p.end();

    QSize _pixSize = _pix.size();
    int _x = 0, _y = 0;
    int w = _pixSize.width(), h = _pixSize.height();
    _x = ( ext.width() - w ) / 2;
    _y = ( ext.height() - h ) / 2;

    painter->drawPixmap( _x, _y, tmpPix );
}


void KPrPicturePreview::slotNormalPicture()
{
    if ( mirrorType != PM_NORMAL )
    {
        mirrorType = PM_NORMAL;
        repaint( false );
    }
}


void KPrPicturePreview::slotHorizontalMirrorPicture()
{
    if ( mirrorType != PM_HORIZONTAL )
    {
        mirrorType = PM_HORIZONTAL;
        repaint( false );
    }
}


void KPrPicturePreview::slotVerticalMirrorPicture()
{
    if ( mirrorType != PM_VERTICAL )
    {
        mirrorType = PM_VERTICAL;
        repaint( false );
    }
}


void KPrPicturePreview::slotHorizontalAndVerticalMirrorPicture()
{
    if ( mirrorType != PM_HORIZONTALANDVERTICAL )
    {
        mirrorType = PM_HORIZONTALANDVERTICAL;
        repaint( false );
    }
}


void KPrPicturePreview::slotPictureDepth0()
{
    if ( depth !=0 )
    {
        depth = 0;
        repaint( false );
    }
}


void KPrPicturePreview::slotPictureDepth1()
{
    if ( depth != 1 )
    {
        depth = 1;
        repaint( false );
    }
}


void KPrPicturePreview::slotPictureDepth8()
{
    if ( depth != 8)
    {
        depth = 8;
        repaint( false );
    }
}


void KPrPicturePreview::slotPictureDepth16()
{
    if ( depth != 16 )
    {
        depth = 16;
        repaint( false );
    }
}


void KPrPicturePreview::slotPictureDepth32()
{
    if ( depth !=32 )
    {
        depth = 32;
        repaint( false );
    }
}


void KPrPicturePreview::slotSwapRGBPicture( bool _on )
{
    if ( swapRGB != _on )
    {
        swapRGB = _on;
        repaint( false );
    }
}


void KPrPicturePreview::slotGrayscalPicture( bool _on )
{
    if ( grayscal != _on )
    {
        grayscal = _on;
        repaint( false );
    }
}


void KPrPicturePreview::slotBrightValue( int _value )
{
    if ( bright != _value )
    {
        bright = _value;
        repaint( false );
    }
}


void KPrPicturePreview::setDepth( int _depth)
{
    if ( _depth != depth )
    {
        depth = _depth;
        repaint( false );
    }
}


void KPrPicturePreview::setMirrorType (PictureMirrorType _t)
{
    if ( mirrorType != _t )
    {
        mirrorType = _t;
        repaint( false );
    }
}


void KPrPicturePreview::setPicturePixmap(const QPixmap &_pixmap)
{
    origPixmap = _pixmap;
    repaint( false );
}


#include "KPrPicturePreview.moc"
