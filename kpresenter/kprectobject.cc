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

#include <kprectobject.h>
#include <kpgradient.h>

#include <kdebug.h>
#include <qregion.h>
#include <qdom.h>
#include <qpainter.h>
#include <qwmatrix.h>
using namespace std;

/******************************************************************/
/* Class: KPRectObject                                            */
/******************************************************************/

/*================ default constructor ===========================*/
KPRectObject::KPRectObject()
    : KP2DObject()
{
    xRnd = 0;
    yRnd = 0;
}

/*================== overloaded constructor ======================*/
KPRectObject::KPRectObject( QPen _pen, QBrush _brush, FillType _fillType,
                            QColor _gColor1, QColor _gColor2, BCType _gType, int _xRnd, int _yRnd,
                            bool _unbalanced, int _xfactor, int _yfactor)
    : KP2DObject( _pen, _brush, _fillType, _gColor1, _gColor2, _gType,
                  _unbalanced, _xfactor, _yfactor )
{
    xRnd = _xRnd;
    yRnd = _yRnd;
}

/*================================================================*/
KPRectObject &KPRectObject::operator=( const KPRectObject & )
{
    return *this;
}

/*========================= save =================================*/
QDomDocumentFragment KPRectObject::save( QDomDocument& doc )
{
    QDomDocumentFragment fragment=KP2DObject::save(doc);
    QDomElement elem=doc.createElement("RNDS");
    elem.setAttribute("x", xRnd);
    elem.setAttribute("y", yRnd);
    fragment.appendChild(elem);
    return fragment;
}

/*========================== load ================================*/
void KPRectObject::load(const QDomElement &element)
{
    KP2DObject::load(element);
    QDomElement e=element.namedItem("RNDS").toElement();
    if(!e.isNull()) {
        int tmp=0;
        if(e.hasAttribute("x"))
            tmp=e.attribute("x").toInt();
        xRnd=tmp;
        tmp=0;
        if(e.hasAttribute("y"))
            tmp=e.attribute("y").toInt();
        yRnd=tmp;
    }
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
