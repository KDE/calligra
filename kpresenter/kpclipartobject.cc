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
#include <qfileinfo.h>
#include <kdebug.h>
using namespace std;

/******************************************************************/
/* Class: KPClipartObject                                        */
/******************************************************************/

/*================ default constructor ===========================*/
KPClipartObject::KPClipartObject( KPClipartCollection *_clipartCollection )
    : KP2DObject()
{
    clipartCollection = _clipartCollection;
    pen = QPen( Qt::black, 1, Qt::NoPen );
    brush = Qt::NoBrush;
    picture = 0L;
}

/*================== overloaded constructor ======================*/
KPClipartObject::KPClipartObject( KPClipartCollection *_clipartCollection, const QString &_filename, QDateTime _lastModified )
    : KP2DObject()
{
    clipartCollection = _clipartCollection;
    pen = QPen( Qt::black, 1, Qt::NoPen );
    brush = Qt::NoBrush;

    if ( !_lastModified.isValid() )
    {
        QFileInfo inf( _filename );
        _lastModified = inf.lastModified();
    }

    picture = 0L;
    setFileName( _filename, _lastModified );
}

/*================================================================*/
KPClipartObject &KPClipartObject::operator=( const KPClipartObject & )
{
    return *this;
}

/*================================================================*/
void KPClipartObject::setFileName( const QString &_filename, QDateTime _lastModified )
{
    if ( !_lastModified.isValid() )
    {
        QFileInfo inf( _filename );
        _lastModified = inf.lastModified();
    }

    if ( picture )
        clipartCollection->removeRef( key );

    key = KPClipartCollection::Key( _filename, _lastModified );
    picture = clipartCollection->findClipart( key );
}

/*========================= save =================================*/
QDomDocumentFragment KPClipartObject::save( QDomDocument& doc )
{
    QDomDocumentFragment fragment=KP2DObject::save(doc);
    QDomElement elem=doc.createElement("KEY");
    key.setAttributes(elem);
    fragment.appendChild(elem);
    return fragment;
}

/*========================== load ================================*/
void KPClipartObject::load(const QDomElement &element)
{
    KP2DObject::load(element);
    QDomElement e=element.namedItem("KEY").toElement();
    if(!e.isNull())
        key.loadAttributes(e, clipartCollection->tmpDate(), clipartCollection->tmpTime());
    else {
        // try to find a FILENAME tag if the KEY is not available...
        e=element.namedItem("FILENAME").toElement();
        if(!e.isNull()) {
            if(e.hasAttribute("filename"))
                key.filename=e.attribute("filename");
            key.lastModified.setDate( clipartCollection->tmpDate() );
            key.lastModified.setTime( clipartCollection->tmpTime() );
        }
    }
}

/*========================= draw =================================*/
void KPClipartObject::draw( QPainter *_painter, int _diffx, int _diffy )
{
    if ( move )
    {
        KPObject::draw( _painter, _diffx, _diffy );
        return;
    }

    if ( !picture )
        return;

    int ox = orig.x() - _diffx;
    int oy = orig.y() - _diffy;
    int ow = ext.width();
    int oh = ext.height();

    _painter->save();

    _painter->setPen( pen );
    _painter->setBrush( brush );

    int penw = pen.width() / 2;

    if ( angle == 0 ) {
        _painter->setPen( Qt::NoPen );
        _painter->setBrush( brush );
        if ( fillType == FT_BRUSH || !gradient )
            _painter->drawRect( ox + penw, oy + penw, ext.width() - 2 * penw, ext.height() - 2 * penw );
        else
            _painter->drawPixmap( ox + penw, oy + penw, *gradient->getGradient(),
                                  0, 0, ow - 2 * penw, oh - 2 * penw );

        // We have to use setViewport here, but it doesn't cumulate with previous transformations
        // (e.g. painter translation set up by kword when embedding kpresenter...)   :(
        _painter->save();
        //QRect r = _painter->window();
        //_painter->translate( ox+1, oy+1 );
        //_painter->scale( 1.0 * (ext.width()-2) / r.width(), 1.0 * (ext.height()-2) / r.height() );
        _painter->setViewport( ox+1, oy+1, ext.width()-2, ext.height()-2 );
        _painter->drawPicture( *picture );
        _painter->restore();

        _painter->setPen( pen );
        _painter->setBrush( Qt::NoBrush );
        _painter->drawRect( ox + penw, oy + penw, ow - 2 * penw, oh - 2 * penw );
    } else {
        _painter->translate( ox, oy );

        QRect br( QPoint( 0, 0 ), ext );
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

        QPixmap pm( pw, ph );
        pm.fill( Qt::white );
        QPainter pnt;
        pnt.begin( &pm );
        pnt.drawPicture( *picture );
        pnt.end();

        _painter->setWorldMatrix( m, true /* always keep previous transformations */ );

        _painter->setPen( Qt::NoPen );
        _painter->setBrush( brush );

        if ( fillType == FT_BRUSH || !gradient )
            _painter->drawRect( rr.left() + pixXPos + penw,
                                rr.top() + pixYPos + penw, ext.width() - 2 * penw, ext.height() - 2 * penw );
        else
            _painter->drawPixmap( rr.left() + pixXPos + penw, rr.top() + pixYPos + penw,
                                  *gradient->getGradient(), 0, 0, ow - 2 * penw, oh - 2 * penw );

        _painter->drawPixmap( br.left() + pixXPos, br.top() + pixYPos, pm );

        _painter->setPen( pen );
        _painter->setBrush( Qt::NoBrush );
        _painter->drawRect( rr.left() + pixXPos + penw, rr.top() + pixYPos + penw, ow - 2 * penw, oh - 2 * penw );

    }

    _painter->restore();

    KPObject::draw( _painter, _diffx, _diffy );
}
