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

#include "kppixmapobject.h"

#include <qpainter.h>
#include <qwmatrix.h>
#include <qfileinfo.h>
#include <qpixmap.h>
#include <qcstring.h>
#include <kdebug.h>
#include <iostream>
#include <stdlib.h>
using namespace std;

/******************************************************************/
/* Class: KPPixmapObject                                          */
/******************************************************************/

/*================ default constructor ===========================*/
KPPixmapObject::KPPixmapObject( KPPixmapCollection *_pixmapCollection )
    : KPObject()
{
    pixmapCollection = _pixmapCollection;
    pixmap = 0L;
    brush = Qt::NoBrush;
    gradient = 0L;
    fillType = FT_BRUSH;
    gType = BCT_GHORZ;
    pen = QPen( Qt::black, 1, Qt::NoPen );
    gColor1 = Qt::red;
    gColor2 = Qt::green;
    unbalanced = false;
    xfactor = 100;
    yfactor = 100;
}

/*================== overloaded constructor ======================*/
KPPixmapObject::KPPixmapObject( KPPixmapCollection *_pixmapCollection, const QString &_filename,
                                QDateTime _lastModified )
    : KPObject()
{
    pixmapCollection = _pixmapCollection;

    if ( !_lastModified.isValid() )
    {
        QFileInfo inf( _filename );
        _lastModified = inf.lastModified();
    }

    ext = orig_size;
    brush = Qt::NoBrush;
    gradient = 0L;
    pixmap = 0L;
    fillType = FT_BRUSH;
    gType = BCT_GHORZ;
    pen = QPen( Qt::black, 1, Qt::NoPen );
    gColor1 = Qt::red;
    gColor2 = Qt::green;
    unbalanced = false;
    xfactor = 100;
    yfactor = 100;

    setPixmap( _filename, _lastModified );
}

/*================================================================*/
KPPixmapObject::~KPPixmapObject()
{
    if ( pixmap )
        pixmapCollection->removeRef( key );
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

    if ( pixmap )
        pixmapCollection->removeRef( key );

    key = KPPixmapCollection::Key( KPPixmapDataCollection::Key( key.dataKey.filename, key.dataKey.lastModified ), ext );
    pixmap = pixmapCollection->findPixmap( key );

    if ( ext == orig_size && pixmap )
        ext = pixmap->size();

    if ( fillType == FT_GRADIENT && gradient )
        gradient->setSize( getSize() );
}

/*======================= set size ===============================*/
void KPPixmapObject::resizeBy( int _dx, int _dy )
{
    KPObject::resizeBy( _dx, _dy );
    if ( move ) return;

    if ( pixmap )
        pixmapCollection->removeRef( key );

    key = KPPixmapCollection::Key( KPPixmapDataCollection::Key( key.dataKey.filename, key.dataKey.lastModified ), ext );
    pixmap = pixmapCollection->findPixmap( key );

    if ( ext == orig_size && pixmap )
        ext = pixmap->size();

    if ( fillType == FT_GRADIENT && gradient )
        gradient->setSize( getSize() );
}

/*================================================================*/
void KPPixmapObject::setPixmap( const QString &_filename, QDateTime _lastModified, const QSize &_size )
{
    if ( !_lastModified.isValid() )
    {
        QFileInfo inf( _filename );
        _lastModified = inf.lastModified();
    }

    if ( pixmap )
        pixmapCollection->removeRef( key );

    key = KPPixmapCollection::Key( KPPixmapDataCollection::Key( _filename, _lastModified ), _size );
    pixmap = pixmapCollection->findPixmap( key );

    if ( ext == orig_size && pixmap )
        ext = pixmap->size();
}

/*================================================================*/
void KPPixmapObject::setFillType( FillType _fillType )
{
    fillType = _fillType;

    if ( fillType == FT_BRUSH && gradient )
    {
        delete gradient;
        gradient = 0;
    }
    if ( fillType == FT_GRADIENT && !gradient )
        gradient = new KPGradient( gColor1, gColor2, gType, getSize(), unbalanced, xfactor, yfactor );
}

/*========================= save =================================*/
void KPPixmapObject::save( QTextStream& out )
{
    out << indent << "<ORIG x=\"" << orig.x() << "\" y=\"" << orig.y() << "\"/>" << endl;
    out << indent << "<SIZE width=\"" << ext.width() << "\" height=\"" << ext.height() << "\"/>" << endl;
    out << indent << "<SHADOW distance=\"" << shadowDistance << "\" direction=\""
        << static_cast<int>( shadowDirection ) << "\" red=\"" << shadowColor.red() << "\" green=\"" << shadowColor.green()
        << "\" blue=\"" << shadowColor.blue() << "\"/>" << endl;
    out << indent << "<EFFECTS effect=\"" << static_cast<int>( effect ) << "\" effect2=\""
        << static_cast<int>( effect2 ) << "\"/>" << endl;
    out << indent << "<PRESNUM value=\"" << presNum << "\"/>" << endl;
    out << indent << "<ANGLE value=\"" << angle << "\"/>" << endl;
    out << indent << "<KEY " << key << "/>" << endl;
    out << indent << "<FILLTYPE value=\"" << static_cast<int>( fillType ) << "\"/>" << endl;
    out << indent << "<GRADIENT red1=\"" << gColor1.red() << "\" green1=\"" << gColor1.green()
        << "\" blue1=\"" << gColor1.blue() << "\" red2=\"" << gColor2.red() << "\" green2=\""
        << gColor2.green() << "\" blue2=\"" << gColor2.blue() << "\" type=\""
        << static_cast<int>( gType ) << "\" unbalanced=\"" << unbalanced << "\" xfactor=\"" << xfactor
        << "\" yfactor=\"" << yfactor << "\"/>" << endl;
    out << indent << "<PEN red=\"" << pen.color().red() << "\" green=\"" << pen.color().green()
        << "\" blue=\"" << pen.color().blue() << "\" width=\"" << pen.width()
        << "\" style=\"" << static_cast<int>( pen.style() ) << "\"/>" << endl;
    out << indent << "<BRUSH red=\"" << brush.color().red() << "\" green=\"" << brush.color().green()
        << "\" blue=\"" << brush.color().blue() << "\" style=\"" << static_cast<int>( brush.style() ) << "\"/>" << endl;
    out << indent << "<DISAPPEAR effect=\"" << static_cast<int>( effect3 ) << "\" doit=\"" << static_cast<int>( disappear )
        << "\" num=\"" << disappearNum << "\"/>" << endl;
}

/*========================== load ================================*/
void KPPixmapObject::load( KOMLParser& parser, QValueList<KOMLAttrib>& lst )
{
    QString tag;
    QString name;

    while ( parser.open( QString::null, tag ) )
    {
        parser.parseTag( tag, name, lst );

        // orig
        if ( name == "ORIG" )
        {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it )
            {
                if ( ( *it ).m_strName == "x" )
                    orig.setX( ( *it ).m_strValue.toInt() );
                if ( ( *it ).m_strName == "y" )
                    orig.setY( ( *it ).m_strValue.toInt() );
            }
        }

        // disappear
        else if ( name == "DISAPPEAR" )
        {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it )
            {
                if ( ( *it ).m_strName == "effect" )
                    effect3 = ( Effect3 )( *it ).m_strValue.toInt();
                if ( ( *it ).m_strName == "doit" )
                    disappear = ( bool )( *it ).m_strValue.toInt();
                if ( ( *it ).m_strName == "num" )
                    disappearNum = ( *it ).m_strValue.toInt();
            }
        }

        // size
        else if ( name == "SIZE" )
        {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it )
            {
                if ( ( *it ).m_strName == "width" )
                    ext.setWidth( ( *it ).m_strValue.toInt() );
                if ( ( *it ).m_strName == "height" )
                    ext.setHeight( ( *it ).m_strValue.toInt() );
            }
        }

        // shadow
        else if ( name == "SHADOW" )
        {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it )
            {
                if ( ( *it ).m_strName == "distance" )
                    shadowDistance = ( *it ).m_strValue.toInt();
                if ( ( *it ).m_strName == "direction" )
                    shadowDirection = ( ShadowDirection )( *it ).m_strValue.toInt();
                if ( ( *it ).m_strName == "red" )
                    shadowColor.setRgb( ( *it ).m_strValue.toInt(),
                                        shadowColor.green(), shadowColor.blue() );
                if ( ( *it ).m_strName == "green" )
                    shadowColor.setRgb( shadowColor.red(), ( *it ).m_strValue.toInt(),
                                        shadowColor.blue() );
                if ( ( *it ).m_strName == "blue" )
                    shadowColor.setRgb( shadowColor.red(), shadowColor.green(),
                                        ( *it ).m_strValue.toInt() );
            }
        }

        // effects
        else if ( name == "EFFECTS" )
        {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it )
            {
                if ( ( *it ).m_strName == "effect" )
                    effect = ( Effect )( *it ).m_strValue.toInt();
                if ( ( *it ).m_strName == "effect2" )
                    effect2 = ( Effect2 )( *it ).m_strValue.toInt();
            }
        }

        // angle
        else if ( name == "ANGLE" )
        {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it )
            {
                if ( ( *it ).m_strName == "value" )
                    angle = ( *it ).m_strValue.toDouble();
            }
        }

        // presNum
        else if ( name == "PRESNUM" )
        {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it )
            {
                if ( ( *it ).m_strName == "value" )
                    presNum = ( *it ).m_strValue.toInt();
            }
        }

        // key
        else if ( name == "KEY" )
        {
            int year, month, day, hour, minute, second, msec;

            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it )
            {
                if ( ( *it ).m_strName == "filename" )
                    key.dataKey.filename = ( *it ).m_strValue;
                else if ( ( *it ).m_strName == "year" )
                    year = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "month" )
                    month = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "day" )
                    day = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "hour" )
                    hour = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "minute" )
                    minute = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "second" )
                    second = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "msec" )
                    msec = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "width" )
                    key.size.setWidth( ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "height" )
                    key.size.setHeight( ( *it ).m_strValue.toInt() );
            }
            key.dataKey.lastModified.setDate( QDate( year, month, day ) );
            key.dataKey.lastModified.setTime( QTime( hour, minute, second, msec ) );
        }

        // pixmap
        else if ( name == "PIXMAP" )
        {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();

            bool openPic = true;
            QString _data;
            QString _fileName;

            for( ; it != lst.end(); ++it )
            {
                if ( ( *it ).m_strName == "data" )
                {
                    _data = ( *it ).m_strValue;
                    if ( _data.isEmpty() )
                        openPic = true;
                    else
                        openPic = false;
                }
                else if ( ( *it ).m_strName == "filename" )
                {
                    _fileName = ( *it ).m_strValue;
                    if ( !_fileName.isEmpty() )
                    {
                        if ( int _envVarB = _fileName.find( '$' ) >= 0 )
                        {
                            int _envVarE = _fileName.find( '/', _envVarB );
                            QString path = getenv( QFile::encodeName(_fileName.mid( _envVarB, _envVarE-_envVarB )) );
                            _fileName.replace( _envVarB-1, _envVarE-_envVarB+1, path );
                        }
                    }
                }
            }

            key.dataKey.filename = _fileName;
            key.dataKey.lastModified.setDate( pixmapCollection->tmpDate() );
            key.dataKey.lastModified.setTime( pixmapCollection->tmpTime() );
            key.size = ext;
            if ( !openPic )
                pixmapCollection->getPixmapDataCollection().setPixmapOldVersion( key.dataKey, _data );
            else
                pixmapCollection->getPixmapDataCollection().setPixmapOldVersion( key.dataKey );
        }

        // pen
        else if ( name == "PEN" )
        {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it )
            {
                if ( ( *it ).m_strName == "red" )
                    pen.setColor( QColor( ( *it ).m_strValue.toInt(), pen.color().green(), pen.color().blue() ) );
                if ( ( *it ).m_strName == "green" )
                    pen.setColor( QColor( pen.color().red(), ( *it ).m_strValue.toInt(), pen.color().blue() ) );
                if ( ( *it ).m_strName == "blue" )
                    pen.setColor( QColor( pen.color().red(), pen.color().green(), ( *it ).m_strValue.toInt() ) );
                if ( ( *it ).m_strName == "width" )
                    pen.setWidth( ( *it ).m_strValue.toInt() );
                if ( ( *it ).m_strName == "style" )
                    pen.setStyle( ( Qt::PenStyle )( *it ).m_strValue.toInt() );
            }
            setPen( pen );
        }

        // brush
        else if ( name == "BRUSH" )
        {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it )
            {
                if ( ( *it ).m_strName == "red" )
                    brush.setColor( QColor( ( *it ).m_strValue.toInt(), brush.color().green(), brush.color().blue() ) );
                if ( ( *it ).m_strName == "green" )
                    brush.setColor( QColor( brush.color().red(), ( *it ).m_strValue.toInt(), brush.color().blue() ) );
                if ( ( *it ).m_strName == "blue" )
                    brush.setColor( QColor( brush.color().red(), brush.color().green(), ( *it ).m_strValue.toInt() ) );
                if ( ( *it ).m_strName == "style" )
                    brush.setStyle( ( Qt::BrushStyle )( *it ).m_strValue.toInt() );
            }
            setBrush( brush );
        }

        // fillType
        else if ( name == "FILLTYPE" )
        {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it )
            {
                if ( ( *it ).m_strName == "value" )
                    fillType = static_cast<FillType>( ( *it ).m_strValue.toInt() );
            }
            setFillType( fillType );
        }

        // gradient
        else if ( name == "GRADIENT" )
        {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it )
            {
                if ( ( *it ).m_strName == "red1" )
                    gColor1 = QColor( ( *it ).m_strValue.toInt(), gColor1.green(), gColor1.blue() );
                if ( ( *it ).m_strName == "green1" )
                    gColor1 = QColor( gColor1.red(), ( *it ).m_strValue.toInt(), gColor1.blue() );
                if ( ( *it ).m_strName == "blue1" )
                    gColor1 = QColor( gColor1.red(), gColor1.green(), ( *it ).m_strValue.toInt() );
                if ( ( *it ).m_strName == "red2" )
                    gColor2 = QColor( ( *it ).m_strValue.toInt(), gColor2.green(), gColor2.blue() );
                if ( ( *it ).m_strName == "green2" )
                    gColor2 = QColor( gColor2.red(), ( *it ).m_strValue.toInt(), gColor2.blue() );
                if ( ( *it ).m_strName == "blue2" )
                    gColor2 = QColor( gColor2.red(), gColor2.green(), ( *it ).m_strValue.toInt() );
                if ( ( *it ).m_strName == "type" )
                    gType = static_cast<BCType>( ( *it ).m_strValue.toInt() );
                if ( ( *it ).m_strName == "unbalanced" )
                    unbalanced = static_cast<bool>( ( *it ).m_strValue.toInt() );
                if ( ( *it ).m_strName == "xfactor" )
                    xfactor = ( *it ).m_strValue.toInt();
                if ( ( *it ).m_strName == "yfactor" )
                    yfactor = ( *it ).m_strValue.toInt();
            }
            setGColor1( gColor1 );
            setGColor2( gColor2 );
            setGType( gType );
            setGUnbalanced( unbalanced );
            setGXFactor( xfactor );
            setGYFactor( yfactor );
        }

        else
            kdError() << "Unknown tag '" << tag << "' in PIXMAP_OBJECT" << endl;

        if ( !parser.close( tag ) )
        {
            kdError() << "ERR: Closing Child" << endl;
            return;
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

    if ( !pixmap ) return;

    int ox = orig.x() - _diffx;
    int oy = orig.y() - _diffy;
    int ow = ext.width();
    int oh = ext.height();
    QRect r;

    _painter->save();
    r = _painter->viewport();

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

            QSize bs = pixmap->size();

            _painter->drawRect( sx, sy, bs.width(), bs.height() );
        }
        else
        {
            r = _painter->viewport();
            _painter->setViewport( ox, oy, r.width(), r.height() );

            QRect br = pixmap->rect();
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

            QSize bs = pixmap->size();
            int dx = 0, dy = 0;
            getShadowCoords( dx, dy, shadowDirection, shadowDistance );
            _painter->drawRect( rr.left() + pixXPos + dx, rr.top() + pixYPos + dy,
                                bs.width(), bs.height() );

            _painter->setViewport( r );
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

        _painter->drawPixmap( ox, oy, *pixmap );

        _painter->setPen( pen );
        _painter->setBrush( Qt::NoBrush );
        _painter->drawRect( ox + penw, oy + penw, ow - 2 * penw, oh - 2 * penw );
    } else {
        r = _painter->viewport();
        _painter->setViewport( ox, oy, r.width(), r.height() );

        QRect br = pixmap->rect();
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

        _painter->drawPixmap( rr.left() + pixXPos, rr.top() + pixYPos, *pixmap );

        _painter->setPen( pen );
        _painter->setBrush( Qt::NoBrush );
        _painter->drawRect( rr.left() + pixXPos + penw, rr.top() + pixYPos + penw, ow - 2 * penw, oh - 2 * penw );

        _painter->setViewport( r );
    }
    _painter->restore();

    KPObject::draw( _painter, _diffx, _diffy );
}




