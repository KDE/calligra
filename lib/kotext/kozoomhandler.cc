/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <faure@kde.org>

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

#include "kozoomhandler.h"
#include <kdebug.h>
#include <qpaintdevice.h>
#include <koGlobal.h>

KoZoomHandler::KoZoomHandler()
{
    m_layoutUnitFactor = 1;
    // Note that this calls the method below, not the derived one
    setZoomAndResolution( 100, QPaintDevice::x11AppDpiX(), QPaintDevice::x11AppDpiY(), false, false );
}

KoZoomHandler::~KoZoomHandler()
{
}

void KoZoomHandler::setZoomAndResolution( int zoom, int dpiX, int dpiY, bool, bool )
{
    m_zoom = zoom;
    // m_resolution[XY] is in pixel per pt
    m_resolutionX = POINT_TO_INCH( static_cast<double>(dpiX) );
    m_resolutionY = POINT_TO_INCH( static_cast<double>(dpiY) );
    m_zoomedResolutionX = static_cast<double>(m_zoom) * m_resolutionX / 100.0;
    m_zoomedResolutionY = static_cast<double>(m_zoom) * m_resolutionY / 100.0;
    kdDebug(32002) << "KoZoomHandler::setZoomAndResolution " << zoom << " " << dpiX << "," << dpiY
                   << " m_resolutionX=" << m_resolutionX
                   << " m_zoomedResolutionX=" << m_zoomedResolutionX
                   << " m_resolutionY=" << m_resolutionY
                   << " m_zoomedResolutionY=" << m_zoomedResolutionY << endl;
}

void KoZoomHandler::setPtToLayoutUnitFactor( int factor )
{
    m_layoutUnitFactor = factor;
}

int KoZoomHandler::fontSizeToLayoutUnit( double ptSizeFloat, bool forPrint ) const
{
    return ptToLayoutUnit( ptSizeFloat / ( m_zoomedResolutionY *
        ( forPrint ? 1.0 : (72.0 / QPaintDevice::x11AppDpiY()) ) ) );
}

double KoZoomHandler::layoutUnitToFontSize( int luSize, bool forPrint ) const
{
    // This is because we are setting pt sizes (so Qt applies x11AppDpiY already, unless when printing!)
    return layoutUnitToPt( luSize ) * m_zoomedResolutionY
        * ( forPrint ? 1.0 : (72.0 / QPaintDevice::x11AppDpiY()) );
}
