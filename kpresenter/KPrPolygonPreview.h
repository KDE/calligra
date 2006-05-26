// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2005 Thorsten Zachmann <zachmann@kde.org>
   Base code from Kontour.
   Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
   Copyright (C) 2001 Toshitaka Fujioka <fujioka@kde.org>

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

#ifndef POLYGONPREVIEW_H
#define POLYGONPREVIEW_H

#include <q3frame.h>
#include <qpen.h>
#include <qbrush.h>

class QPainter;

class KPrPolygonPreview : public Q3Frame
{
    Q_OBJECT

public:
    KPrPolygonPreview( QWidget* parent, const char*);
    ~KPrPolygonPreview() {}

    void setPenBrush( const QPen &_pen, const QBrush &_brush )
        { pen = _pen; brush = _brush; repaint( true ); }

public slots:
    void slotConvexConcave( bool convexConcave );
    void slotConvexPolygon();
    void slotConcavePolygon();
    void slotCornersValue( int value );
    void slotSharpnessValue( int value );

protected:
    void drawContents( QPainter* );

    int nCorners;
    int sharpness;
    bool isConcave;
    QPen pen;
    QBrush brush;
};

#endif /* POLYGONPREVIEW_H */
