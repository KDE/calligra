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

#ifndef kozoomhandler_h
#define kozoomhandler_h

#include <koRect.h>

/**
 * A KoTextZoomHandler converts between an internal text layout resolution of
 * ~1440 DPI (by default) and the point-size for the fonts (those known by the user).
 * This conversion doesn't depend on the zoom level. Anything depending on the zoom
 * is handled by KoZoomHandler.
 * Not a real class, it only has static methods, since it has no data.
 */
class KoTextZoomHandler
{
public:
    KoTextZoomHandler() {}
    virtual ~KoTextZoomHandler() {}

    /** Change the factor that converts between pointsizes
     * and layout units (by default 20 - for 1440 DPI at 72 DPI) */
    static void setPtToLayoutUnitFactor( int factor ) { m_layoutUnitFactor = factor; }

    /** Not zoom dependent. Simply convert a pt value (e.g. a frame)
     * to high-resolution layout unit coordinates. */
    static int ptToLayoutUnit( double pt )
                   // is qRound needed ?
    { return static_cast<int>( pt * static_cast<double>( m_layoutUnitFactor ) ); }
    /** Same thing for integer values, e.g. a font size in pt */
    static int ptToLayoutUnit( int ptSize )
    { return ptSize * m_layoutUnitFactor; }

    static QPoint ptToLayoutUnit( const KoPoint &p )
    { return QPoint( ptToLayoutUnit( p.x() ),
                     ptToLayoutUnit( p.y() ) ); }
    static QRect ptToLayoutUnit( const KoRect &r )
    { return QRect( ptToLayoutUnit( r.topLeft() ),
                    ptToLayoutUnit( r.bottomRight() ) ); }

    static double layoutUnitToPt( int lu )
    { return static_cast<double>( lu ) / static_cast<double>( m_layoutUnitFactor ); }
    static KoPoint layoutUnitToPt( const QPoint& p )
    { return KoPoint( layoutUnitToPt( p.x() ),
                      layoutUnitToPt( p.y() ) ); }

protected:
    /** This being static ensures that the same value is used by all KoZoomHandler instances */
    static int m_layoutUnitFactor;
};

/**
 * This class handles the zooming and DPI stuff (conversions between pt values and pixels).
 * An instance of KoZoomHandler operates at a given zoom (see @ref setZoomAndResolution)
 * so there is usually one instance of KoZoomHandler per view.
 */
class KoZoomHandler : public KoTextZoomHandler
{
public:
    KoZoomHandler();
    virtual ~KoZoomHandler() {}

    /**
     * Change the zoom factor to @p z (e.g. 150 for 150%)
     * and/or change the resolution, given in DPI.
     * This is done on startup, when zooming, and when printing.
     * The same call combines both so that all the updating done behind
     * the scenes is done only once, even if both zoom and DPI must be changed.
     * @param updateViews whether to repaint all views accordingly [to be changed to updateView]
     * @param forPrint whether this is for printing [might be removed in the future]
     */
    virtual void setZoomAndResolution( int zoom, int dpiX, int dpiY, bool updateViews, bool forPrint );

    /**
     * @return the conversion factor between pt and pixel, that
     * takes care of the zoom and the DPI setting.
     * Use zoomIt(pt) instead, though.
     */
    double zoomedResolutionX() const { return m_zoomedResolutionX; }
    double zoomedResolutionY() const { return m_zoomedResolutionY; }

    double resolutionX() const { return m_resolutionX; }
    double resolutionY() const { return m_resolutionY; }

    int zoom() const { return m_zoom; }

    // Input: pt. Output: pixels. Resolution and zoom are applied.
    int zoomItX( double z ) const {
        return qRound( m_zoomedResolutionX * z );
    }
    int zoomItY( double z ) const {
        return qRound( m_zoomedResolutionY * z );
    }

    QPoint zoomPoint( const KoPoint & p ) const {
        return QPoint( zoomItX( p.x() ), zoomItY( p.y() ) );
    }
    QRect zoomRect( const KoRect & r ) const {
        return QRect( zoomItX( r.x() ), zoomItY( r.y() ),
                      zoomItX( r.width() ), zoomItY( r.height() ) );
    }

    // Input: pixels. Output: pt.
    double unzoomItX( int x ) const {
        return static_cast<double>( x ) / m_zoomedResolutionX;
    }
    double unzoomItY( int y ) const {
        return static_cast<double>( y ) / m_zoomedResolutionY;
    }
    KoPoint unzoomPoint( const QPoint & p ) const {
        return KoPoint( unzoomItX( p.x() ), unzoomItY( p.y() ) );
    }
    KoRect unzoomRect( const QRect & r ) const {
        return KoRect( unzoomItX( r.x() ), unzoomItY( r.y() ),
                       unzoomItX( r.width() ), unzoomItY( r.height() ) );
    }


    //// Support for WYSIWYG text layouting /////

    // The pixel <-> layout unit conversions
    // apply both the pt<->LU conversion, and the zoom level.
    // Understand that LU is "pixels in the layout unit system",
    // so there is no need for applying the resolution here.
    // We call ptToLayoutUnit but we mean pixelToPixelsInLayoutUnit.
    int pixelToLayoutUnitX( int x ) const
    { return ptToLayoutUnit( x * 100.0 / static_cast<double>(m_zoom) ); }
    int pixelToLayoutUnitY( int y ) const
    { return ptToLayoutUnit( y * 100.0 / static_cast<double>(m_zoom) ); }
    QPoint pixelToLayoutUnit( const QPoint &p ) const
    { return QPoint( pixelToLayoutUnitX( p.x() ),
                     pixelToLayoutUnitY( p.y() ) ); }
    QRect pixelToLayoutUnit( const QRect &r ) const
    { return QRect( pixelToLayoutUnit( r.topLeft() ),
                    pixelToLayoutUnit( r.bottomRight() ) ); }

    int layoutUnitToPixelX( int x ) const
    { return qRound( layoutUnitToPt( x ) * static_cast<double>(m_zoom) / 100.0 ); }
    int layoutUnitToPixelY( int y ) const
    { return qRound( layoutUnitToPt( y ) * static_cast<double>(m_zoom) / 100.0 ); }

    // This variant converts a height, using y as reference.
    // This prevents rounding problems.
    int layoutUnitToPixelY( int y, int h ) const;

    QPoint layoutUnitToPixel( const QPoint &p ) const
    { return QPoint( layoutUnitToPixelX( p.x() ),
                     layoutUnitToPixelY( p.y() ) ); }
    QRect layoutUnitToPixel( const QRect &r ) const
    { return QRect( layoutUnitToPixel( r.topLeft() ),
                    layoutUnitToPixel( r.bottomRight() ) ); }

    /**
     * Given the font size for the font in layout units, in pt (use pointSize())
     * this returns the font size to use on screen the current zoom, in pt (use setPointSizeFloat()),
     */
    double layoutUnitToFontSize( int luSize, bool /*forPrint*/ ) const;

    // Note: For converting fontsizes from/to layout units and zoom-independent
    // pt sizes (like the one the user sees, e.g. 12pt),
    // use ptToLayoutUnit and layoutUnitToPt, not layoutToFontSize.

protected:
    int m_zoom;
    double m_resolutionX;
    double m_resolutionY;
    double m_zoomedResolutionX;
    double m_zoomedResolutionY;
};

#endif
