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
#include <koUnit.h>
#include <koGlobal.h>

// Layout text at 1440 DPI
// Well, not really always 1440 DPI, but always 20 times the point size
// This is constant, no need to litterally apply 1440 DPI at all resolutions.
int KoTextZoomHandler::m_layoutUnitFactor = 20;

KoZoomHandler::KoZoomHandler()
{
    // Note that this calls the method below, not the derived one
    setZoomAndResolution( 100, KoGlobal::dpiX(), KoGlobal::dpiY() );
}

void KoZoomHandler::setZoomAndResolution( int zoom, int dpiX, int dpiY )
{
    // m_resolution[XY] is in pixel per pt
    m_resolutionX = POINT_TO_INCH( static_cast<double>(dpiX) );
    m_resolutionY = POINT_TO_INCH( static_cast<double>(dpiY) );
    setZoom( zoom );
    /*kdDebug(32500) << "KoZoomHandler::setZoomAndResolution " << zoom << " " << dpiX << "," << dpiY
              << " m_resolutionX=" << m_resolutionX
              << " m_zoomedResolutionX=" << m_zoomedResolutionX
              << " m_resolutionY=" << m_resolutionY
              << " m_zoomedResolutionY=" << m_zoomedResolutionY << endl;*/
}

void KoZoomHandler::setResolution( double resolutionX, double resolutionY )
{
    m_zoom = 100;
    m_resolutionX = resolutionX;
    m_resolutionY = resolutionY;
    m_zoomedResolutionX = resolutionX;
    m_zoomedResolutionY = resolutionY;
}

void KoZoomHandler::setZoomedResolution( double zoomedResolutionX, double zoomedResolutionY )
{
    // m_zoom doesn't matter, it's only used in setZoom() to calculated the zoomed resolutions
    // Here we know them. The whole point of this method is to allow a different zoom factor
    // for X and for Y, as can be useful for e.g. fullscreen kpresenter presentations.
    m_zoomedResolutionX = zoomedResolutionX;
    m_zoomedResolutionY = zoomedResolutionY;
}

void KoZoomHandler::setZoom( int zoom )
{
    m_zoom = zoom;
    if( m_zoom == 100 ) {
        m_zoomedResolutionX = m_resolutionX;
        m_zoomedResolutionY = m_resolutionY;
    } else {
        m_zoomedResolutionX = static_cast<double>(m_zoom) * m_resolutionX / 100.0;
        m_zoomedResolutionY = static_cast<double>(m_zoom) * m_resolutionY / 100.0;
    }
}

#if 0
int KoZoomHandler::fontSizeToLayoutUnit( double ptSizeFloat, bool forPrint ) const
{
    return ptToLayoutUnit( ptSizeFloat / ( m_zoomedResolutionY *
        ( forPrint ? 1.0 : (72.0 / KoGlobal::dpiY()) ) ) );
}
#endif

double KoZoomHandler::layoutUnitToFontSize( int luSize, bool /*forPrint*/ ) const
{
    // Qt will use QPaintDevice::x11AppDpiY() to go from pt to pixel for fonts
    return layoutUnitPtToPt( luSize ) * m_zoomedResolutionY
#ifdef Q_WS_X11
        / POINT_TO_INCH(QPaintDevice::x11AppDpiY())
#endif
        ;
}

int KoZoomHandler::layoutUnitToPixelX( int x, int w ) const
{
    // We call layoutUnitToPixelX on the right value, i.e. x+w-1,
    // and then determine the height from the result (i.e. right-left+1).
    // Calling layoutUnitToPixelX(w) leads to rounding problems.
    return layoutUnitToPixelY( x + w - 1 ) - layoutUnitToPixelY( x ) + 1;
}

int KoZoomHandler::layoutUnitToPixelY( int y, int h ) const
{
    // We call layoutUnitToPixelY on the bottom value, i.e. y+h-1,
    // and then determine the height from the result (i.e. bottom-top+1).
    // Calling layoutUnitToPixelY(h) leads to rounding problems.
    return layoutUnitToPixelY( y + h - 1 ) - layoutUnitToPixelY( y ) + 1;
}

int KoZoomHandler::layoutUnitToPixelX( int lupix ) const
{
    return int( static_cast<double>( lupix * m_zoomedResolutionX )
                / ( static_cast<double>( m_layoutUnitFactor ) * m_resolutionX ) );
}

int KoZoomHandler::layoutUnitToPixelY( int lupix ) const
{
    // qRound replaced with a truncation, too many problems (e.g. bottom of parags)
    return int( static_cast<double>( lupix * m_zoomedResolutionY )
                / ( static_cast<double>( m_layoutUnitFactor ) * m_resolutionY ) );
}

int KoZoomHandler::pixelToLayoutUnitX( int x ) const
{
    return qRound( static_cast<double>( x * m_layoutUnitFactor * m_resolutionX )
                   / m_zoomedResolutionX );
}

int KoZoomHandler::pixelToLayoutUnitY( int y ) const
{
    return qRound( static_cast<double>( y * m_layoutUnitFactor * m_resolutionY )
                   / m_zoomedResolutionY );
}

