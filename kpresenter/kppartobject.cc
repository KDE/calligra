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

#include <kppartobject.h>
#include <kpresenter_doc.h>
#include <kpresenter_view.h>
#include <kpgradient.h>

#include <qpicture.h>
#include <qpainter.h>
#include <kdebug.h>
using namespace std;

/******************************************************************/
/* Class: KPPartObject                                            */
/******************************************************************/

/*======================== constructor ===========================*/
KPPartObject::KPPartObject( KPresenterChild *_child )
    : KP2DObject()
{
    child = _child;
    brush = Qt::NoBrush;
    pen = QPen( Qt::black, 1, Qt::NoPen );
    _enableDrawing = true;
}

/*================================================================*/
KPPartObject &KPPartObject::operator=( const KPPartObject & )
{
    return *this;
}

/*================================================================*/
void KPPartObject::rotate( float _angle )
{
    KPObject::rotate( _angle );

    child->setRotation( _angle );
    child->setRotationPoint( QPoint( getOrig().x() + getSize().width() / 2,
                             getOrig().y() + getSize().height() / 2 ) );
    if ( fillType == FT_GRADIENT && gradient )
        gradient->setSize( getSize() );
}

/*======================== draw ==================================*/
void KPPartObject::setSize( int _width, int _height )
{
    KPObject::setSize( _width, _height );
    if ( move ) return;

    child->setGeometry( QRect( orig, ext ) );
    child->setRotationPoint( QPoint( getOrig().x() + getSize().width() / 2,
                                     getOrig().y() + getSize().height() / 2 ) );
    if ( fillType == FT_GRADIENT && gradient )
        gradient->setSize( getSize() );
}

/*======================== draw ==================================*/
void KPPartObject::setOrig( QPoint _point )
{
    KPObject::setOrig( _point );

    child->setGeometry( QRect( orig, ext ) );
    child->setRotationPoint( QPoint( getOrig().x() + getSize().width() / 2,
                                     getOrig().y() + getSize().height() / 2 ) );
}

/*======================== draw ==================================*/
void KPPartObject::setOrig( int _x, int _y )
{
    KPObject::setOrig( _x, _y );

    child->setGeometry( QRect( orig, ext ) );
    child->setRotationPoint( QPoint( getOrig().x() + getSize().width() / 2,
                                     getOrig().y() + getSize().height() / 2 ) );

}

/*======================== draw ==================================*/
void KPPartObject::moveBy( QPoint _point )
{
    KPObject::moveBy( _point );

    child->setGeometry( QRect( orig, ext ) );
    child->setRotationPoint( QPoint( getOrig().x() + getSize().width() / 2,
                                     getOrig().y() + getSize().height() / 2 ) );
}

/*======================== draw ==================================*/
void KPPartObject::moveBy( int _dx, int _dy )
{
    KPObject::moveBy( _dx, _dy );

    child->setGeometry( QRect( orig, ext ) );
    child->setRotationPoint( QPoint( getOrig().x() + getSize().width() / 2,
                                     getOrig().y() + getSize().height() / 2 ) );
}

/*======================== draw ==================================*/
void KPPartObject::resizeBy( int _dx, int _dy )
{
    KPObject::resizeBy( _dx, _dy );
    if ( move ) return;

    child->setGeometry( QRect( orig, ext ) );
    child->setRotationPoint( QPoint( getOrig().x() + getSize().width() / 2,
                                     getOrig().y() + getSize().height() / 2 ) );
    if ( fillType == FT_GRADIENT && gradient )
        gradient->setSize( getSize() );
}

/*======================== draw ==================================*/
void KPPartObject::draw( QPainter *_painter, int _diffx, int _diffy )
{
    if ( move )
    {
        KPObject::draw( _painter, _diffx, _diffy );
        return;
    }

//     int ox = orig.x() - _diffx;
//     int oy = orig.y() - _diffy;
    int ow = ext.width();
    int oh = ext.height();

    int penw = pen.width() / 2;

    _painter->save();
    _painter->translate( -_diffx, -_diffy );

    if ( angle == 0 )
    {
        child->transform( *_painter );

        _painter->setPen( Qt::NoPen );
        _painter->setBrush( brush );
        if ( fillType == FT_BRUSH || !gradient )
            _painter->drawRect( penw, penw, ext.width() - 2 * penw, ext.height() - 2 * penw );
        else
            _painter->drawPixmap( penw, penw, *gradient->getGradient(), 0, 0, ow - 2 * penw, oh - 2 * penw );

        _painter->setPen( pen );
        _painter->setBrush( Qt::NoBrush );
        _painter->drawRect( penw, penw, ow - 2 * penw, oh - 2 * penw );

        paint( _painter );
    }
    else
    {
        child->transform( *_painter );

        /* QRect br = QRect( 0, 0, ow, oh );
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

        _painter->setWorldMatrix( m, true ); */

        _painter->setPen( Qt::NoPen );
        _painter->setBrush( brush );

        if ( fillType == FT_BRUSH || !gradient )
            _painter->drawRect(penw, penw, ext.width() - 2 * penw, ext.height() - 2 * penw );
        else
            _painter->drawPixmap( penw, penw, *gradient->getGradient(), 0, 0, ow - 2 * penw, oh - 2 * penw );

        _painter->setPen( pen );
        _painter->setBrush( Qt::NoBrush );
        _painter->drawRect( penw, penw, ow - 2 * penw, oh - 2 * penw );

        paint( _painter );
    }

    _painter->restore();

    KPObject::draw( _painter, _diffx, _diffy );
}

/*================================================================*/
void KPPartObject::slot_changed(KoChild *child)
{
    QRect g = child->geometry();
    KPObject::setOrig( g.x(), g.y() );
    KPObject::setSize( g.width(), g.height() );
    if ( fillType == FT_GRADIENT && gradient )
        gradient->setSize( g.size() );
}

/*================================================================*/
void KPPartObject::paint( QPainter *_painter )
{
    if ( !_enableDrawing ) return;

    // ######### Torben: Care about zooming
    if ( child && child->document() )
        child->document()->paintEverything( *_painter, QRect( QPoint( 0, 0 ), getSize() ), true, 0 );
}

/*================================================================*/
void KPPartObject::activate( QWidget *_widget, int /*diffx*/, int /*diffy*/ )
{
    KPresenterView *view = (KPresenterView*)_widget;
    KoDocument* part = child->document();
    if ( !part )
        return;
    view->partManager()->addPart( part, false );
    view->partManager()->setActivePart( part, view );
}

/*================================================================*/
void KPPartObject::deactivate()
{
}

/*================================================================*/
QDomDocumentFragment KPPartObject::save( QDomDocument& doc )
{
    // ### FIXME
    QDomDocumentFragment fragment=doc.createDocumentFragment();
    QDomElement elem=doc.createElement("EFFECTS");
    elem.setAttribute("effect", static_cast<int>( effect ));
    elem.setAttribute("effect2", static_cast<int>( effect2 ));
    fragment.appendChild(elem);
    fragment.appendChild(KPObject::createPenElement("PEN", pen, doc));
    fragment.appendChild(KPObject::createBrushElement("BRUSH", brush, doc));
    fragment.appendChild(KPObject::createValueElement("PRESNUM", presNum, doc));
    elem=doc.createElement("ANGLE");
    elem.setAttribute("value", angle);
    fragment.appendChild(elem);
    fragment.appendChild(KPObject::createValueElement("FILLTYPE", static_cast<int>(fillType), doc));
    fragment.appendChild(KPObject::createGradientElement("GRADIENT", gColor1, gColor2, static_cast<int>(gType),
                                                         unbalanced, xfactor, yfactor, doc));
    elem=doc.createElement("DISAPPEAR");
    elem.setAttribute("effect", static_cast<int>( effect3 ));
    elem.setAttribute("doit", static_cast<int>( disappear ));
    elem.setAttribute("num", disappearNum);
    fragment.appendChild(elem);
    return fragment;
}

/*========================== load ================================*/
void KPPartObject::load(const QDomElement &element)
{
    KPObject::load(element);
    QDomElement e=element.namedItem("PEN").toElement();
    if(!e.isNull())
        setPen(KPObject::toPen(e));
    e=element.namedItem("BRUSH").toElement();
    if(!e.isNull())
        setBrush(KPObject::toBrush(e));
    e=element.namedItem("FILLTYPE").toElement();
    if(!e.isNull()) {
        int tmp=0;
        if(e.hasAttribute("value"))
            tmp=e.attribute("value").toInt();
        setFillType(static_cast<FillType>(tmp));
    }
    e=element.namedItem("GRADIENT").toElement();
    if(!e.isNull()) {
        KPObject::toGradient(e, gColor1, gColor2, gType, unbalanced, xfactor, yfactor);
        if(gradient)
            gradient->init(gColor1, gColor2, gType, unbalanced, xfactor, yfactor);
    }
}

#include <kppartobject.moc>
