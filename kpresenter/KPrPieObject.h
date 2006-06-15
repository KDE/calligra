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

#ifndef kppieobject_h
#define kppieobject_h

#include "KPrObject.h"
#include <KoBrush.h>
#include <KoStyleStack.h>

class KPrGradient;
class KPrPieObjectAdaptor;

class KPrPieObject : public KPr2DObject, public KPrStartEndLine
{
public:
    KPrPieObject();
    KPrPieObject( const KoPen &_pen, const QBrush &_brush, FillType _fillType,
                 const QColor &_gColor1, const QColor &_gColor2, BCType _gType, PieType _pieType,
                 int _p_angle, int _p_len, LineEnd _lineBegin, LineEnd _lineEnd,
                 bool _unbalanced, int _xfactor, int _yfactor );
    virtual ~KPrPieObject() {}
    virtual KPrObjectAdaptor* dbusObject();
    KPrPieObject &operator=( const KPrPieObject & );

    virtual void setPieType( PieType _pieType )
        { pieType = _pieType; m_redrawGradientPix = true; }
    virtual void setPieAngle( int _p_angle )
        { p_angle = _p_angle; m_redrawGradientPix = true; }
    virtual void setPieLength( int _p_len )
        { p_len = _p_len; m_redrawGradientPix = true; }
    virtual void setLineBegin( LineEnd _lineBegin )
        { lineBegin = _lineBegin; }
    virtual void setLineEnd( LineEnd _lineEnd )
        { lineEnd = _lineEnd; }

    virtual ObjType getType() const
        { return OT_PIE; }
    virtual QString getTypeString() const
        {
            switch ( pieType ) {
            case PT_PIE:
                return i18n("Pie");
                break;
            case PT_ARC:
                return i18n("Arc");
                break;
            case PT_CHORD:
                return i18n("Chord");
                break;
            }
            return QString::null;
        }

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

    virtual QDomDocumentFragment save( QDomDocument& doc, double offset );

    virtual double load(const QDomElement &element);
    virtual void flip(bool horizontal );
    virtual void loadOasis(const QDomElement &element, KoOasisContext & context, KPrLoadingInfo *info);

    virtual KoSize getRealSize() const;
    virtual KoPoint getRealOrig() const;

protected:
    virtual const char * getOasisElementName() const;
    virtual bool saveOasisObjectAttributes( KPOasisSaveContext &sc ) const;
    virtual void fillStyle( KoGenStyle& styleObjectAuto, KoGenStyles& mainStyles ) const;

    virtual void paint( QPainter *_painter, KoTextZoomHandler*_zoomHandler,
                        int /* pageNum */, bool drawingShadow, bool drawContour );

    void setMinMax( double &min_x, double &min_y, double &max_x, double &max_y, KoPoint point ) const;
    void getRealSizeAndOrig( KoSize &size, KoPoint &realOrig ) const;

    void setEndPoints( KoPointArray &points ) const;

    PieType pieType;
    int p_angle, p_len;
};

#endif
