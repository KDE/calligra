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

#ifndef kppieobject_h
#define kppieobject_h

#include <kpobject.h>

class KPGradient;

/******************************************************************/
/* Class: KPPieObject                                             */
/******************************************************************/

class KPPieObject : public KP2DObject
{
public:
    KPPieObject();
    KPPieObject( QPen _pen, QBrush _brush, FillType _fillType,
                 QColor _gColor1, QColor _gColor2, BCType _gType, PieType _pieType,
                 int _p_angle, int _p_len, LineEnd _lineBegin, LineEnd _lineEnd,
                 bool _unbalanced, int _xfactor, int _yfactor );
    virtual ~KPPieObject() {}

    KPPieObject &operator=( const KPPieObject & );

    virtual void setPieType( PieType _pieType )
    { pieType = _pieType; }
    virtual void setPieAngle( int _p_angle )
    { p_angle = _p_angle; }
    virtual void setPieLength( int _p_len )
    { p_len = _p_len; }
    virtual void setLineBegin( LineEnd _lineBegin )
    { lineBegin = _lineBegin; }
    virtual void setLineEnd( LineEnd _lineEnd )
    { lineEnd = _lineEnd; }

    virtual ObjType getType() const
    { return OT_PIE; }
    virtual PieType getPieType() const
    { return pieType; }
    virtual int getPieAngle() const
    { return p_angle; }
    virtual int getPieLength() const
    { return p_len; }
    virtual LineEnd getLineBegin() const
    { return lineBegin; }
    virtual LineEnd getLineEnd() const
    { return lineEnd; }

    virtual QDomDocumentFragment save( QDomDocument& doc );
    virtual void load(const QDomElement &element);

protected:
    virtual void paint( QPainter *_painter );

    PieType pieType;
    int p_angle, p_len;
    LineEnd lineBegin, lineEnd;
};

#endif
