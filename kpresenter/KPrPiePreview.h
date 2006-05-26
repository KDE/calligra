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

#ifndef PIEPREVIEW_H
#define PIEPREVIEW_H

#include <q3frame.h>
#include <qpen.h>
#include <qbrush.h>

#include "global.h"

class KPrPiePreview : public Q3Frame
{
    Q_OBJECT

public:
    KPrPiePreview( QWidget* parent );
    ~KPrPiePreview() {}

    void setAngle( int _angle )
        { angle = _angle; repaint( true ); }
    void setLength( int _len )
        { len = _len; repaint( true ); }
    void setType( PieType _type )
        { type = _type; repaint( true ); }
    void setPenBrush( const QPen &_pen, const QBrush &_brush )
        { pen = _pen; brush = _brush; repaint( true ); }

protected:
    void drawContents( QPainter* );

    int angle, len;
    PieType type;
    QPen pen;
    QBrush brush;
};

#endif /* PIEPREVIEW_H */
