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

#include "kppieobject.h"
#include "kpgradient.h"
#include "KPPieObjectIface.h"

#include <qregion.h>
#include <qpicture.h>
#include <qdom.h>
#include <qpainter.h>
#include <kdebug.h>
#include <kozoomhandler.h>
using namespace std;

/******************************************************************/
/* Class: KPPieObject                                             */
/******************************************************************/

/*================ default constructor ===========================*/
KPPieObject::KPPieObject()
    : KP2DObject()
{
    pieType = PT_PIE;
    p_angle = 720; //45 * 16
    p_len = 1440; //90 * 16
    lineBegin = L_NORMAL;
    lineEnd = L_NORMAL;
}

/*================== overloaded constructor ======================*/
KPPieObject::KPPieObject( const QPen &_pen, const QBrush &_brush, FillType _fillType,
                          const QColor &_gColor1, const QColor &_gColor2, BCType _gType,
                          PieType _pieType, int _p_angle, int _p_len,
                          LineEnd _lineBegin, LineEnd _lineEnd,
                          bool _unbalanced, int _xfactor, int _yfactor )
    : KP2DObject( _pen, _brush, _fillType, _gColor1, _gColor2, _gType, _unbalanced, _xfactor, _yfactor )
{
    pieType = _pieType;
    p_angle = _p_angle;
    p_len = _p_len;
    lineBegin = _lineBegin;
    lineEnd = _lineEnd;
}

DCOPObject* KPPieObject::dcopObject()
{
    if ( !dcop )
        dcop = new KPPieObjectIface( this );
    return dcop;
}


/*================================================================*/
KPPieObject &KPPieObject::operator=( const KPPieObject & )
{
    return *this;
}

/*========================= save =================================*/
QDomDocumentFragment KPPieObject::save( QDomDocument& doc, double offset )
{
  QDomDocumentFragment fragment=KP2DObject::save(doc, offset);
  if (lineBegin!=L_NORMAL)
    fragment.appendChild(KPObject::createValueElement("LINEBEGIN", static_cast<int>(lineBegin), doc));
  if (lineEnd!=L_NORMAL)
    fragment.appendChild(KPObject::createValueElement("LINEEND", static_cast<int>(lineEnd), doc));
  if (p_angle!=720)
    fragment.appendChild(KPObject::createValueElement("PIEANGLE", p_angle, doc));
  if (p_len!=1440)
    fragment.appendChild(KPObject::createValueElement("PIELENGTH", p_len, doc));
  if (pieType!=PT_PIE)
    fragment.appendChild(KPObject::createValueElement("PIETYPE", static_cast<int>(pieType), doc));
  return fragment;
}

/*========================== load ================================*/
double KPPieObject::load(const QDomElement &element)
{
    double offset=KP2DObject::load(element);
    QDomElement e=element.namedItem("LINEBEGIN").toElement();
    if(!e.isNull()) {
        int tmp=0;
        if(e.hasAttribute("value"))
            tmp=e.attribute("value").toInt();
        lineBegin=static_cast<LineEnd>(tmp);
    }
    e=element.namedItem("LINEEND").toElement();
    if(!e.isNull()) {
        int tmp=0;
        if(e.hasAttribute("value"))
            tmp=e.attribute("value").toInt();
        lineEnd=static_cast<LineEnd>(tmp);
    }
    e=element.namedItem("PIEANGLE").toElement();
    if(!e.isNull()) {
        int tmp=0;
        if(e.hasAttribute("value"))
            tmp=e.attribute("value").toInt();
        p_angle=tmp;
    }
    e=element.namedItem("PIELENGTH").toElement();
    if(!e.isNull()) {
        int tmp=0;
        if(e.hasAttribute("value"))
            tmp=e.attribute("value").toInt();
        p_len=tmp;
    }
    e=element.namedItem("PIETYPE").toElement();
    if(!e.isNull()) {
        int tmp=0;
        if(e.hasAttribute("value"))
            tmp=e.attribute("value").toInt();
        pieType=static_cast<PieType>(tmp);
    }
    return offset;
}

/*======================== paint =================================*/
void KPPieObject::paint( QPainter* _painter, KoZoomHandler*_zoomHandler,
			 bool /*drawingShadow*/, bool drawContour )
{
    double ow = ext.width();
    double oh = ext.height();
    double pw = ( ( pen.style() == Qt::NoPen ) ? 1 : pen.width() ) / 2.0;

    if ( drawContour ) {
        QPen pen3( Qt::black, 1, Qt::DotLine );
        _painter->setPen( pen3 );
        _painter->setRasterOp( Qt::NotXorROP );
    }
    else {
        QPen pen2( pen );
        pen2.setWidth(_zoomHandler->zoomItX( pen2.width()));
        _painter->setPen( pen2 );
        _painter->setBrush( brush );
    }
    switch ( pieType )
    {
    case PT_PIE:
        _painter->drawPie( _zoomHandler->zoomItX(pw), _zoomHandler->zoomItY( pw),
                           _zoomHandler->zoomItX( ow - 2 * pw),
                           _zoomHandler->zoomItY( oh - 2 * pw), p_angle, p_len );
        break;
    case PT_ARC:
        _painter->drawArc( _zoomHandler->zoomItX(pw), _zoomHandler->zoomItY(pw),
                           _zoomHandler->zoomItX(ow - 2 * pw),
                           _zoomHandler->zoomItY(oh - 2 * pw), p_angle, p_len );
        break;
    case PT_CHORD:
        _painter->drawChord( _zoomHandler->zoomItX(pw), _zoomHandler->zoomItY(pw),
                             _zoomHandler->zoomItX(ow - 2 * pw),
                             _zoomHandler->zoomItY(oh - 2 * pw), p_angle, p_len );
        break;
    default: break;
    }
}

void KPPieObject::flip(bool horizontal )
{
    if ( horizontal )
    {
        if ( p_angle <= 90*16 )
        {
            p_angle = (360*16 - p_angle -p_len);
        }
        else if ( p_angle >90*16 && p_angle <180*16 )
        {
            p_angle = (p_angle + 90*16 + p_len) ;
        }
        else if ( p_angle >180*16 && p_angle <270*16 )
        {
            p_angle = (360*16 - (p_angle + p_len))%(360*16);
        }
        else if ( p_angle >270*16 && p_angle <360*16 )
        {
            p_angle = (360*16-p_angle -p_len)%(360*16);
        }
    }
    else
    {
        if ( p_angle <= 90*16 )
        {
            p_angle = 180*16- p_angle - p_len;
        }
        else if ( p_angle >90*16 && p_angle <180*16 )
        {
            p_angle = 180*16 - p_angle - p_len  ;
        }
        else if ( p_angle >180*16 && p_angle <270*16 )
        {
            p_angle = 360*16 - (p_angle - 180*16) - p_len;
        }
        else if ( p_angle >270*16 && p_angle <360*16 )
        {
            p_angle = 180*16+ (360*16 - (p_angle+p_len));
        }
    }
}
