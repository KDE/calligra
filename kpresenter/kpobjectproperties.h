// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2005 Thorsten Zachmann <zachmann@kde.org>

   The code is mostly a copy from kword/framedia.cc

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

#ifndef KPOBJECTPROPERTIES_H
#define KPOBJECTPROPERTIES_H

#include "kprcommand.h"

class KPObjectProperties
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
        PtOther = 256
    };

    KPObjectProperties( const QPtrList<KPObject> &objects );
    ~KPObjectProperties();

    int getPropertyFlags() { return m_flags; }

    /// get rectangle properties
    RectValueCmd::RectValues getRectValues() const { return m_rectValues; }
    /// get polygon properties
    PolygonSettingCmd::PolygonSettings getPolygonSettings() const { return m_polygonSettings; }
    /// get pie properties
    PieValueCmd::PieValues getPieValues() const { return m_pieValues; }
    // get picture properties
    PictureSettingCmd::PictureSettings getPictureSettings() const { return m_pictureSettings; }
    const QPixmap &getPixmap() const { return m_pixmap; }
    /// get text properties
    MarginsStruct getMarginsStruct() { return m_marginsStruct; }
    PropValue getProtectContent() { return m_protectContent; }

protected:
    void getProperties( const QPtrList<KPObject> &objects );

    void getRectProperties( KPObject *object );
    void getPolygonSettings( KPObject *object );
    void getPieProperties( KPObject *object );
    void getPictureProperties( KPObject *object );
    void getTextProperties( KPObject *object );

private:
    QPtrList<KPObject> m_objects;
    int m_flags;

    /// rect properties
    RectValueCmd::RectValues m_rectValues;
    /// polygon properties
    PolygonSettingCmd::PolygonSettings m_polygonSettings;
    /// pie properties
    PieValueCmd::PieValues m_pieValues;
    /// picture properties
    PictureSettingCmd::PictureSettings m_pictureSettings;
    QPixmap m_pixmap;
    /// text properties
    MarginsStruct m_marginsStruct;
    PropValue m_protectContent;
};

#endif /* KPOBJECTPROPERTIES_H */
