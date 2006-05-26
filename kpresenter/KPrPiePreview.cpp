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

#include "KPrPiePreview.h"

#include <qpainter.h>
//Added by qt3to4:
#include <Q3Frame>

KPrPiePreview::KPrPiePreview( QWidget* parent )
    : Q3Frame( parent )
{
    setFrameStyle( WinPanel | Sunken );
    setBackgroundColor( Qt::white );
    angle = 720; //45 * 16
    len = 1440; //90 * 16
    pen = QPen( Qt::black );
    brush = QBrush( Qt::white );
    type = PT_PIE;

    setMinimumSize( 200, 100 );
}

void KPrPiePreview::drawContents( QPainter* painter )
{
    int ow = width() - 8;
    int oh = height() - 8;

    painter->setPen( pen );
    int pw = pen.width();
    painter->setBrush( brush );

    switch ( type )
    {
        case PT_PIE:
            painter->drawPie( pw + 4, pw + 4, ow - 2 * pw, oh - 2 * pw, angle, len );
            break;
        case PT_ARC:
            painter->drawArc( pw + 4, pw + 4, ow - 2 * pw, oh - 2 * pw, angle, len );
            break;
        case PT_CHORD:
            painter->drawChord( pw + 4, pw + 4, ow - 2 * pw, oh - 2 * pw, angle, len );
            break;
        default: 
            break;
    }
}

#include "KPrPiePreview.moc"
