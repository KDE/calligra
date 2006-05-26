// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2005 Thorsten Zachmann <zachmann@kde.org>
   based on work by
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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef RECTPREVIEW_H
#define RECTPREVIEW_H

#include <q3frame.h>
#include <qpen.h>
#include <qbrush.h>

class KPrRectPreview : public Q3Frame
{
    Q_OBJECT

public:
    KPrRectPreview( QWidget* parent );
    ~KPrRectPreview() {}

    void setRnds( int _rx, int _ry )
        { xRnd = _rx; yRnd = _ry; repaint( contentsRect(), true ); }
    void setPenBrush( const QPen &_pen, const QBrush &_brush )
        { pen = _pen; brush = _brush; repaint( true ); }

protected:
    void drawContents( QPainter* );

    int xRnd, yRnd;
    QPen pen;
    QBrush brush;
};

#endif /* RECTPREVIEW_H */
