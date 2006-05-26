// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2005-2006 Thorsten Zachmann <zachmann@kde.org>

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

#ifndef KPOBJECTPROPERTIES_H
#define KPOBJECTPROPERTIES_H

#include "KPrCommand.h"
//Added by qt3to4:
#include <QPixmap>
#include <Q3PtrList>

class KPrObjectProperties
{
public:
    enum PropertyType {
        PtPen = 1,
        PtLineEnds = 2,
        PtBrush = 4,
        PtPie = 8,
        PtPicture = 16,
        PtPolygon = 32,
        PtRectangle = 64,
        PtText = 128,
        PtOther = 256,
        PtPenWidth = 512    
    };

    KPrObjectProperties( const Q3PtrList<KPrObject> &objects );
    ~KPrObjectProperties();

    int getPropertyFlags() { return m_flags; }

    /// get pen properties
    KoPenCmd::Pen getPen() const { return m_pen; }
    /// get brush properties
    KPrBrushCmd::Brush getBrush() const { return m_brush; }
    /// get rectangle properties
    KPrRectValueCmd::RectValues getRectValues() const { return m_rectValues; }
    /// get polygon properties
    KPrPolygonSettingCmd::PolygonSettings getPolygonSettings() const { return m_polygonSettings; }
    /// get pie properties
    KPrPieValueCmd::PieValues getPieValues() const { return m_pieValues; }
    // get picture properties
    KPrPictureSettingCmd::PictureSettings getPictureSettings() const { return m_pictureSettings; }
    const QPixmap &getPixmap() const { return m_pixmap; }
    /// get text properties
    MarginsStruct getMarginsStruct() { return m_marginsStruct; }
    PropValue getProtectContent() { return m_protectContent; }

protected:
    void getProperties( const Q3PtrList<KPrObject> &objects );

    void getPenProperties( KPrObject *object );
    void getLineEndsProperties( KPrObject *object );
    void getBrushProperties( KPrObject *object );
    void getRectProperties( KPrObject *object );
    void getPolygonSettings( KPrObject *object );
    void getPieProperties( KPrObject *object );
    void getPictureProperties( KPrObject *object );
    void getTextProperties( KPrObject *object );

private:
    Q3PtrList<KPrObject> m_objects;
    int m_flags;

    /// pen properties
    KoPenCmd::Pen m_pen;
    /// brush properties
    KPrBrushCmd::Brush m_brush;
    /// rect properties
    KPrRectValueCmd::RectValues m_rectValues;
    /// polygon properties
    KPrPolygonSettingCmd::PolygonSettings m_polygonSettings;
    /// pie properties
    KPrPieValueCmd::PieValues m_pieValues;
    /// picture properties
    KPrPictureSettingCmd::PictureSettings m_pictureSettings;
    QPixmap m_pixmap;
    /// text properties
    MarginsStruct m_marginsStruct;
    PropValue m_protectContent;
};

#endif /* KPOBJECTPROPERTIES_H */
