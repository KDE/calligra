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

#ifndef kpautoformobject_h
#define kpautoformobject_h

#include <qpixmap.h>

#include <kpobject.h>
#include <global.h>
#include <autoformEdit/atfinterpreter.h>

#define RAD_FACTOR 180.0 / M_PI

class KPGradient;

/******************************************************************/
/* Class: KPAutoformObject                                        */
/******************************************************************/

class KPAutoformObject : public KP2DObject
{
public:
    KPAutoformObject();
    KPAutoformObject( QPen _pen, QBrush _brush, QString _filename, LineEnd _lineBegin, LineEnd _lineEnd,
                      FillType _fillType, QColor _gColor1, QColor _gColor2, BCType _gType,
                      bool _unbalanced, int _xfactor, int _yfactor);
    virtual ~KPAutoformObject() {}

    KPAutoformObject &operator=( const KPAutoformObject & );

    virtual void setSize( int _width, int _height );
    virtual void resizeBy( int _dx, int _dy );

    virtual void setFileName( QString _filename );
    virtual void setLineBegin( LineEnd _lineBegin )
    { lineBegin = _lineBegin; }
    virtual void setLineEnd( LineEnd _lineEnd )
    { lineEnd = _lineEnd; }
    virtual void setFillType( FillType _fillType );
    virtual void setGColor1( QColor _gColor1 )
    { KP2DObject::setGColor1(_gColor1); redrawPix = true; }
    virtual void setGColor2( QColor _gColor2 )
    { KP2DObject::setGColor2(_gColor2); redrawPix = true; }
    virtual void setGType( BCType _gType )
    { KP2DObject::setGType(_gType); redrawPix = true; }

    virtual ObjType getType() const
    { return OT_AUTOFORM; }
    virtual QString getFileName() const
    { return filename; }
    virtual LineEnd getLineBegin() const
    { return lineBegin; }
    virtual LineEnd getLineEnd() const
    { return lineEnd; }

    virtual QDomDocumentFragment save( QDomDocument& doc );
    virtual void load(const QDomElement &element);

protected:
    float getAngle( QPoint p1, QPoint p2 );
    virtual void paint( QPainter *_painter );

    QString filename;
    LineEnd lineBegin, lineEnd;

    ATFInterpreter atfInterp;
    QPixmap pix;
    bool redrawPix;

};

#endif
