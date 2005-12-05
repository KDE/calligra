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
#include "KPrRectPreview.h"

#include <qpainter.h>

KPrRectPreview::KPrRectPreview( QWidget* parent, const char* name )
    : QFrame( parent, name )
{
    setFrameStyle( WinPanel | Sunken );
    setBackgroundColor( white );
    xRnd = 0;
    yRnd = 0;

    setMinimumSize( 200, 100 );
}

void KPrRectPreview::drawContents( QPainter* painter )
{
    int ow = width();
    int oh = height();

    painter->setPen( pen );
    painter->setBrush( brush );

    painter->save();
    painter->drawRoundRect( 10, 10, ow - 20, oh - 20, xRnd, yRnd );
    painter->restore();
}

#include "KPrRectPreview.moc"
