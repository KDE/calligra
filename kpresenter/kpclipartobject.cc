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

#include <kpclipartobject.h>
#include <kpgradient.h>

#include <qpainter.h>
#include <qwmatrix.h>
#include <qdom.h>
#include <qpicture.h>
#include <qfileinfo.h>
#include <kdebug.h>
#include <kozoomhandler.h>
using namespace std;

/******************************************************************/
/* Class: KPClipartObject                                        */
/******************************************************************/

/*================ default constructor ===========================*/
KPClipartObject::KPClipartObject( KoPictureCollection *_clipartCollection )
    : KP2DObject()
{
    clipartCollection = _clipartCollection;
    pen = QPen( Qt::black, 1, Qt::NoPen );
    brush = Qt::NoBrush;
}

/*================== overloaded constructor ======================*/
KPClipartObject::KPClipartObject( KoPictureCollection *_clipartCollection, const KoPictureKey & key )
    : KP2DObject()
{
    clipartCollection = _clipartCollection;
    pen = QPen( Qt::black, 1, Qt::NoPen );
    brush = Qt::NoBrush;

    setClipart( key );
}

/*================================================================*/
KPClipartObject &KPClipartObject::operator=( const KPClipartObject & )
{
    return *this;
}

/*================================================================*/
void KPClipartObject::setClipart( const KoPictureKey & key )
{
    /*if ( !_lastModified.isValid() )
    {
        QFileInfo inf( _filename );
        _lastModified = inf.lastModified();
    }*/

    //if ( picture )
    //    clipartCollection->removeRef( key );

    m_clipart = clipartCollection->findPicture( key );
    if ( m_clipart.isNull() )
        kdWarning() << "Clipart not found in collection " << key.toString() << endl;
}

/*========================= save =================================*/
QDomDocumentFragment KPClipartObject::save( QDomDocument& doc, double offset )
{
    QDomDocumentFragment fragment=KP2DObject::save(doc, offset);
    QDomElement elem=doc.createElement("KEY");
    m_clipart.getKey().saveAttributes(elem);
    fragment.appendChild(elem);
    return fragment;
}

/*========================== load ================================*/
double KPClipartObject::load(const QDomElement &element)
{
    double offset=KP2DObject::load(element);
    QDomElement e=element.namedItem("KEY").toElement();
    if(!e.isNull()) {
        KoPictureKey key;
        key.loadAttributes( e );
        m_clipart.clear();
        m_clipart.setKey( key );
    }
    else {
        // try to find a FILENAME tag if the KEY is not available...
        e=element.namedItem("FILENAME").toElement();
        if(!e.isNull()) {
            /*KPClipart key( e.attribute("filename"),
                           QDateTime( clipartCollection->tmpDate(),
                           clipartCollection->tmpTime() ) );*/
            // Loads from the disk directly (unless it's in the collection already?)
            m_clipart = clipartCollection->loadPicture( e.attribute("filename") );
        }
    }
    return offset;
}

/*========================= draw =================================*/
void KPClipartObject::draw( QPainter *_painter, KoZoomHandler*_zoomHandler,
			    SelectionMode selectionMode , bool drawContour )
{
    if ( m_clipart.isNull() )
        return;

    const double ox = orig.x();
    const double oy = orig.y();
    const double ow = ext.width();
    const double oh = ext.height();
    const QSize size = _zoomHandler->zoomSize( ext );

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
    int penw = pen.width() / 2;

    _painter->translate( _zoomHandler->zoomItX(ox), _zoomHandler->zoomItY(oy) );

    kdDebug() << "Angle: " << angle << " (in KPClipartObject::draw)" << endl;
    if ( kAbs( angle ) > DBL_EPSILON )
    {
        kdDebug() << "Non-null angle!" << endl;
        QRect br( QPoint( 0, 0 ), size );
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

        _painter->setWorldMatrix( m, true /* always keep previous transformations */ );
    }

    _painter->setPen( Qt::NoPen );
    if ( !drawContour )
        _painter->setBrush( brush );

    if ( fillType == FT_BRUSH || !gradient || drawContour )
        _painter->drawRect( _zoomHandler->zoomItX( penw ),
                            _zoomHandler->zoomItY( penw ),
                            _zoomHandler->zoomItX( ext.width() - 2 * penw),
                            _zoomHandler->zoomItY( ext.height() - 2 * penw) );
    else {
        gradient->setSize( size );
        _painter->drawPixmap( _zoomHandler->zoomItX( penw ),
                              _zoomHandler->zoomItY( penw ),
                              gradient->pixmap(), 0, 0,
                              _zoomHandler->zoomItX(ow - 2 * penw),
                              _zoomHandler->zoomItY(oh - 2 * penw) );
    }

    if ( !drawContour ) {
        m_clipart.draw(*_painter,
            0, 0,
            _zoomHandler->zoomItX( ext.width()), _zoomHandler->zoomItY( ext.height()),
            0, 0, 0, 0);
    }
    
    _painter->setPen( pen2 );
    _painter->setBrush( Qt::NoBrush );
    _painter->drawRect( _zoomHandler->zoomItX( penw ),
                        _zoomHandler->zoomItY( penw ),
                        _zoomHandler->zoomItX( ow - 2 * penw),
                        _zoomHandler->zoomItY( oh - 2 * penw) );

    _painter->restore();
    KPObject::draw( _painter, _zoomHandler, selectionMode, drawContour );
}
