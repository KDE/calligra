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
}

/*================== overloaded constructor ======================*/
KPPixmapObject::KPPixmapObject( KoPictureCollection *_imageCollection, const KoPictureKey & key )
    : KP2DObject()
{
    imageCollection = _imageCollection;

    ext = KoSize(); // invalid size means unset
    brush = Qt::NoBrush;
    pen = QPen( Qt::black, 1, Qt::NoPen );

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
    return fragment;
}

/*========================== load ================================*/
double KPPixmapObject::load(const QDomElement &element)
{
    double offset=KP2DObject::load(element);
    QDomElement e=element.namedItem("KEY").toElement();
    if(!e.isNull()) {
        KoPictureKey key;
        key.loadAttributes(e, QDate( 1970, 1, 1 ), QTime( 0, 0 ) ); // We need a valid date
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
                QDateTime dateTime(  QDate( 1970, 1, 1 ) ); // We need a valid date
                KoPictureKey key( _fileName, dateTime );
                image.clear();
                image.setKey(key);
                QByteArray rawData=_data.utf8(); // XPM is normally ASCII, therefore UTF-8
                rawData[rawData.size()-1]=char(10); // Replace the NULL character by a LINE FEED
                QBuffer buffer(rawData);
                image.loadXpm(&buffer);
            }
        }
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
        if ( angle == 0 )
        {
            double sx = ox;
            double sy = oy;
            getShadowCoords( sx, sy );
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

            double dx = 0, dy = 0;
            getShadowCoords( dx, dy );

            QWMatrix m;
            m.translate( pw / 2, ph / 2 );
            m.rotate( angle );
            m.translate( rr.left() + pixXPos + _zoomHandler->zoomItX( dx ),
                         rr.top() + pixYPos + _zoomHandler->zoomItY( dy ) );

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
            QSize _pixSize =  QSize( _zoomHandler->zoomItX( ow ), _zoomHandler->zoomItY( oh ) );
            QPixmap _pixmap = image.generatePixmap( _pixSize );
            _painter->drawPixmap( QRect( (int)( _zoomHandler->zoomItX( ox ) + penw ),
                                         (int)( _zoomHandler->zoomItY( oy ) + penw ),
                                         (int)( _zoomHandler->zoomItX( ow ) - 2 * penw ),
                                         (int)( _zoomHandler->zoomItY( oh ) - 2 * penw ) ),
                                  _pixmap );
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
            QSize _pixSize =  QSize( _zoomHandler->zoomItX( ow ), _zoomHandler->zoomItY( oh ) );
            QPixmap _pixmap = image.generatePixmap( _pixSize );
            _painter->drawPixmap( QRect( (int)penw, (int)penw,
                                         (int)( _zoomHandler->zoomItX( ow ) - 2 * penw ),
                                         (int)( _zoomHandler->zoomItY( oh ) - 2 * penw ) ),
                                  _pixmap );
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
