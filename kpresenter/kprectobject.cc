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

#include "kprectobject.h"
#include "kpgradient.h"

#include <kdebug.h>
#include <qregion.h>
#include <qpainter.h>
#include <qwmatrix.h>
#include <iostream>
using namespace std;

/******************************************************************/
/* Class: KPRectObject                                            */
/******************************************************************/

/*================ default constructor ===========================*/
KPRectObject::KPRectObject()
    : KPObject(), pen(), brush(), gColor1( Qt::red ), gColor2( Qt::green )
{
    xRnd = 0;
    yRnd = 0;
    gradient = 0;
    fillType = FT_BRUSH;
    gType = BCT_GHORZ;
    drawShadow = false;
    unbalanced = false;
    xfactor = 100;
    yfactor = 100;
}

/*================== overloaded constructor ======================*/
KPRectObject::KPRectObject( QPen _pen, QBrush _brush, FillType _fillType,
                            QColor _gColor1, QColor _gColor2, BCType _gType, int _xRnd, int _yRnd,
                            bool _unbalanced, int _xfactor, int _yfactor)
    : KPObject(), pen( _pen ), brush( _brush ), gColor1( _gColor1 ), gColor2( _gColor2 )
{
    xRnd = _xRnd;
    yRnd = _yRnd;
    gType = _gType;
    fillType = _fillType;
    drawShadow = false;
    unbalanced = _unbalanced;
    xfactor = _xfactor;
    yfactor = _yfactor;

    if ( fillType == FT_GRADIENT )
        gradient = new KPGradient( gColor1, gColor2, gType, QSize( 1, 1 ), unbalanced, xfactor, yfactor );
    else
        gradient = 0;
}

/*================================================================*/
KPRectObject &KPRectObject::operator=( const KPRectObject & )
{
    return *this;
}

/*================================================================*/
void KPRectObject::setSize( int _width, int _height )
{
    KPObject::setSize( _width, _height );
    if ( move ) return;

    if ( fillType == FT_GRADIENT && gradient )
        gradient->setSize( getSize() );
}

/*================================================================*/
void KPRectObject::resizeBy( int _dx, int _dy )
{
    KPObject::resizeBy( _dx, _dy );
    if ( move ) return;

    if ( fillType == FT_GRADIENT && gradient )
        gradient->setSize( getSize() );
}

/*================================================================*/
void KPRectObject::setFillType( FillType _fillType )
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
QDomDocumentFragment KPRectObject::save( QDomDocument& doc )
{
    QDomDocumentFragment fragment=KPObject::save(doc);
    fragment.appendChild(KPObject::createValueElement("FILLTYPE", static_cast<int>(fillType), doc));
    fragment.appendChild(KPObject::createGradientElement("GRADIENT", gColor1, gColor2, static_cast<int>(gType),
                                                         unbalanced, xfactor, yfactor, doc));
    fragment.appendChild(KPObject::createPenElement("PEN", pen, doc));
    fragment.appendChild(KPObject::createBrushElement("BRUSH", brush, doc));
    QDomElement elem=doc.createElement("RNDS");
    elem.setAttribute("x", xRnd);
    elem.setAttribute("y", yRnd);
    fragment.appendChild(elem);
    return fragment;
}

/*========================== load ================================*/
void KPRectObject::load( KOMLParser& parser, QValueList<KOMLAttrib>& lst )
{
    QString tag;
    QString name;

    int tmpRndX = xRnd;
    int tmpRndY = yRnd;

    setRnds( 0, 0 );

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

        // recttype
        else if ( name == "RNDS" )
        {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it )
            {
                if ( ( *it ).m_strName == "x" )
                    xRnd = ( *it ).m_strValue.toInt();
                if ( ( *it ).m_strName == "y" )
                    yRnd = ( *it ).m_strValue.toInt();
            }
        }

        // recttype
        else if ( name == "RECTTYPE" )
        {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it )
            {
                if ( ( *it ).m_strName == "value" )
                {
                    if ( static_cast<RectType>( ( *it ).m_strValue.toInt() ) == RT_ROUND )
                        setRnds( tmpRndX, tmpRndY );
                }
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
                    brush.setColor( QColor( ( *it ).m_strValue.toInt(), brush.color().green(),
                                            brush.color().blue() ) );
                if ( ( *it ).m_strName == "green" )
                    brush.setColor( QColor( brush.color().red(), ( *it ).m_strValue.toInt(),
                                            brush.color().blue() ) );
                if ( ( *it ).m_strName == "blue" )
                    brush.setColor( QColor( brush.color().red(), brush.color().green(),
                                            ( *it ).m_strValue.toInt() ) );
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
            kdError() << "Unknown tag '" << tag << "' in RECT_OBJECT" << endl;

        if ( !parser.close( tag ) )
        {
            kdError() << "ERR: Closing Child" << endl;
            return;
        }
    }
}

/*========================= draw =================================*/
void KPRectObject::draw( QPainter *_painter, int _diffx, int _diffy )
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

    _painter->save();

    if ( shadowDistance > 0 )
    {
        drawShadow = true;
        QPen tmpPen( pen );
        pen.setColor( shadowColor );
        QBrush tmpBrush( brush );
        brush.setColor( shadowColor );

        if ( angle == 0 )
        {
            int sx = ox;
            int sy = oy;
            getShadowCoords( sx, sy, shadowDirection, shadowDistance );

            _painter->translate( sx, sy );
            paint( _painter );
        }
        else
        {
            _painter->translate( ox, oy );

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

            _painter->setWorldMatrix( m, true );
            paint( _painter );
        }

        pen = tmpPen;
        brush = tmpBrush;
    }

    drawShadow = false;

    _painter->restore();

    _painter->save();
    _painter->translate( ox, oy );

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

        _painter->setWorldMatrix( m, true );
        paint( _painter );
    }

    _painter->restore();

    KPObject::draw( _painter, _diffx, _diffy );
}

/*======================== paint =================================*/
void KPRectObject::paint( QPainter* _painter )
{
    int ow = ext.width();
    int oh = ext.height();
    int pw = pen.width() / 2;

    if ( drawShadow || fillType == FT_BRUSH || !gradient )
    {
        _painter->setPen( pen );
        _painter->setBrush( brush );
        _painter->drawRoundRect( pw, pw, ow - 2 * pw, oh - 2 * pw, xRnd, yRnd );
    }
    else
    {
          if ( angle == 0 || angle==360 )
                _painter->drawPixmap( pw, pw, *gradient->getGradient(), 0, 0, ow - 2 * pw, oh - 2 * pw );
          else  //rotated
          {
                QPixmap pix( ow - 2 * pw, oh - 2 * pw );
                QPainter p;
                p.begin( &pix );
                p.drawPixmap( 0, 0, *gradient->getGradient() );
                p.end();

                _painter->drawPixmap( pw, pw, pix );
          }

          _painter->setPen( pen );
          _painter->setBrush( Qt::NoBrush );
          _painter->drawRoundRect( pw, pw, ow - 2 * pw, oh - 2 * pw, xRnd, yRnd );
    }
}




