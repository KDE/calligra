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

const QString &KPObject::tagORIG=KGlobal::staticQString("ORIG");
const QString &KPObject::attrX=KGlobal::staticQString("x");
const QString &KPObject::attrY=KGlobal::staticQString("y");
const QString &KPObject::tagSIZE=KGlobal::staticQString("SIZE");
const QString &KPObject::attrWidth=KGlobal::staticQString("width");
const QString &KPObject::attrHeight=KGlobal::staticQString("height");
const QString &KPObject::tagSHADOW=KGlobal::staticQString("SHADOW");
const QString &KPObject::attrDistance=KGlobal::staticQString("distance");
const QString &KPObject::attrDirection=KGlobal::staticQString("direction");
const QString &KPObject::attrColor=KGlobal::staticQString("color");
const QString &KPObject::tagEFFECTS=KGlobal::staticQString("EFFECTS");
const QString &KPObject::attrEffect=KGlobal::staticQString("effect");
const QString &KPObject::attrEffect2=KGlobal::staticQString("effect2");
const QString &KPObject::tagPRESNUM=KGlobal::staticQString("PRESNUM");
const QString &KPObject::tagANGLE=KGlobal::staticQString("ANGLE");
const QString &KPObject::tagDISAPPEAR=KGlobal::staticQString("DISAPPEAR");
const QString &KPObject::attrDoit=KGlobal::staticQString("doit");
const QString &KPObject::attrNum=KGlobal::staticQString("num");
const QString &KPObject::tagFILLTYPE=KGlobal::staticQString("FILLTYPE");
const QString &KPObject::tagGRADIENT=KGlobal::staticQString("GRADIENT");
const QString &KPObject::tagPEN=KGlobal::staticQString("PEN");
const QString &KPObject::tagBRUSH=KGlobal::staticQString("BRUSH");
const QString &KPObject::attrValue=KGlobal::staticQString("value");
const QString &KPObject::attrC1=KGlobal::staticQString("color1");
const QString &KPObject::attrC2=KGlobal::staticQString("color2");
const QString &KPObject::attrType=KGlobal::staticQString("type");
const QString &KPObject::attrUnbalanced=KGlobal::staticQString("unbalanced");
const QString &KPObject::attrXFactor=KGlobal::staticQString("xfactor");
const QString &KPObject::attrYFactor=KGlobal::staticQString("yfactor");
const QString &KPObject::attrStyle=KGlobal::staticQString("style");

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
    QDomElement elem=doc.createElement(tagORIG);
    elem.setAttribute(attrX, orig.x());
    elem.setAttribute(attrY, orig.y());
    fragment.appendChild(elem);
    elem=doc.createElement(tagSIZE);
    elem.setAttribute(attrWidth, ext.width());
    elem.setAttribute(attrHeight, ext.height());
    fragment.appendChild(elem);
    if(shadowDistance!=0 || shadowDirection!=SD_RIGHT_BOTTOM) { // no, we don't check the color :)
        elem=doc.createElement(tagSHADOW);
        elem.setAttribute(attrDistance, shadowDistance);
        elem.setAttribute(attrDirection, static_cast<int>( shadowDirection ));
        elem.setAttribute(attrColor, shadowColor.name());
        fragment.appendChild(elem);
    }
    if(effect!=EF_NONE || effect2!=EF2_NONE) {
        elem=doc.createElement(tagEFFECTS);
        elem.setAttribute(attrEffect, static_cast<int>( effect ));
        elem.setAttribute(attrEffect2, static_cast<int>( effect2 ));
        fragment.appendChild(elem);
    }
    if(presNum!=0)
        fragment.appendChild(KPObject::createValueElement(tagPRESNUM, presNum, doc));
    if(angle!=0.0) {
        elem=doc.createElement(tagANGLE);
        elem.setAttribute(attrValue, angle);
        fragment.appendChild(elem);
    }
    if(effect3!=EF3_NONE || disappear) {
        elem=doc.createElement(tagDISAPPEAR);
        elem.setAttribute(attrEffect, static_cast<int>( effect3 ));
        elem.setAttribute(attrDoit, static_cast<int>( disappear ));
        elem.setAttribute(attrNum, disappearNum);
        fragment.appendChild(elem);
    }
    return fragment;
}

void KPObject::load(const QDomElement &element) {

    QDomElement e=element.namedItem(tagORIG).toElement();
    if(!e.isNull()) {
        if(e.hasAttribute(attrX))
            orig.setX(e.attribute(attrX).toInt());
        if(e.hasAttribute(attrY))
            orig.setY(e.attribute(attrY).toInt());
    }
    e=element.namedItem(tagSIZE).toElement();
    if(!e.isNull()) {
        if(e.hasAttribute(attrWidth))
            ext.setWidth(e.attribute(attrWidth).toInt());
        if(e.hasAttribute(attrHeight))
            ext.setHeight(e.attribute(attrHeight).toInt());
    }
    e=element.namedItem(tagSHADOW).toElement();
    if(!e.isNull()) {
        if(e.hasAttribute(attrDistance))
            shadowDistance=e.attribute(attrDistance).toInt();
        if(e.hasAttribute(attrDirection))
            shadowDirection=static_cast<ShadowDirection>(e.attribute(attrDirection).toInt());
        shadowColor=retrieveColor(e);
    }
    else {
        shadowDistance=0;
        shadowDirection=SD_RIGHT_BOTTOM;
        shadowColor=Qt::gray;
    }
    e=element.namedItem(tagEFFECTS).toElement();
    if(!e.isNull()) {
        if(e.hasAttribute(attrEffect))
            effect=static_cast<Effect>(e.attribute(attrEffect).toInt());
        if(e.hasAttribute(attrEffect2))
            effect2=static_cast<Effect2>(e.attribute(attrEffect2).toInt());
    }
    else {
        effect=EF_NONE;
        effect2=EF2_NONE;
    }
    e=element.namedItem(tagANGLE).toElement();
    if(!e.isNull()) {
        if(e.hasAttribute(attrValue))
            angle=e.attribute(attrValue).toFloat();
    }
    else
        angle=0.0;
    e=element.namedItem(tagPRESNUM).toElement();
    if(!e.isNull()) {
        if(e.hasAttribute(attrValue))
            presNum=e.attribute(attrValue).toInt();
    }
    else
        presNum=0;
    e=element.namedItem(tagDISAPPEAR).toElement();
    if(!e.isNull()) {
        if(e.hasAttribute(attrEffect))
            effect3=static_cast<Effect3>(e.attribute(attrEffect).toInt());
        if(e.hasAttribute(attrDoit))
            disappear=static_cast<bool>(e.attribute(attrDoit).toInt());
        if(e.hasAttribute(attrNum))
            disappearNum=e.attribute(attrNum).toInt();
    }
    else {
        effect3=EF3_NONE;
        disappear=false;
        disappearNum=1;
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

        QWMatrix m;
        m.translate( pw / 2, ph / 2 );
        m.rotate( angle );
        m.translate( rr.left() + xPos, rr.top() + yPos );

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

        QWMatrix m;
        m.translate( pw / 2, ph / 2 );
        m.rotate( angle );
        m.translate( rr.left() + xPos, rr.top() + yPos );

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

            QWMatrix m;
            m.translate( pw / 2, ph / 2 );
            m.rotate( angle );
            m.translate( rr.left() + xPos, rr.top() + yPos );

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
    elem.setAttribute(attrValue, value);
    return elem;
}

QDomElement KPObject::createGradientElement(const QString &tag, const QColor &c1, const QColor &c2,
                                            int type, bool unbalanced, int xfactor, int yfactor, QDomDocument &doc) {
    QDomElement elem=doc.createElement(tag);
    elem.setAttribute(attrC1, c1.name());
    elem.setAttribute(attrC2, c2.name());
    elem.setAttribute(attrType, type);
    elem.setAttribute(attrUnbalanced, (uint)unbalanced);
    elem.setAttribute(attrXFactor, xfactor);
    elem.setAttribute(attrYFactor, yfactor);
    return elem;
}

void KPObject::toGradient(const QDomElement &element, QColor &c1, QColor &c2, BCType &type,
                          bool &unbalanced, int &xfactor, int &yfactor) const {
    c1=retrieveColor(element, attrC1, "red1", "green1", "blue1");
    c2=retrieveColor(element, attrC2, "red2", "green2", "blue2");
    if(element.hasAttribute(attrType))
        type=static_cast<BCType>(element.attribute(attrType).toInt());
    if(element.hasAttribute(attrUnbalanced))
        unbalanced=static_cast<bool>(element.attribute(attrUnbalanced).toInt());
    if(element.hasAttribute(attrXFactor))
        xfactor=element.attribute(attrXFactor).toInt();
    if(element.hasAttribute(attrYFactor))
        yfactor=element.attribute(attrYFactor).toInt();
}

QDomElement KPObject::createPenElement(const QString &tag, const QPen &pen, QDomDocument &doc) {

    QDomElement elem=doc.createElement(tag);
    elem.setAttribute(attrColor, pen.color().name());
    elem.setAttribute(attrWidth, pen.width());
    elem.setAttribute(attrStyle, static_cast<int>(pen.style()));
    return elem;
}

QPen KPObject::toPen(const QDomElement &element) const {

    QPen pen;
    pen.setColor(retrieveColor(element));
    if(element.hasAttribute(attrStyle))
        pen.setStyle(static_cast<Qt::PenStyle>(element.attribute(attrStyle).toInt()));
    if(element.hasAttribute(attrWidth))
        pen.setWidth(element.attribute(attrWidth).toInt());
    return pen;
}

QDomElement KPObject::createBrushElement(const QString &tag, const QBrush &brush, QDomDocument &doc) {

    QDomElement elem=doc.createElement(tag);
    elem.setAttribute(attrColor, brush.color().name());
    elem.setAttribute(attrStyle, static_cast<int>(brush.style()));
    return elem;
}

QBrush KPObject::toBrush(const QDomElement &element) const {

    QBrush brush;
    brush.setColor(retrieveColor(element));
    if(element.hasAttribute(attrStyle))
        brush.setStyle(static_cast<Qt::BrushStyle>(element.attribute(attrStyle).toInt()));
    return brush;
}

QColor KPObject::retrieveColor(const QDomElement &element, const QString &cattr,
                               const QString &rattr, const QString &gattr, const QString &battr) const {
    QColor ret;
    if(element.hasAttribute(cattr))
        ret.setNamedColor(element.attribute(cattr));
    else {
        int red=0, green=0, blue=0;
        if(element.hasAttribute(rattr))
            red=element.attribute(rattr).toInt();
        if(element.hasAttribute(gattr))
            green=element.attribute(gattr).toInt();
        if(element.hasAttribute(battr))
            blue=element.attribute(battr).toInt();
        ret.setRgb(red, green, blue);
    }
    return ret;
}

KP2DObject::KP2DObject()
    : KPObject(), pen(), brush(), gColor1( Qt::red ), gColor2( Qt::green )
{
    gradient = 0;
    fillType = FT_BRUSH;
    gType = BCT_GHORZ;
    drawShadow = false;
    unbalanced = false;
    xfactor = 100;
    yfactor = 100;
}

KP2DObject::KP2DObject( QPen _pen, QBrush _brush, FillType _fillType,
                        QColor _gColor1, QColor _gColor2, BCType _gType,
                        bool _unbalanced, int _xfactor, int _yfactor )
    : KPObject(), pen( _pen ), brush( _brush ), gColor1( _gColor1 ), gColor2( _gColor2 )
{
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

KP2DObject &KP2DObject::operator=( const KP2DObject & )
{
    return *this;
}

void KP2DObject::setSize( int _width, int _height )
{
    KPObject::setSize( _width, _height );
    if ( move ) return;

    if ( fillType == FT_GRADIENT && gradient )
        gradient->setSize( getSize() );
}

void KP2DObject::resizeBy( int _dx, int _dy )
{
    KPObject::resizeBy( _dx, _dy );
    if ( move ) return;

    if ( fillType == FT_GRADIENT && gradient )
        gradient->setSize( getSize() );
}

void KP2DObject::setFillType( FillType _fillType )
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

QDomDocumentFragment KP2DObject::save( QDomDocument& doc )
{
    QDomDocumentFragment fragment=KPObject::save(doc);
    if(fillType!=FT_BRUSH)
        fragment.appendChild(KPObject::createValueElement(tagFILLTYPE, static_cast<int>(fillType), doc));
    if(gColor1!=Qt::red || gColor2!=Qt::green || gType!=BCT_GHORZ || unbalanced || xfactor!=100 || yfactor!=100)
        fragment.appendChild(KPObject::createGradientElement(tagGRADIENT, gColor1, gColor2, static_cast<int>(gType),
                                                             unbalanced, xfactor, yfactor, doc));
    if(pen.color()!=Qt::black || pen.width()!=1 || pen.style()!=Qt::SolidLine)
        fragment.appendChild(KPObject::createPenElement(tagPEN, pen, doc));
    if(brush.color()!=Qt::black || brush.style()!=Qt::NoBrush)
        fragment.appendChild(KPObject::createBrushElement(tagBRUSH, brush, doc));
    return fragment;
}

void KP2DObject::load(const QDomElement &element)
{
    KPObject::load(element);
    QDomElement e=element.namedItem(tagPEN).toElement();
    if(!e.isNull())
        setPen(KPObject::toPen(e));
    else
        pen=QPen();
    e=element.namedItem(tagBRUSH).toElement();
    if(!e.isNull())
        setBrush(KPObject::toBrush(e));
    else
        brush=QBrush();
    e=element.namedItem(tagFILLTYPE).toElement();
    if(!e.isNull()) {
        if(e.hasAttribute(attrValue))
            setFillType(static_cast<FillType>(e.attribute(attrValue).toInt()));
    }
    else
        setFillType(FT_BRUSH);
    e=element.namedItem(tagGRADIENT).toElement();
    if(!e.isNull()) {
        KPObject::toGradient(e, gColor1, gColor2, gType, unbalanced, xfactor, yfactor);
        if(gradient)
            gradient->init(gColor1, gColor2, gType, unbalanced, xfactor, yfactor);
    }
    else {
        gColor1=Qt::red;
        gColor2=Qt::green;
        gType=BCT_GHORZ;
        unbalanced=false;
        xfactor=100;
        yfactor=100;
    }
}

void KP2DObject::draw( QPainter *_painter, int _diffx, int _diffy )
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

            QWMatrix m;
            m.translate( pw / 2, ph / 2 );
            m.rotate( angle );
            m.translate( rr.left() + xPos + sx, rr.top() + yPos + sy );

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

        QWMatrix m;
        m.translate( pw / 2, ph / 2 );
        m.rotate( angle );
        m.translate( rr.left() + xPos, rr.top() + yPos );

        _painter->setWorldMatrix( m, true );
        paint( _painter );
    }

    _painter->restore();

    KPObject::draw( _painter, _diffx, _diffy );
}
