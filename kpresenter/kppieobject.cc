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

#include <kppieobject.h>
#include <kpgradient.h>

#include <qregion.h>
#include <qpicture.h>
#include <qdom.h>
#include <qpainter.h>
#include <qwmatrix.h>
#include <kdebug.h>
using namespace std;

/******************************************************************/
/* Class: KPPieObject                                             */
/******************************************************************/

/*================ default constructor ===========================*/
KPPieObject::KPPieObject()
    : KP2DObject()
{
    pieType = PT_PIE;
    p_angle = 45 * 16;
    p_len = 90 * 16;
    lineBegin = L_NORMAL;
    lineEnd = L_NORMAL;
}

/*================== overloaded constructor ======================*/
KPPieObject::KPPieObject( QPen _pen, QBrush _brush, FillType _fillType,
                          QColor _gColor1, QColor _gColor2, BCType _gType,
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

/*================================================================*/
KPPieObject &KPPieObject::operator=( const KPPieObject & )
{
    return *this;
}

/*========================= save =================================*/
QDomDocumentFragment KPPieObject::save( QDomDocument& doc )
{
    QDomDocumentFragment fragment=KP2DObject::save(doc);
    fragment.appendChild(KPObject::createValueElement("LINEBEGIN", static_cast<int>(lineBegin), doc));
    fragment.appendChild(KPObject::createValueElement("LINEEND", static_cast<int>(lineEnd), doc));
    fragment.appendChild(KPObject::createValueElement("PIEANGLE", p_angle, doc));
    fragment.appendChild(KPObject::createValueElement("PIELENGTH", p_len, doc));
    fragment.appendChild(KPObject::createValueElement("PIETYPE", static_cast<int>(pieType), doc));
    return fragment;
}

/*========================== load ================================*/
void KPPieObject::load(const QDomElement &element)
{
    KP2DObject::load(element);
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
}

/*======================== paint =================================*/
void KPPieObject::paint( QPainter* _painter )
{
    int ow = ext.width();
    int oh = ext.height();

    _painter->setPen( pen );
    int pw = pen.width() / 2;
    _painter->setBrush( brush );

    switch ( pieType )
    {
    case PT_PIE:
        _painter->drawPie( pw, pw, ow - 2 * pw, oh - 2 * pw, p_angle, p_len );
        break;
    case PT_ARC:
        _painter->drawArc( pw, pw, ow - 2 * pw, oh - 2 * pw, p_angle, p_len );
        break;
    case PT_CHORD:
        _painter->drawChord( pw, pw, ow - 2 * pw, oh - 2 * pw, p_angle, p_len );
        break;
    default: break;
    }
}
