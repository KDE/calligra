/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>

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


#include "contextstyle.h"

ContextStyle::ContextStyle()
        : m_zoomedResolutionX(1.0), m_zoomedResolutionY(1.0),
          defaultFont("times"), nameFont("times"), numberFont("symbol"),
          operatorFont("symbol"), symbolFont("symbol"),
          defaultColor(Qt::black), numberColor(Qt::blue),
          operatorColor(Qt::darkGreen), errorColor(Qt::darkRed),
          emptyColor(Qt::blue)
{
    nameFont.setItalic(true);
    distance = 4;
    operatorSpace = 4;
    linearMovement = false;
    baseSize = 18;
    minimumSize = 8;
    sizeReduction = 2;
    lineWidth = 1;
    emptyRectWidth = 10;
    emptyRectHeight = 10;
    centerSymbol = false;
}

void ContextStyle::setResolution(double zX, double zY)
{
    m_zoomedResolutionX = zX;
    m_zoomedResolutionY = zY;
}

int ContextStyle::getDistanceX(int) const
{
    return zoomItX( distance );
}

int ContextStyle::getDistanceY(int) const
{
    return zoomItY( distance );
}

int ContextStyle::getOperatorSpace(int /*size*/) const
{
    return zoomItX( operatorSpace );
}

int ContextStyle::getBaseSize() const
{
    return zoomItY( baseSize );
}

int ContextStyle::getMinimumSize() const
{
    return zoomItY( minimumSize );
}

int ContextStyle::getSizeReduction() const
{
    return zoomItY( sizeReduction );
}

int ContextStyle::getLineWidth() const
{
    return zoomItY( lineWidth );
}


int ContextStyle::getEmptyRectWidth() const
{
    return zoomItY( emptyRectWidth );
}

int ContextStyle::getEmptyRectHeight() const
{
    return zoomItY( emptyRectHeight );
}
