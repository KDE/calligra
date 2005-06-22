// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2005 Thorsten Zachmann <zachmann@kde.org>

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

#ifndef kpellipseobject_h
#define kpellipseobject_h

#include "kpobject.h"
#include "global.h"

class KPGradient;
class QPainter;
class DCOPObject;

class KPEllipseObject : public KP2DObject
{
public:
    KPEllipseObject();
    KPEllipseObject( const QPen &_pen, const QBrush &_brush, FillType _fillType,
                     const QColor &_gColor1, const QColor &_gColor2, BCType _gType,
                     bool _unbalanced, int _xfactor, int _yfactor );
    virtual ~KPEllipseObject() {}

    virtual DCOPObject* dcopObject();


    KPEllipseObject &operator=( const KPEllipseObject & );

    virtual ObjType getType() const
        { return OT_ELLIPSE; }
    virtual QString getTypeString() const
        { return i18n("Ellipse"); }

    virtual KoSize getRealSize() const;

protected:
    virtual const char * getOasisElementName() const;
    virtual bool saveOasisObjectAttributes( KPOasisSaveContext &sc ) const;

    virtual void paint( QPainter *_painter,KoZoomHandler *_zoomHandler,
                        int /* pageNum */, bool drawingShadow, bool drawContour = FALSE );

};

#endif
