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

#ifndef kprectobject_h
#define kprectobject_h

#include <kpobject.h>

class KPGradient;

/******************************************************************/
/* Class: KPRectObject                                            */
/******************************************************************/

class KPRectObject : public KP2DObject
{
public:
    KPRectObject();
    KPRectObject( QPen _pen, QBrush _brush, FillType _fillType,
                  QColor _gColor1, QColor _gColor2, BCType _gType, int _xRnd, int _yRnd,
                  bool _unbalanced, int _xfactor, int _yfactor );
    virtual ~KPRectObject() {}

    KPRectObject &operator=( const KPRectObject & );

    virtual void setRnds( int _xRnd, int _yRnd )
    { xRnd = _xRnd; yRnd = _yRnd; }

    virtual ObjType getType() const
    { return OT_RECT; }
    virtual void getRnds( int &_xRnd, int &_yRnd ) const
    { _xRnd = xRnd; _yRnd = yRnd; }

    virtual QDomDocumentFragment save( QDomDocument& doc );
    virtual void load(const QDomElement &element);

protected:
    virtual void paint( QPainter *_painter );

    int xRnd, yRnd;
};

#endif
