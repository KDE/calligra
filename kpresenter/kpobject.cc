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

#include <KPresenterObjectIface.h>

#include <kpobject.h>
#include <kptextobject.h>

#include <qpainter.h>
#include <qwmatrix.h>
#include <qpointarray.h>
#include <qregion.h>
#include <qdom.h>

#include <stdlib.h>
#include <fstream.h>
#include <math.h>


/******************************************************************/
/* Class: KPObject                                                */
/******************************************************************/

/*======================== constructor ===========================*/
KPObject::KPObject()
    : orig(), ext(), shadowColor( Qt::gray ), sticky( FALSE )
{
    presNum = 0;
    disappearNum = 1;
    effect = EF_NONE;
    effect2 = EF2_NONE;
    effect3 = EF3_NONE;
    disappear = false;
    angle = 0.0;
    shadowDirection = SD_RIGHT_BOTTOM;
    shadowDistance = 0;
    dSelection = true;
    selected = false;
    presFakt = 0.0;
    zoomed = false;
    ownClipping = true;
    subPresStep = 0;
    specEffects = false;
    onlyCurrStep = true;
    inObjList = true;
    cmds = 0;
    move = false;
    sticky = false;
    dcop = 0;
}

KPObject::~KPObject()
{
}

/*================================================================*/
KPObject &KPObject::operator=( const KPObject & )
{
    return *this;
}

QDomDocumentFragment KPObject::save( QDomDocument& doc )
{
    QDomDocumentFragment fragment=doc.createDocumentFragment();
    QDomElement elem=doc.createElement("ORIG");
    elem.setAttribute("x", orig.x());
    elem.setAttribute("y", orig.y());
    fragment.appendChild(elem);
    elem=doc.createElement("SIZE");
    elem.setAttribute("width", ext.width());
    elem.setAttribute("height", ext.height());
    fragment.appendChild(elem);
    elem=doc.createElement("SHADOW");
    elem.setAttribute("distance", shadowDistance);
    elem.setAttribute("direction", static_cast<int>( shadowDirection ));
    elem.setAttribute("red", shadowColor.red());
    elem.setAttribute("green", shadowColor.green());
    elem.setAttribute("blue", shadowColor.blue());
    fragment.appendChild(elem);
    elem=doc.createElement("EFFECTS");
    elem.setAttribute("effect", static_cast<int>( effect ));
    elem.setAttribute("effect2", static_cast<int>( effect2 ));
    fragment.appendChild(elem);
    fragment.appendChild(KPObject::createValueElement("PRESNUM", presNum, doc));
    elem=doc.createElement("ANGLE");
    elem.setAttribute("value", angle);
    fragment.appendChild(elem);
    elem=doc.createElement("DISAPPEAR");
    elem.setAttribute("effect", static_cast<int>( effect3 ));
    elem.setAttribute("doit", static_cast<int>( disappear ));
    elem.setAttribute("num", disappearNum);
    fragment.appendChild(elem);
    return fragment;
}

void KPObject::load(const QDomElement &element) {

    QDomElement e=element.namedItem("ORIG").toElement();
    if(!e.isNull()) {
        int tmp=0;
        if(e.hasAttribute("x"))
            tmp=e.attribute("x").toInt();
        orig.setX(tmp);
        tmp=0;
        if(e.hasAttribute("y"))
            tmp=e.attribute("y").toInt();
        orig.setY(tmp);
    }
    e=element.namedItem("SIZE").toElement();
    if(!e.isNull()) {
        int tmp=0;
        if(e.hasAttribute("width"))
            tmp=e.attribute("width").toInt();
        ext.setWidth(tmp);
        tmp=0;
        if(e.hasAttribute("height"))
            tmp=e.attribute("height").toInt();
        ext.setHeight(tmp);
    }
    e=element.namedItem("SHADOW").toElement();
    if(!e.isNull()) {
        int tmp=0;
        if(e.hasAttribute("distance"))
            tmp=e.attribute("distance").toInt();
        shadowDistance=tmp;
        tmp=0;
        if(e.hasAttribute("direction"))
            tmp=e.attribute("direction").toInt();
        shadowDirection=static_cast<ShadowDirection>(tmp);
        int red=0, green=0, blue=0;
        if(e.hasAttribute("red"))
            red=e.attribute("red").toInt();
        if(e.hasAttribute("green"))
            green=e.attribute("green").toInt();
        if(e.hasAttribute("blue"))
            blue=e.attribute("blue").toInt();
        shadowColor.setRgb(red, green, blue);
    }
    e=element.namedItem("EFFECTS").toElement();
    if(!e.isNull()) {
        int tmp=0;
        if(e.hasAttribute("effect"))
            tmp=e.attribute("effect").toInt();
        effect=static_cast<Effect>(tmp);
        tmp=0;
        if(e.hasAttribute("effect2"))
            tmp=e.attribute("effect2").toInt();
        effect2=static_cast<Effect2>(tmp);
    }
    e=element.namedItem("ANGLE").toElement();
    if(!e.isNull()) {
        float tmp=0.0;
        if(e.hasAttribute("value"))
            tmp=e.attribute("value").toFloat();
        angle=tmp;
    }
    e=element.namedItem("PRESNUM").toElement();
    if(!e.isNull()) {
        int tmp=0;
        if(e.hasAttribute("value"))
            tmp=e.attribute("value").toInt();
        presNum=tmp;
    }
    e=element.namedItem("DISAPPEAR").toElement();
    if(!e.isNull()) {
        int tmp=0;
        if(e.hasAttribute("effect"))
            tmp=e.attribute("effect").toInt();
        effect3=static_cast<Effect3>(tmp);
        tmp=0;
        if(e.hasAttribute("doit"))
            tmp=e.attribute("doit").toInt();
        disappear=static_cast<bool>(tmp);
        tmp=0;
        if(e.hasAttribute("num"))
            tmp=e.attribute("num").toInt();
        disappearNum=tmp;
    }
}

/*======================= get bounding rect ======================*/
QRect KPObject::getBoundingRect( int _diffx, int _diffy ) const
{
    QRect r( orig.x() - _diffx, orig.y() - _diffy,
             ext.width(), ext.height() );

    if ( shadowDistance > 0 )
    {
        int sx = r.x(), sy = r.y();
        getShadowCoords( sx, sy, shadowDirection, shadowDistance );
        QRect r2( sx, sy, r.width(), r.height() );
        r = r.unite( r2 );
    }

    if ( angle == 0.0 )
        return r;
    else
    {
        QWMatrix mtx;
        mtx.rotate( angle );
        QRect rr = mtx.map( r );

        int diffw = std::abs( rr.width() - r.width() );
        int diffh = std::abs( rr.height() - r.height() );

        return QRect( r.x() - diffw, r.y() - diffh,
                      r.width() + diffw * 2, r.height() + diffh * 2 );
    }
}

/*======================== contain point ? =======================*/
bool KPObject::contains( QPoint _point, int _diffx, int _diffy ) const
{
    if ( angle == 0.0 )
    {
        QRect r( orig.x() - _diffx, orig.y() - _diffy,
                 ext.width(), ext.height() );
        return r.contains( _point );
    }
    else
    {
        QRect br = QRect( 0, 0, ext.width(), ext.height() );
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

        QRect r = m.map( br );
        r.moveBy( orig.x() - _diffx, orig.y() - _diffy );

        return r.contains( _point );
    }
}

/*================================================================*/
bool KPObject::intersects( QRect _rect, int _diffx, int _diffy ) const
{
    if ( angle == 0.0 )
    {
        QRect r( orig.x() - _diffx, orig.y() - _diffy,
                 ext.width(), ext.height() );
        return r.intersects( _rect );
    }
    else
    {
        QRect br = QRect( 0, 0, ext.width(), ext.height() );
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

        QRect r = m.map( br );
        r.moveBy( orig.x() - _diffx, orig.y() - _diffy );

        return r.intersects( _rect );
    }
}

/*======================== get cursor ============================*/
QCursor KPObject::getCursor( QPoint _point, int _diffx, int _diffy, ModifyType &_modType ) const
{
    int px = _point.x();
    int py = _point.y();

    int ox = orig.x() - _diffx;
    int oy = orig.y() - _diffy;
    int ow = ext.width();
    int oh = ext.height();

    QRect r( ox, oy, ow, oh );

    if ( !r.contains( _point ) )
        return Qt::arrowCursor;

    if ( px >= ox && py >= oy && px <= ox + 6 && py <= oy + 6 )
    {
        _modType = MT_RESIZE_LU;
        return Qt::sizeFDiagCursor;
    }

    if ( px >= ox && py >= oy + oh / 2 - 3 && px <= ox + 6 && py <= oy + oh / 2 + 3 )
    {
        _modType = MT_RESIZE_LF;
        return Qt::sizeHorCursor;
    }

    if ( px >= ox && py >= oy + oh - 6 && px <= ox + 6 && py <= oy + oh )
    {
        _modType = MT_RESIZE_LD;
        return Qt::sizeBDiagCursor;
    }

    if ( px >= ox + ow / 2 - 3 && py >= oy && px <= ox + ow / 2 + 3 && py <= oy + 6 )
    {
        _modType = MT_RESIZE_UP;
        return Qt::sizeVerCursor;
    }

    if ( px >= ox + ow / 2 - 3 && py >= oy + oh - 6 && px <= ox + ow / 2 + 3 && py <= oy + oh )
    {
        _modType = MT_RESIZE_DN;
        return Qt::sizeVerCursor;
    }

    if ( px >= ox + ow - 6 && py >= oy && px <= ox + ow && py <= oy + 6 )
    {
        _modType = MT_RESIZE_RU;
        return Qt::sizeBDiagCursor;
    }

    if ( px >= ox + ow - 6 && py >= oy + oh / 2 - 3 && px <= ox + ow && py <= oy + oh / 2 + 3 )
    {
        _modType = MT_RESIZE_RT;
        return Qt::sizeHorCursor;
    }

    if ( px >= ox + ow - 6 && py >= oy + oh - 6 && px <= ox + ow && py <= oy + oh )
    {
        _modType = MT_RESIZE_RD;
        return Qt::sizeFDiagCursor;
    }

    _modType = MT_MOVE;
    return Qt::sizeAllCursor;
}

/*========================= zoom =================================*/
void KPObject::zoom( float _fakt )
{
    presFakt = _fakt;

    zoomed = true;

    oldOrig = orig;
    oldExt = ext;

    orig.setX( static_cast<int>( static_cast<float>( orig.x() ) * presFakt ) );
    orig.setY( static_cast<int>( static_cast<float>( orig.y() ) * presFakt ) );
    ext.setWidth( static_cast<int>( static_cast<float>( ext.width() ) * presFakt ) );
    ext.setHeight( static_cast<int>( static_cast<float>( ext.height() ) * presFakt ) );

    setSize( ext );
    setOrig( orig );
}

/*==================== zoom orig =================================*/
void KPObject::zoomOrig()
{
    ASSERT(zoomed);
    zoomed = false;

    orig = oldOrig;
    ext = oldExt;

    setSize( ext );
    setOrig( orig );
}

/*======================== draw ==================================*/
void KPObject::draw( QPainter *_painter, int _diffx, int _diffy )
{
    if ( dSelection )
    {
        _painter->save();
        _painter->translate( orig.x() - _diffx, orig.y() - _diffy );
        paintSelection( _painter );
        _painter->restore();
    }
}

/*====================== get shadow coordinates ==================*/
void KPObject::getShadowCoords( int& _x, int& _y, ShadowDirection /*_direction*/, int /*_distance*/ ) const
{
    int sx = 0, sy = 0;

    switch ( shadowDirection )
    {
    case SD_LEFT_UP:
    {
        sx = _x - shadowDistance;
        sy = _y - shadowDistance;
    } break;
    case SD_UP:
    {
        sx = _x;
        sy = _y - shadowDistance;
    } break;
    case SD_RIGHT_UP:
    {
        sx = _x + shadowDistance;
        sy = _y - shadowDistance;
    } break;
    case SD_RIGHT:
    {
        sx = _x + shadowDistance;
        sy = _y;
    } break;
    case SD_RIGHT_BOTTOM:
    {
        sx = _x + shadowDistance;
        sy = _y + shadowDistance;
    } break;
    case SD_BOTTOM:
    {
        sx = _x;
        sy = _y + shadowDistance;
    } break;
    case SD_LEFT_BOTTOM:
    {
        sx = _x - shadowDistance;
        sy = _y + shadowDistance;
    } break;
    case SD_LEFT:
    {
        sx = _x - shadowDistance;
        sy = _y;
    } break;
    }

    _x = sx; _y = sy;
}

/*======================== paint selection =======================*/
void KPObject::paintSelection( QPainter *_painter )
{
    _painter->save();
    Qt::RasterOp rop = _painter->rasterOp();

    _painter->setRasterOp( Qt::NotROP );

    if ( getType() == OT_TEXT && dynamic_cast<KPTextObject*>( this )->getDrawEditRect() )
    {
        _painter->save();

        if ( angle != 0 )
        {
            QRect br = QRect( 0, 0, ext.width(), ext.height() );
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
        }

        _painter->setPen( QPen( Qt::black, 1, Qt::DotLine ) );
        _painter->setBrush( Qt::NoBrush );
        _painter->drawRect( 0, 0, ext.width(), ext.height() );

        _painter->restore();
    }

    _painter->setPen( QPen( Qt::black, 1, Qt::SolidLine ) );
    _painter->setBrush( Qt::black );

    if ( selected )
    {
        _painter->fillRect( 0, 0, 6, 6, Qt::black );
        _painter->fillRect( 0, ext.height() / 2 - 3, 6, 6, Qt::black );
        _painter->fillRect( 0, ext.height() - 6, 6, 6, Qt::black );
        _painter->fillRect( ext.width() - 6, 0, 6, 6, Qt::black );
        _painter->fillRect( ext.width() - 6, ext.height() / 2 - 3, 6, 6, Qt::black );
        _painter->fillRect( ext.width() - 6, ext.height() - 6, 6, 6, Qt::black );
        _painter->fillRect( ext.width() / 2 - 3, 0, 6, 6, Qt::black );
        _painter->fillRect( ext.width() / 2 - 3, ext.height() - 6, 6, 6, Qt::black );
    }

    _painter->setRasterOp( rop );
    _painter->restore();
}

/*======================== do delete =============================*/
void KPObject::doDelete()
{
    if ( cmds == 0 && !inObjList )delete this;
}

/*=============================================================*/
DCOPObject* KPObject::dcopObject()
{
    if ( !dcop )
        dcop = new KPresenterObjectIface( this );

    return dcop;
}

void KPObject::setupClipRegion( QPainter *painter, const QRegion &clipRegion )
{
    QRegion region = painter->clipRegion();
    if ( region.isEmpty() )
        region = clipRegion;
    else
        region.unite( clipRegion );

    painter->setClipRegion( region );
}

QDomElement KPObject::createValueElement(const QString &tag, int value, QDomDocument &doc) {
    QDomElement elem=doc.createElement(tag);
    elem.setAttribute("value", value);
    return elem;
}

QDomElement KPObject::createGradientElement(const QString &tag, const QColor &c1, const QColor &c2,
                                            int type, bool unbalanced, int xfactor, int yfactor, QDomDocument &doc) {
    QDomElement elem=doc.createElement(tag);
    elem.setAttribute("red1", c1.red());
    elem.setAttribute("green1", c1.green());
    elem.setAttribute("blue1", c1.blue());
    elem.setAttribute("red2", c2.red());
    elem.setAttribute("green2", c2.green());
    elem.setAttribute("blue2", c2.blue());
    elem.setAttribute("type", type);
    elem.setAttribute("unbalanced", (uint)unbalanced);
    elem.setAttribute("xfactor", xfactor);
    elem.setAttribute("yfactor", yfactor);
    return elem;
}

void KPObject::toGradient(const QDomElement &element, QColor &c1, QColor &c2, BCType &type,
                          bool &unbalanced, int &xfactor, int &yfactor) const {
    int red=0, green=0, blue=0, t=0;
    if(element.hasAttribute("red1"))
        red=element.attribute("red1").toInt();
    if(element.hasAttribute("green1"))
        green=element.attribute("green1").toInt();
    if(element.hasAttribute("blue1"))
        blue=element.attribute("blue1").toInt();
    c1.setRgb(red, green, blue);
    red=green=blue=0;
    if(element.hasAttribute("red2"))
        red=element.attribute("red2").toInt();
    if(element.hasAttribute("green2"))
        green=element.attribute("green2").toInt();
    if(element.hasAttribute("blue2"))
        blue=element.attribute("blue2").toInt();
    c2.setRgb(red, green, blue);
    if(element.hasAttribute("type"))
        t=element.attribute("type").toInt();
    type=static_cast<BCType>(t);
    if(element.hasAttribute("unbalanced"))
        unbalanced=static_cast<bool>(element.attribute("unbalanced").toInt());
    if(element.hasAttribute("xfactor"))
        xfactor=element.attribute("xfactor").toInt();
    if(element.hasAttribute("yfactor"))
        yfactor=element.attribute("yfactor").toInt();
}

QDomElement KPObject::createPenElement(const QString &tag, const QPen &pen, QDomDocument &doc) {

    QDomElement elem=doc.createElement(tag);
    elem.setAttribute("red", pen.color().red());
    elem.setAttribute("green", pen.color().green());
    elem.setAttribute("blue", pen.color().blue());
    elem.setAttribute("width", pen.width());
    elem.setAttribute("style", static_cast<int>(pen.style()));
    return elem;
}

QPen KPObject::toPen(const QDomElement &element) const {

    QPen pen;
    int red=0, green=0, blue=0;

    if(element.hasAttribute("red"))
        red=element.attribute("red").toInt();
    if(element.hasAttribute("green"))
        green=element.attribute("green").toInt();
    if(element.hasAttribute("blue"))
        blue=element.attribute("blue").toInt();
    pen.setColor(QColor(red, green, blue));
    if(element.hasAttribute("style"))
        pen.setStyle(static_cast<Qt::PenStyle>(element.attribute("style").toInt()));
    if(element.hasAttribute("width"))
        pen.setWidth(element.attribute("width").toInt());
    return pen;
}


QDomElement KPObject::createBrushElement(const QString &tag, const QBrush &brush, QDomDocument &doc) {

    QDomElement elem=doc.createElement(tag);
    elem.setAttribute("red", brush.color().red());
    elem.setAttribute("green", brush.color().green());
    elem.setAttribute("blue", brush.color().blue());
    elem.setAttribute("style", static_cast<int>(brush.style()));
    return elem;
}

QBrush KPObject::toBrush(const QDomElement &element) const {

    QBrush brush;
    int red=0, green=0, blue=0;

    if(element.hasAttribute("red"))
        red=element.attribute("red").toInt();
    if(element.hasAttribute("green"))
        green=element.attribute("green").toInt();
    if(element.hasAttribute("blue"))
        blue=element.attribute("blue").toInt();
    brush.setColor(QColor(red, green, blue));
    if(element.hasAttribute("style"))
        brush.setStyle(static_cast<Qt::BrushStyle>(element.attribute("style").toInt()));
    return brush;
}
