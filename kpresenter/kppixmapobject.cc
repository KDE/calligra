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
#include <kdebug.h>

// for getenv ()
#include <stdlib.h>
using namespace std;

/******************************************************************/
/* Class: KPPixmapObject                                          */
/******************************************************************/

/*================ default constructor ===========================*/
KPPixmapObject::KPPixmapObject( KPImageCollection *_imageCollection )
    : KPObject()
{
    imageCollection = _imageCollection;
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
KPPixmapObject::KPPixmapObject( KPImageCollection *_imageCollection, const QString &_filename,
                                QDateTime _lastModified )
    : KPObject()
{
    imageCollection = _imageCollection;

    if ( !_lastModified.isValid() )
    {
        QFileInfo inf( _filename );
        _lastModified = inf.lastModified();
    }

    ext = orig_size;
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

    setPixmap( _filename, _lastModified );
}

/*================================================================*/
KPPixmapObject::~KPPixmapObject()
{
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
void KPPixmapObject::setPixmap( const QString &_filename, QDateTime _lastModified, const QSize &/*_size*/ )
{
    if ( !_lastModified.isValid() )
    {
        QFileInfo inf( _filename );
        _lastModified = inf.lastModified();
    }

    image = imageCollection->loadImage( KPImageKey( _filename, _lastModified ) );

    if ( ext == orig_size )
        ext = image.size();

    image = image.scale( ext );

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
QDomDocumentFragment KPPixmapObject::save( QDomDocument& doc )
{
    QDomDocumentFragment fragment=KPObject::save(doc);
    fragment.appendChild(KPObject::createValueElement("FILLTYPE", static_cast<int>(fillType), doc));
    fragment.appendChild(KPObject::createGradientElement("GRADIENT", gColor1, gColor2, static_cast<int>(gType),
                                                         unbalanced, xfactor, yfactor, doc));
    fragment.appendChild(KPObject::createPenElement("PEN", pen, doc));
    fragment.appendChild(KPObject::createBrushElement("BRUSH", brush, doc));
    QDomElement elem=doc.createElement("KEY");
    image.key().setAttributes(elem);
    fragment.appendChild(elem);
    return fragment;
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
            KPImageKey key;
            QSize size;
            int year, month, day, hour, minute, second, msec;

            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it )
            {
                if ( ( *it ).m_strName == "filename" )
                    key.filename = ( *it ).m_strValue;
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
                    size.setWidth( ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "height" )
                    size.setHeight( ( *it ).m_strValue.toInt() );
            }
            key.lastModified.setDate( QDate( year, month, day ) );
            key.lastModified.setTime( QTime( hour, minute, second, msec ) );

            // ### the size attributes seem unused. What are they supposed to
            // be used for?

            // create a 'temporary' image. Later on reload() will be called
            // where we load the real image.
            image = KPImage( key, QImage() );
        }

        // pixmap
        else if ( name == "PIXMAP" )
        {
            KPImageKey key;

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

            key.filename = _fileName;
            key.lastModified.setDate( imageCollection->tmpDate() );
            key.lastModified.setTime( imageCollection->tmpTime() );

            if ( openPic )
                image = imageCollection->loadImage( key );
            else
                image = imageCollection->loadImage( key, _data );

            if ( ext == orig_size )
                ext = image.size();

            image = image.scale( ext );
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
