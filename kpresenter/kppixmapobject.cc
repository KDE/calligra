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

#include <qpainter.h>
#include <qwmatrix.h>
#include <qfileinfo.h>
#include <qpixmap.h>
#include <qdom.h>
#include <kdebug.h>

// for getenv ()
#include <stdlib.h>
using namespace std;

/******************************************************************/
/* Class: KPPixmapObject                                          */
/******************************************************************/

/*================ default constructor ===========================*/
KPPixmapObject::KPPixmapObject( KPImageCollection *_imageCollection )
    : KP2DObject()
{
    imageCollection = _imageCollection;
    brush = Qt::NoBrush;
    pen = QPen( Qt::black, 1, Qt::NoPen );
}

/*================== overloaded constructor ======================*/
KPPixmapObject::KPPixmapObject( KPImageCollection *_imageCollection, const KPImageKey & key )
    : KP2DObject()
{
    imageCollection = _imageCollection;

    ext = orig_size;
    brush = Qt::NoBrush;
    pen = QPen( Qt::black, 1, Qt::NoPen );

    setPixmap( key );
}

/*================================================================*/
KPPixmapObject &KPPixmapObject::operator=( const KPPixmapObject & )
{
    return *this;
}

/*======================= set size ===============================*/
void KPPixmapObject::setSize( int _width, int _height )
{
    KPObject::setSize( _width, _height );
    if ( move ) return;

    if ( ext == orig_size )
        ext = image.size();

    image = image.scale( ext );

    if ( fillType == FT_GRADIENT && gradient )
        gradient->setSize( getSize() );
}

/*======================= set size ===============================*/
void KPPixmapObject::resizeBy( int _dx, int _dy )
{
    KPObject::resizeBy( _dx, _dy );
    if ( move ) return;

    if ( ext == orig_size )
        ext = image.size();

    image = image.scale( ext );

    if ( fillType == FT_GRADIENT && gradient )
        gradient->setSize( getSize() );
}

/*================================================================*/
void KPPixmapObject::setPixmap( const KPImageKey & key, const QSize &/*_size*/ )
{
    image = imageCollection->findImage( key );

    if ( ext == orig_size )
        ext = image.size();

    image = image.scale( ext );

}

/*========================= save =================================*/
QDomDocumentFragment KPPixmapObject::save( QDomDocument& doc )
{
    QDomDocumentFragment fragment=KP2DObject::save(doc);
    QDomElement elem=doc.createElement("KEY");
    image.key().saveAttributes(elem);
    fragment.appendChild(elem);
    return fragment;
}

/*========================== load ================================*/
void KPPixmapObject::load(const QDomElement &element)
{
    KP2DObject::load(element);
    QDomElement e=element.namedItem("KEY").toElement();
    if(!e.isNull()) {
        KPImageKey key;
        key.loadAttributes(e, imageCollection->tmpDate(), imageCollection->tmpTime());
        image = KPImage( key, QImage() );
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
                image = imageCollection->loadImage( _fileName );
            else
            {
                QDateTime dateTime( imageCollection->tmpDate(), imageCollection->tmpTime() );
                KPImageKey key( _fileName, dateTime );
                image = imageCollection->loadXPMImage( key, _data );
            }

            if ( ext == orig_size )
                ext = image.size();

            image = image.scale( ext );
        }
    }
}

/*========================= draw =================================*/
void KPPixmapObject::draw( QPainter *_painter, int _diffx, int _diffy )
{
    if ( move )
    {
        KPObject::draw( _painter, _diffx, _diffy );
        return;
    }

    if ( image.isNull() ) return;

    int ox = orig.x() - _diffx;
    int oy = orig.y() - _diffy;
    int ow = ext.width();
    int oh = ext.height();

    _painter->save();

    _painter->setPen( pen );
    _painter->setBrush( brush );

    int penw = pen.width() / 2;

    if ( shadowDistance > 0 )
    {
        if ( angle == 0 )
        {
            int sx = ox;
            int sy = oy;
            getShadowCoords( sx, sy, shadowDirection, shadowDistance );

            _painter->setPen( QPen( shadowColor ) );
            _painter->setBrush( shadowColor );

            QSize bs = image.size();

            _painter->drawRect( sx, sy, bs.width(), bs.height() );
        }
        else
        {
            _painter->translate( ox, oy );

            QSize bs = image.size();
            QRect br = QRect( 0, 0, bs.width(), bs.height() );
            int pw = br.width();
            int ph = br.height();
            QRect rr = br;
            int pixYPos = -rr.y();
            int pixXPos = -rr.x();
            br.moveTopLeft( QPoint( -br.width() / 2, -br.height() / 2 ) );
            rr.moveTopLeft( QPoint( -rr.width() / 2, -rr.height() / 2 ) );

            QWMatrix m, mtx;
            mtx.rotate( angle );
            m.translate( pw / 2, ph / 2 );
            m = mtx * m;

            _painter->setWorldMatrix( m );

            _painter->setPen( QPen( shadowColor ) );
            _painter->setBrush( shadowColor );

            int dx = 0, dy = 0;
            getShadowCoords( dx, dy, shadowDirection, shadowDistance );
            _painter->drawRect( rr.left() + pixXPos + dx, rr.top() + pixYPos + dy,
                                bs.width(), bs.height() );
        }
    }
    _painter->restore();
    _painter->save();

    if ( angle == 0 ) {
        _painter->setPen( Qt::NoPen );
        _painter->setBrush( brush );
        if ( fillType == FT_BRUSH || !gradient )
            _painter->drawRect( ox + penw, oy + penw, ext.width() - 2 * penw, ext.height() - 2 * penw );
        else
            _painter->drawPixmap( ox + penw, oy + penw, *gradient->getGradient(),
                                  0, 0, ow - 2 * penw, oh - 2 * penw );

        _painter->drawPixmap( ox, oy, image.pixmap() );

        _painter->setPen( pen );
        _painter->setBrush( Qt::NoBrush );
        _painter->drawRect( ox + penw, oy + penw, ow - 2 * penw, oh - 2 * penw );
    } else {
        _painter->translate( ox, oy );

        QSize bs = image.size();
        QRect br = QRect( 0, 0, bs.width(), bs.height() );
        int pw = br.width();
        int ph = br.height();
        QRect rr = br;
        int pixYPos = -rr.y();
        int pixXPos = -rr.x();
        br.moveTopLeft( QPoint( -br.width() / 2, -br.height() / 2 ) );
        rr.moveTopLeft( QPoint( -rr.width() / 2, -rr.height() / 2 ) );

        QWMatrix m, mtx;
        mtx.rotate( angle );
        m.translate( pw / 2, ph / 2 );
        m = mtx * m;

        _painter->setWorldMatrix( m, true );

        _painter->setPen( Qt::NoPen );
        _painter->setBrush( brush );

        if ( fillType == FT_BRUSH || !gradient )
            _painter->drawRect( rr.left() + pixXPos + penw, rr.top() + pixYPos + penw,
                                ext.width() - 2 * penw, ext.height() - 2 * penw );
        else
            _painter->drawPixmap( rr.left() + pixXPos + penw, rr.top() + pixYPos + penw,
                                  *gradient->getGradient(), 0, 0, ow - 2 * penw, oh - 2 * penw );

        _painter->drawPixmap( rr.left() + pixXPos, rr.top() + pixYPos, image.pixmap() );

        _painter->setPen( pen );
        _painter->setBrush( Qt::NoBrush );
        _painter->drawRect( rr.left() + pixXPos + penw, rr.top() + pixYPos + penw, ow - 2 * penw, oh - 2 * penw );
    }
    _painter->restore();

    KPObject::draw( _painter, _diffx, _diffy );
}
