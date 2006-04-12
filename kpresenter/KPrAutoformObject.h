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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef kpautoformobject_h
#define kpautoformobject_h

#include <qpixmap.h>
#include <KoBrush.h>

#include "KPrObject.h"
#include "global.h"
#include "autoformEdit/ATFInterpreter.h"

class KPrGradient;
class DCOPObject;

class KPrAutoformObject : public KPr2DObject, public KPrStartEndLine
{
public:
    KPrAutoformObject();
    KPrAutoformObject( const KoPen & _pen, const QBrush &_brush, const QString &_filename, LineEnd _lineBegin, LineEnd _lineEnd,
                      FillType _fillType, const QColor &_gColor1, const QColor &_gColor2, BCType _gType,
                      bool _unbalanced, int _xfactor, int _yfactor);
    virtual ~KPrAutoformObject() {}

    KPrAutoformObject &operator=( const KPrAutoformObject & );

    virtual DCOPObject* dcopObject();

    virtual void setFileName( const QString &_filename );
    virtual void setLineBegin( LineEnd _lineBegin )
        { lineBegin = _lineBegin; }
    virtual void setLineEnd( LineEnd _lineEnd )
        { lineEnd = _lineEnd; }

    virtual ObjType getType() const
        { return OT_AUTOFORM; }
    virtual QString getTypeString() const
        { return i18n("Autoform"); }

    QString getFileName() const
        { return filename; }
    virtual LineEnd getLineBegin() const
        { return lineBegin; }
    virtual LineEnd getLineEnd() const
        { return lineEnd; }

    virtual QDomDocumentFragment save( QDomDocument& doc,double offset );

    virtual double load(const QDomElement &element);
protected:
    virtual const char * getOasisElementName() const;
    virtual bool saveOasisObjectAttributes( KPOasisSaveContext &sc ) const;
    virtual void fillStyle( KoGenStyle& styleObjectAuto, KoGenStyles& mainStyles ) const;

    virtual void paint( QPainter *_painter, KoTextZoomHandler *_zoomHandler,
                        int /* pageNum */, bool drawingShadow, bool drawContour = false );

    QString filename;

    ATFInterpreter atfInterp;
};

#endif
