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

#include <kppixmapobject.h>
#include <kpgradient.h>

#include "KPPixmapObjectIface.h"

#include <qbuffer.h>
#include <qpainter.h>
#include <qwmatrix.h>
#include <qfileinfo.h>
#include <qpixmap.h>
#include <qdom.h>
#include <qimage.h>

#include <kdebug.h>
#include <koSize.h>
#include <kozoomhandler.h>
// for getenv ()
#include <stdlib.h>

/******************************************************************/
/* Class: KPPixmapObject                                          */
/******************************************************************/

/*================ default constructor ===========================*/
KPPixmapObject::KPPixmapObject( KoPictureCollection *_imageCollection )
    : KP2DObject()
{
    imageCollection = _imageCollection;
    brush = Qt::NoBrush;
    pen = QPen( Qt::black, 1, Qt::NoPen );
    mirrorType = PM_NORMAL;
    depth = 0;
    swapRGB = false;
    grayscal = false;
    bright = 0;
}

/*================== overloaded constructor ======================*/
KPPixmapObject::KPPixmapObject( KoPictureCollection *_imageCollection, const KoPictureKey & key )
    : KP2DObject()
{
    imageCollection = _imageCollection;

    ext = KoSize(); // invalid size means unset
    brush = Qt::NoBrush;
    pen = QPen( Qt::black, 1, Qt::NoPen );
    mirrorType = PM_NORMAL;
    depth = 0;
    swapRGB = false;
    grayscal = false;
    bright = 0;

    setPixmap( key );
}

DCOPObject* KPPixmapObject::dcopObject()
{
    if ( !dcop )
	dcop = new KPPixmapObjectIface( this );
    return dcop;
}


/*================================================================*/
KPPixmapObject &KPPixmapObject::operator=( const KPPixmapObject & )
{
    return *this;
}

/*================================================================*/
void KPPixmapObject::setPixmap( const KoPictureKey & key )
{
    image = imageCollection->findPicture( key );
}

/*========================= save =================================*/
QDomDocumentFragment KPPixmapObject::save( QDomDocument& doc, double offset )
{
    QDomDocumentFragment fragment=KP2DObject::save(doc, offset);
    QDomElement elem=doc.createElement("KEY");
    image.getKey().saveAttributes(elem);
    fragment.appendChild(elem);

    QDomElement elemSettings = doc.createElement( "PICTURESETTINGS" );

    elemSettings.setAttribute( "mirrorType", static_cast<int>( mirrorType ) );
    elemSettings.setAttribute( "depth", depth );
    elemSettings.setAttribute( "swapRGB", static_cast<int>( swapRGB ) );
    elemSettings.setAttribute( "grayscal", static_cast<int>( grayscal ) );
    elemSettings.setAttribute( "bright", bright );

    fragment.appendChild( elemSettings );

    return fragment;
}

/*========================== load ================================*/
double KPPixmapObject::load(const QDomElement &element)
{
    double offset=KP2DObject::load(element);
    QDomElement e=element.namedItem("KEY").toElement();
    if(!e.isNull()) {
        KoPictureKey key;
        key.loadAttributes( e );
        image.clear();
        image.setKey(key);
    }
    else {
        // try to find a PIXMAP tag if the KEY is not available...
        e=element.namedItem("PIXMAP").toElement();
        if(!e.isNull()) {
            bool openPic = true;
            QString _data;
            QString _fileName;
            if(e.hasAttribute("data"))
                _data=e.attribute("data");
            if ( _data.isEmpty() )
                openPic = true;
            else
                openPic = false;
            if(e.hasAttribute("filename"))
                _fileName=e.attribute("filename");
            if ( !_fileName.isEmpty() )
            {
                if ( int _envVarB = _fileName.find( '$' ) >= 0 )
                {
                    int _envVarE = _fileName.find( '/', _envVarB );
                    QString path = getenv( QFile::encodeName(_fileName.mid( _envVarB, _envVarE-_envVarB )) );
                    _fileName.replace( _envVarB-1, _envVarE-_envVarB+1, path );
                }
            }

            if ( openPic )
                // !! this loads it from the disk (unless it's in the image collection already)
                image = imageCollection->loadPicture( _fileName );
            else
            {
                KoPictureKey key( _fileName );
                image.clear();
                image.setKey(key);
                QByteArray rawData=_data.utf8(); // XPM is normally ASCII, therefore UTF-8
                rawData[rawData.size()-1]=char(10); // Replace the NULL character by a LINE FEED
                QBuffer buffer(rawData);
                image.loadXpm(&buffer);
            }
        }
    }

    e = element.namedItem( "PICTURESETTINGS" ).toElement();
    if ( !e.isNull() ) {
        PictureMirrorType _mirrorType = PM_NORMAL;
        int _depth = 0;
        bool _swapRGB = false;
        bool _grayscal = false;
        int _bright = 0;

        if ( e.hasAttribute( "mirrorType" ) )
            _mirrorType = static_cast<PictureMirrorType>( e.attribute( "mirrorType" ).toInt() );
        if ( e.hasAttribute( "depth" ) )
            _depth = e.attribute( "depth" ).toInt();
        if ( e.hasAttribute( "swapRGB" ) )
            _swapRGB = static_cast<bool>( e.attribute( "swapRGB" ).toInt() );
        if ( e.hasAttribute( "grayscal" ) )
            _grayscal = static_cast<bool>( e.attribute( "grayscal" ).toInt() );
        if ( e.hasAttribute( "bright" ) )
            _bright = e.attribute( "bright" ).toInt();

        mirrorType = _mirrorType;
        depth = _depth;
        swapRGB = _swapRGB;
        grayscal = _grayscal;
        bright = _bright;
    }
    else {
        mirrorType = PM_NORMAL;
        depth = 0;
        swapRGB = false;
        grayscal = false;
        bright = 0;
    }

    return offset;
}

/*========================= draw =================================*/
void KPPixmapObject::draw( QPainter *_painter, KoZoomHandler*_zoomHandler,
			   SelectionMode selectionMode, bool drawContour )
{
    if ( image.isNull() ) return;

    double ox = orig.x();
    double oy = orig.y();
    double ow = ext.width();
    double oh = ext.height();

     _painter->save();

    QPen pen2;
    if ( drawContour ) {
        pen2 = QPen( Qt::black, 1, Qt::DotLine );
        _painter->setRasterOp( Qt::NotXorROP );
    }
    else {
        pen2 = pen;
        pen2.setWidth( _zoomHandler->zoomItX( pen.width() ) );
    }

    _painter->setPen( pen2 );
    if ( !drawContour )
	_painter->setBrush( brush );

    double penw = _zoomHandler->zoomItX( pen.width() ) / 2.0;

    if ( shadowDistance > 0 && !drawContour )
    {
      double sx = ox;
      double sy = oy;

      getShadowCoords( sx, sy );

      if ( angle == 0 )
        {
            _painter->setPen( QPen( shadowColor ) );
            _painter->setBrush( shadowColor );
            _painter->drawRect( _zoomHandler->zoomItX( sx ), _zoomHandler->zoomItY( sy ),
                                _zoomHandler->zoomItX( ext.width() ), _zoomHandler->zoomItY( ext.height() ) );
        }
        else
        {
            _painter->translate( _zoomHandler->zoomItX( ox ), _zoomHandler->zoomItY( oy ) );

            QSize bs = QSize( _zoomHandler->zoomItX( ow ), _zoomHandler->zoomItY( oh ) );
            QRect br = QRect( 0, 0, bs.width(), bs.height() );
            int pw = br.width();
            int ph = br.height();
            QRect rr = br;
            int pixYPos = -rr.y();
            int pixXPos = -rr.x();
            br.moveTopLeft( QPoint( -br.width() / 2, -br.height() / 2 ) );
            rr.moveTopLeft( QPoint( -rr.width() / 2, -rr.height() / 2 ) );

	    QWMatrix m;
            m.translate( pw / 2, ph / 2 );
            m.rotate( angle );
            m.translate( rr.left() + pixXPos + _zoomHandler->zoomItX( sx ),
                         rr.top() + pixYPos + _zoomHandler->zoomItY( sy ) );

            _painter->setWorldMatrix( m, true );

            _painter->setPen( QPen( shadowColor ) );
            _painter->setBrush( shadowColor );

            _painter->drawRect( 0, 0, bs.width(), bs.height() );
        }
    }
    _painter->restore();
    _painter->save();
    QSize size( _zoomHandler->zoomSize( ext ) );

    if ( angle == 0 ) {
        // Draw background
        _painter->setPen( Qt::NoPen );
	if ( !drawContour )
	    _painter->setBrush( brush );
        if ( fillType == FT_BRUSH || !gradient || drawContour )
            _painter->drawRect( (int)( _zoomHandler->zoomItX( ox ) + penw ),
                                (int)( _zoomHandler->zoomItY( oy ) + penw ),
                                (int)( _zoomHandler->zoomItX( ext.width() ) - 2 * penw ),
                                (int)( _zoomHandler->zoomItY( ext.height() ) - 2 * penw ) );
        else {
            gradient->setSize( size );
            _painter->drawPixmap( (int)( _zoomHandler->zoomItX( ox ) + penw ),
                                  (int)( _zoomHandler->zoomItY( oy ) + penw ),
                                  gradient->pixmap(), 0, 0,
                                  (int)( _zoomHandler->zoomItX( ow ) - 2 * penw ),
                                  (int)( _zoomHandler->zoomItY( oh ) - 2 * penw ) );
        }

	// Draw pixmap
        if ( !drawContour ) {
            QRect rect( (int)( _zoomHandler->zoomItX( ox ) + penw ),
                        (int)( _zoomHandler->zoomItY( oy ) + penw ),
                        (int)( _zoomHandler->zoomItX( ow ) - 2 * penw ),
                        (int)( _zoomHandler->zoomItY( oh ) - 2 * penw ) );
            QPixmap _pixmap = image.generatePixmap( rect.size() );

	    QPixmap tmpPix = changePictureSettings( _pixmap ); // hmm, what about caching that pixmap?

            _painter->drawPixmap( rect, tmpPix );
	}

        // Draw border - TODO port to KoBorder::drawBorders() (after writing a simplified version of it, that takes the same border on each size)
        _painter->setPen( pen2 );
        _painter->setBrush( Qt::NoBrush );
        _painter->drawRect( (int)( _zoomHandler->zoomItX( ox ) + penw ), (int)( _zoomHandler->zoomItY( oy ) + penw ),
                            (int)( _zoomHandler->zoomItX( ow ) - 2 * penw ), (int)( _zoomHandler->zoomItY( oh ) - 2 * penw ) );
    } else {
        _painter->translate( _zoomHandler->zoomItX( ox ), _zoomHandler->zoomItY( oy ) );

        QSize bs = QSize( _zoomHandler->zoomItX( ow ), _zoomHandler->zoomItY( oh ) );
        QRect br = QRect( 0, 0, bs.width(), bs.height() );
        int pw = br.width();
        int ph = br.height();
        QRect rr = br;
        int pixYPos = -rr.y();
        int pixXPos = -rr.x();
        br.moveTopLeft( QPoint( -br.width() / 2, -br.height() / 2 ) );
        rr.moveTopLeft( QPoint( -rr.width() / 2, -rr.height() / 2 ) );

        QWMatrix m;
        m.translate( pw / 2, ph / 2 );
        m.rotate( angle );
        m.translate( rr.left() + pixXPos, rr.top() + pixYPos );

        _painter->setWorldMatrix( m, true );

        _painter->setPen( Qt::NoPen );
	if ( !drawContour )
	    _painter->setBrush( brush );

        if ( fillType == FT_BRUSH || !gradient  || drawContour )
            _painter->drawRect( (int)penw, (int)penw,
                                (int)( _zoomHandler->zoomItX( ext.width() ) - 2 * penw ),
                                (int)( _zoomHandler->zoomItY( ext.height() ) - 2 * penw ) );
        else {
            gradient->setSize( size );
            _painter->drawPixmap( (int)penw, (int)penw, gradient->pixmap(), 0, 0,
                                  (int)( _zoomHandler->zoomItX( ow ) - 2 * penw ),
                                  (int)( _zoomHandler->zoomItY( oh ) - 2 * penw ) );
        }

        // Draw pixmap
        if ( !drawContour ) {
            QRect rect( (int)penw,
                        (int)penw,
                        (int)( _zoomHandler->zoomItX( ow ) - 2 * penw ),
                        (int)( _zoomHandler->zoomItY( oh ) - 2 * penw ) );
            QPixmap _pixmap = image.generatePixmap( rect.size() );

	    QPixmap tmpPix = changePictureSettings( _pixmap );

            _painter->drawPixmap( rect, tmpPix );
	}

        _painter->setPen( pen2 );
        _painter->setBrush( Qt::NoBrush );
        _painter->drawRect( (int)penw, (int)penw,
                            (int)( _zoomHandler->zoomItX( ow ) - 2 * penw ),
                            (int)( _zoomHandler->zoomItY( oh ) - 2 * penw ) );
    }

    _painter->restore();

    KPObject::draw( _painter, _zoomHandler, selectionMode, drawContour );
}

QPixmap KPPixmapObject::getOrignalPixmap()
{
    QSize _pixSize = image.getOriginalSize();
    QPixmap _pixmap = image.generatePixmap( _pixSize );

    return _pixmap;
}

QPixmap KPPixmapObject::changePictureSettings( QPixmap _tmpPixmap )
{

  if (mirrorType == PM_NORMAL && !swapRGB && !grayscal && bright == 0)
    return _tmpPixmap;

    QImage _tmpImage = _tmpPixmap.convertToImage();
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

    _tmpImage = _tmpImage.mirror( _horizontal, _vertical );

    if ( depth != 0 ) {
        QImage tmpImg = _tmpImage.convertDepth( depth );
        if ( !tmpImg.isNull() )
            _tmpImage = tmpImg;
    }

    if ( swapRGB )
        _tmpImage = _tmpImage.swapRGB();

    if ( grayscal ) {
        if ( depth == 1 || depth == 8 ) {
            for ( int i = 0; i < _tmpImage.numColors(); ++i ) {
                QRgb rgb = _tmpImage.color( i );
                int gray = qGray( rgb );
                rgb = qRgb( gray, gray, gray );
                _tmpImage.setColor( i, rgb );
            }
        }
        else {
            int _width = _tmpImage.width();
            int _height = _tmpImage.height();
            int _x = 0;
            int _y = 0;

            for ( _x = 0; _x < _width; ++_x ) {
                for ( _y = 0; _y < _height; ++_y ) {
                    if ( _tmpImage.valid( _x, _y ) ) {
                        QRgb rgb = _tmpImage.pixel( _x, _y );
                        int gray = qGray( rgb );
                        rgb = qRgb( gray, gray, gray );
                        _tmpImage.setPixel( _x, _y, rgb );
                    }
                }
            }
        }
    }

    if ( bright != 0 ) {
        if ( depth == 1 || depth == 8 ) {
            for ( int i = 0; i < _tmpImage.numColors(); ++i ) {
                QRgb rgb = _tmpImage.color( i );
                QColor c( rgb );

                if ( bright > 0 )
                    rgb = c.light( 100 + bright ).rgb();
                else
                    rgb = c.dark( 100 + abs( bright ) ).rgb();

                _tmpImage.setColor( i, rgb );
            }
        }
        else {
            int _width = _tmpImage.width();
            int _height = _tmpImage.height();
            int _x = 0;
            int _y = 0;

            for ( _x = 0; _x < _width; ++_x ) {
                for ( _y = 0; _y < _height; ++_y ) {
                    if ( _tmpImage.valid( _x, _y ) ) {
                        QRgb rgb = _tmpImage.pixel( _x, _y );
                        QColor c( rgb );

                        if ( bright > 0 )
                            rgb = c.light( 100 + bright ).rgb();
                        else
                            rgb = c.dark( 100 + abs( bright ) ).rgb();

                        _tmpImage.setPixel( _x, _y, rgb );
                    }
                }
            }
        }
    }

    _tmpPixmap.convertFromImage( _tmpImage );

    QPixmap tmpPix( _tmpPixmap.size() );
    tmpPix.fill( Qt::white );

    QPainter _p;
    _p.begin( &tmpPix );
    _p.drawPixmap( 0, 0, _tmpPixmap );
    _p.end();

    return tmpPix;
}
