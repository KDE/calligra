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

#include "kpautoformobject.h"
#include "kpresenter_utils.h"
#include "kpgradient.h"

#include <qpointarray.h>
#include <qlist.h>
#include <qregion.h>
#include <qpicture.h>
#include <qpainter.h>
#include <qwmatrix.h>
#include <kstddirs.h>
#include <kdebug.h>

#include <kapp.h>

#include <math.h>
#include <iostream>
using namespace std;

/******************************************************************/
/* Class: KPAutoformObject                                        */
/******************************************************************/

/*================ default constructor ===========================*/
KPAutoformObject::KPAutoformObject()
    : KPObject(), pen(), brush(), filename(), gColor1( Qt::red ), gColor2( Qt::green ), atfInterp( 0, "" ), pix()
{
    lineBegin = L_NORMAL;
    lineEnd = L_NORMAL;
    gradient = 0;
    fillType = FT_BRUSH;
    gType = BCT_GHORZ;
    drawShadow = false;
    redrawPix = false;
    unbalanced = false;
    xfactor = 100;
    yfactor = 100;
}

/*================== overloaded constructor ======================*/
KPAutoformObject::KPAutoformObject( QPen _pen, QBrush _brush, QString _filename, LineEnd _lineBegin, LineEnd _lineEnd,
                                    FillType _fillType, QColor _gColor1, QColor _gColor2, BCType _gType,
                                    bool _unbalanced, int _xfactor, int _yfactor)
    : KPObject(), pen( _pen ), brush( _brush ), filename( _filename ), gColor1( _gColor1 ), gColor2( _gColor2 ), atfInterp( 0, "" ), pix()
{
    atfInterp.load( filename );
    lineBegin = _lineBegin;
    lineEnd = _lineEnd;
    gType = _gType;
    fillType = _fillType;
    drawShadow = false;
    redrawPix = true;
    unbalanced = _unbalanced;
    xfactor = _xfactor;
    yfactor = _yfactor;

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
KPAutoformObject &KPAutoformObject::operator=( const KPAutoformObject & )
{
    return *this;
}

/*================================================================*/
void KPAutoformObject::setSize( int _width, int _height )
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
void KPAutoformObject::resizeBy( int _dx, int _dy )
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

/*====================== set filename ============================*/
void KPAutoformObject::setFileName( QString _filename )
{
    filename = _filename;
    atfInterp.load( filename );
}

/*================================================================*/
void KPAutoformObject::setFillType( FillType _fillType )
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

/*========================= save =================================*/
void KPAutoformObject::save( QTextStream& out )
{
    out << indent << "<ORIG x=\"" << orig.x() << "\" y=\"" << orig.y() << "\"/>" << endl;
    out << indent << "<SIZE width=\"" << ext.width() << "\" height=\"" << ext.height() << "\"/>" << endl;
    out << indent << "<SHADOW distance=\"" << shadowDistance << "\" direction=\""
        << static_cast<int>( shadowDirection ) << "\" red=\"" << shadowColor.red() << "\" green=\"" << shadowColor.green()
        << "\" blue=\"" << shadowColor.blue() << "\"/>" << endl;
    out << indent << "<EFFECTS effect=\"" << static_cast<int>( effect ) << "\" effect2=\""
        << static_cast<int>( effect2 ) << "\"/>" << endl;
    out << indent << "<PEN red=\"" << pen.color().red() << "\" green=\"" << pen.color().green()
        << "\" blue=\"" << pen.color().blue() << "\" width=\"" << pen.width()
        << "\" style=\"" << static_cast<int>( pen.style() ) << "\"/>" << endl;
    out << indent << "<BRUSH red=\"" << brush.color().red() << "\" green=\"" << brush.color().green()
        << "\" blue=\"" << brush.color().blue() << "\" style=\"" << static_cast<int>( brush.style() ) << "\"/>" << endl;
    out << indent << "<LINEBEGIN value=\"" << static_cast<int>( lineBegin ) << "\"/>" << endl;
    out << indent << "<LINEEND value=\"" << static_cast<int>( lineEnd ) << "\"/>" << endl;
    out << indent << "<PRESNUM value=\"" << presNum << "\"/>" << endl;
    out << indent << "<ANGLE value=\"" << angle << "\"/>" << endl;

    QString afDir = locate( "autoforms", "" );
    int len = afDir.length();
    QString str = filename;
    str = str.remove( 0, len );
    out << indent << "<FILENAME value=\"" << str << "\"/>" << endl;
    out << indent << "<FILLTYPE value=\"" << static_cast<int>( fillType ) << "\"/>" << endl;
    out << indent << "<GRADIENT red1=\"" << gColor1.red() << "\" green1=\"" << gColor1.green()
        << "\" blue1=\"" << gColor1.blue() << "\" red2=\"" << gColor2.red() << "\" green2=\""
        << gColor2.green() << "\" blue2=\"" << gColor2.blue() << "\" type=\""
        << static_cast<int>( gType ) << "\" unbalanced=\"" << unbalanced << "\" xfactor=\"" << xfactor
        << "\" yfactor=\"" << yfactor << "\"/>" << endl;
    out << indent << "<DISAPPEAR effect=\"" << static_cast<int>( effect3 ) << "\" doit=\"" << static_cast<int>( disappear )
        << "\" num=\"" << disappearNum << "\"/>" << endl;
}

/*========================== load ================================*/
void KPAutoformObject::load( KOMLParser& parser, QValueList<KOMLAttrib>& lst )
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

        // lineBegin
        else if ( name == "LINEBEGIN" )
        {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it )
            {
                if ( ( *it ).m_strName == "value" )
                    lineBegin = ( LineEnd )( *it ).m_strValue.toInt();
            }
        }

        // lineEnd
        else if ( name == "LINEEND" )
        {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it )
            {
                if ( ( *it ).m_strName == "value" )
                    lineEnd = ( LineEnd )( *it ).m_strValue.toInt();
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

        // filename
        else if ( name == "FILENAME" )
        {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it )
            {
                if ( ( *it ).m_strName == "value" )
                {
                    filename = ( *it ).m_strValue;
                    // huh? (Werner)
                    QString afDir = locate( "autoforms", filename );
                    atfInterp.load( filename );
                }
            }
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
            kdError() << "Unknown tag '" << tag << "' in AUTOFORM_OBJECT" << endl;

        if ( !parser.close( tag ) )
        {
            kdError() << "ERR: Closing Child" << endl;
            return;
        }
    }
}

/*========================= draw =================================*/
void KPAutoformObject::draw( QPainter *_painter, int _diffx, int _diffy )
{
    if ( move )
    {
        KPObject::draw( _painter, _diffx, _diffy );
        return;
    }

    int ox = orig.x() - _diffx;
    int oy = orig.y() - _diffy;
    int ow = ext.width();
    int oh = ext.height();
    QRect r;

    _painter->save();

    if ( shadowDistance > 0 )
    {
        drawShadow = true;
        QPen tmpPen( pen );
        pen.setColor( shadowColor );
        QBrush tmpBrush( brush );
        brush.setColor( shadowColor );
        r = _painter->viewport();

        if ( angle == 0 )
        {
            int sx = ox;
            int sy = oy;
            getShadowCoords( sx, sy, shadowDirection, shadowDistance );

            _painter->setViewport( sx, sy, r.width(), r.height() );
            paint( _painter );
        }
        else
        {
            _painter->setViewport( ox, oy, r.width(), r.height() );

            QRect br = QRect( 0, 0, ow, oh );
            int pw = br.width();
            int ph = br.height();
            QRect rr = br;
            int yPos = -rr.y();
            int xPos = -rr.x();
            rr.moveTopLeft( QPoint( -rr.width() / 2, -rr.height() / 2 ) );

            int sx = 0;
            int sy = 0;
            getShadowCoords( sx, sy, shadowDirection, shadowDistance );

            QWMatrix m, mtx, m2;
            mtx.rotate( angle );
            m.translate( pw / 2, ph / 2 );
            m2.translate( rr.left() + xPos + sx, rr.top() + yPos + sy );
            m = m2 * mtx * m;

            _painter->setWorldMatrix( m );
            paint( _painter );
        }

        _painter->setViewport( r );
        pen = tmpPen;
        brush = tmpBrush;
    }

    drawShadow = false;

    _painter->restore();
    _painter->save();

    r = _painter->viewport();
    _painter->setViewport( ox, oy, r.width(), r.height() );

    if ( angle == 0 )
        paint( _painter );
    else
    {
        QRect br = QRect( 0, 0, ow, oh );
        int pw = br.width();
        int ph = br.height();
        QRect rr = br;
        int yPos = -rr.y();
        int xPos = -rr.x();
        rr.moveTopLeft( QPoint( -rr.width() / 2, -rr.height() / 2 ) );

        QWMatrix m, mtx, m2;
        mtx.rotate( angle );
        m.translate( pw / 2, ph / 2 );
        m2.translate( rr.left() + xPos, rr.top() + yPos );
        m = m2 * mtx * m;

        _painter->setWorldMatrix( m );
        paint( _painter );
    }

    _painter->setViewport( r );

    _painter->restore();

    KPObject::draw( _painter, _diffx, _diffy );
}

/*===================== get angle ================================*/
float KPAutoformObject::getAngle( QPoint p1, QPoint p2 )
{
    float _angle = 0.0;

    if ( p1.x() == p2.x() )
    {
        if ( p1.y() < p2.y() )
            _angle = 270.0;
        else
            _angle = 90.0;
    }
    else
    {
        float x1, x2, y1, y2;

        if ( p1.x() <= p2.x() )
        {
            x1 = p1.x(); y1 = p1.y();
            x2 = p2.x(); y2 = p2.y();
        }
        else
        {
            x2 = p1.x(); y2 = p1.y();
            x1 = p2.x(); y1 = p2.y();
        }

        float m = -( y2 - y1 ) / ( x2 - x1 );
        _angle = atan( m ) * RAD_FACTOR;

        if ( p1.x() < p2.x() )
            _angle = 180.0 - _angle;
        else
            _angle = -_angle;
    }

    return _angle;
}

/*======================== paint =================================*/
void KPAutoformObject::paint( QPainter* _painter )
{
    unsigned int pw = 0, pwOrig = 0, px, py;

    _painter->setPen( pen );
    pwOrig = pen.width() + 3;
    _painter->setBrush( brush );

    QPointArray pntArray = atfInterp.getPointArray( ext.width(), ext.height() );
    QList<ATFInterpreter::AttribList> atrLs = atfInterp.getAttribList();
    QPointArray pntArray2( pntArray.size() );
    for ( unsigned int i = 0; i < pntArray.size(); i++ )
    {
        px = pntArray.at( i ).x();
        py = pntArray.at( i ).y();
        if ( atrLs.at( i )->pwDiv > 0 )
        {
            pw = pwOrig / atrLs.at( i )->pwDiv;
            if ( px < static_cast<unsigned int>( ext.width() ) / 2 ) px += pw;
            if ( py < static_cast<unsigned int>( ext.height() ) / 2 ) py += pw;
            if ( px > static_cast<unsigned int>( ext.width() ) / 2 ) px -= pw;
            if ( py > static_cast<unsigned int>( ext.height() ) / 2 ) py -= pw;
        }
        pntArray2.setPoint( i, px, py );
    }

    if ( pntArray2.size() > 0 )
    {
        if ( pntArray2.at( 0 ) == pntArray2.at( pntArray2.size() - 1 ) )
        {
            if ( drawShadow || fillType == FT_BRUSH || !gradient )
                _painter->drawPolygon( pntArray2 );
            else
            {
                if ( angle == 0 )
                {
                    int ox = _painter->viewport().x() + static_cast<int>( _painter->worldMatrix().dx() );
                    int oy = _painter->viewport().y() + static_cast<int>( _painter->worldMatrix().dy() );

                    QPointArray pntArray3 = pntArray2.copy();
                    pntArray3.translate( ox, oy );
                    _painter->save();

                    QRegion clipregion( pntArray3 );

                    if ( _painter->hasClipping() )
                        clipregion = _painter->clipRegion().intersect( clipregion );

                    _painter->setClipRegion( clipregion );

                    _painter->drawPixmap( 0, 0, *gradient->getGradient() );

                    _painter->restore();
                }
                else
                {
                    if ( redrawPix )
                    {
                        redrawPix = false;
                        QRegion clipregion( pntArray2 );
                        QPicture pic;
                        QPainter p;

                        p.begin( &pic );
                        p.setClipRegion( clipregion );
                        p.drawPixmap( 0, 0, *gradient->getGradient() );
                        p.end();

                        pix.fill( Qt::white );
                        QPainter p2;
                        p2.begin( &pix );
                        p2.drawPicture( pic );
                        p2.end();
                    }

                    _painter->drawPixmap( 0, 0, pix );
                }

                _painter->setPen( pen );
                _painter->setBrush( Qt::NoBrush );
                _painter->drawPolygon( pntArray2 );
            }
        }
        else
        {
            QSize diff1( 0, 0 ), diff2( 0, 0 );
            int _w = pen.width();

            if ( lineBegin != L_NORMAL )
                diff1 = getBoundingSize( lineBegin, _w );

            if ( lineEnd != L_NORMAL )
                diff2 = getBoundingSize( lineEnd, _w );

            if ( pntArray.size() > 1 )
            {
                if ( lineBegin != L_NORMAL )
                {
                    QPoint pnt1( pntArray2.at( 0 ) ), pnt2( pntArray2.at( 1 ) ), pnt3, pnt4( pntArray.at( 0 ) );
                    float _angle = getAngle( pnt1, pnt2 );

                    switch ( static_cast<int>( _angle ) )
                    {
                    case 0:
                    {
                        pnt3.setX( pnt4.x() - diff1.width() / 2 );
                        pnt3.setY( pnt1.y() );
                    } break;
                    case 180:
                    {
                        pnt3.setX( pnt4.x() + diff1.width() / 2 );
                        pnt3.setY( pnt1.y() );
                    } break;
                    case 90:
                    {
                        pnt3.setX( pnt1.x() );
                        pnt3.setY( pnt4.y() - diff1.width() / 2 );
                    } break;
                    case 270:
                    {
                        pnt3.setX( pnt1.x() );
                        pnt3.setY( pnt4.y() + diff1.width() / 2 );
                    } break;
                    default:
                        pnt3 = pnt1;
                        break;
                    }

                    drawFigure( lineBegin, _painter, pnt3, pen.color(), _w, _angle );
                }

                if ( lineEnd != L_NORMAL )
                {
                    QPoint pnt1( pntArray2.at( pntArray2.size() - 1 ) ), pnt2( pntArray2.at( pntArray2.size() - 2 ) );
                    QPoint  pnt3, pnt4( pntArray.at( pntArray.size() - 1 ) );
                    float _angle = getAngle( pnt1, pnt2 );

                    switch ( ( int )_angle )
                    {
                    case 0:
                    {
                        pnt3.setX( pnt4.x() - diff2.width() / 2 );
                        pnt3.setY( pnt1.y() );
                    } break;
                    case 180:
                    {
                        pnt3.setX( pnt4.x() + diff2.width() / 2 );
                        pnt3.setY( pnt1.y() );
                    } break;
                    case 90:
                    {
                        pnt3.setX( pnt1.x() );
                        pnt3.setY( pnt4.y() - diff2.width() / 2 );
                    } break;
                    case 270:
                    {
                        pnt3.setX( pnt1.x() );
                        pnt3.setY( pnt4.y() + diff2.width() / 2 );
                    } break;
                    default:
                        pnt3 = pnt1;
                        break;
                    }

                    drawFigure( lineEnd, _painter, pnt3, pen.color(), _w, _angle );
                }
            }

            _painter->setPen( pen );
            _painter->drawPolyline( pntArray2 );
        }

    }
}






