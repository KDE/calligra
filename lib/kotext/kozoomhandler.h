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
#include <koffice_export.h>
/**
 * A KoTextZoomHandler converts between an internal text layout resolution of
 * ~1440 DPI (by default) and the point-size for the fonts (those known by the user).
 * This conversion doesn't depend on the zoom level. Anything depending on the zoom
 * is handled by KoZoomHandler.
 * Not a real class, it only has static methods, since it has no data.
 */
class KOTEXT_EXPORT KoTextZoomHandler
{
public:
    KoTextZoomHandler() {}
    virtual ~KoTextZoomHandler() {}

    /** Change the factor that converts between pointsizes
     * and layout units (by default 20 - for 1440 DPI at 72 DPI) */
    static void setPtToLayoutUnitFactor( int factor ) { m_layoutUnitFactor = factor; }

    /** Not zoom dependent. Simply convert a pt value (e.g. a frame)
     * to high-resolution layout unit coordinates (in pt). */
    static double ptToLayoutUnitPt( double pt )
    { return pt * static_cast<double>( m_layoutUnitFactor ); }
    /** Same thing for integer values, e.g. a font size in pt */
    static int ptToLayoutUnitPt( int ptSize )
    { return ptSize * m_layoutUnitFactor; }

    static KoPoint ptToLayoutUnitPt( const KoPoint &p )
    { return KoPoint( ptToLayoutUnitPt( p.x() ),
                      ptToLayoutUnitPt( p.y() ) ); }
    static KoRect ptToLayoutUnitPt( const KoRect &r )
    { return KoRect( ptToLayoutUnitPt( r.topLeft() ),
                     ptToLayoutUnitPt( r.bottomRight() ) ); }

    static double layoutUnitPtToPt( double lupt )
    { return lupt / static_cast<double>( m_layoutUnitFactor ); }
    static KoPoint layoutUnitPtToPt( const KoPoint& p )
    { return KoPoint( layoutUnitPtToPt( p.x() ),
                      layoutUnitPtToPt( p.y() ) ); }

protected:
    /** This being static ensures that the same value is used by all KoZoomHandler instances */
    static int m_layoutUnitFactor;
};

/**
 * This class handles the zooming and DPI stuff (conversions between pt values and pixels).
 * An instance of KoZoomHandler operates at a given zoom (see setZoomAndResolution() and setZoom())
 * so there is usually one instance of KoZoomHandler per view.
 */
class KOTEXT_EXPORT KoZoomHandler : public KoTextZoomHandler
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
     */
    virtual void setZoomAndResolution( int zoom, int dpiX, int dpiY );

    /**
     * @return the conversion factor between pt and pixel, that
     * takes care of the zoom and the DPI setting.
     * Use zoomIt(pt) instead, though.
     */
    double zoomedResolutionX() const { return m_zoomedResolutionX; }
    double zoomedResolutionY() const { return m_zoomedResolutionY; }

    double resolutionX() const { return m_resolutionX; }
    double resolutionY() const { return m_resolutionY; }

    /**
     * Zoom factor for X. Equivalent to zoomedResolutionX()/resolutionX()
     */
    double zoomFactorX() const { return m_zoomedResolutionX / m_resolutionX; }
    /**
     * Zoom factor for Y. Equivalent to zoomedResolutionY()/resolutionY()
     */
    double zoomFactorY() const { return m_zoomedResolutionY / m_resolutionY; }


    /**
     * Set a resolution for X and Y, when no zoom applies (e.g. when painting an
     * embedded document. This will set the zoom to 100, and it will set
     * zoomedResolution[XY] to the resolution[XY] parameters
     * Helper method, equivalent to setZoomAndResolution(100,...).
     */
    void setResolution( double resolutionX, double resolutionY );

    /**
     * Set the zoomed resolution for X and Y.
     * Compared to the setZoom... methods, this allows to set a different
     * zoom factor for X and for Y.
     */
    virtual void setZoomedResolution( double zoomedResolutionX, double zoomedResolutionY );

    /**
     * Change the zoom level, keeping the resolution unchanged.
     * @param zoom the zoom factor (e.g. 100 for 100%)
     */
    void setZoom( int zoom );

    /**
     * @return the global zoom factor (e.g. 100 for 100%).
     * Only use this to display to the user, don't use in calculations
     */
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
        QRect _r;
        _r.setCoords( zoomItX( r.left() ),  zoomItY( r.top() ),
                      zoomItX( r.right() ), zoomItY( r.bottom() ) );
        return _r;
    }
    /**
     * Returns the size in pixels for a input size in points.
     *
     * This function can return a size with 1 pixel to less, depending
     * on the reference point and the width and/or the zoom level.
     * It's save to use if the starting point is (0/0).
     * You can use it if you don't know the starting point yet
     * (like when inserting a picture), but then please take
     * care of it afterwards, when you know the reference point.
     */
    QSize zoomSize( const KoSize & s ) const {
        return QSize( zoomItX( s.width() ), zoomItY( s.height() ) );
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
        KoRect _r;
        _r.setCoords( unzoomItX( r.left() ),  unzoomItY( r.top() ),
                      unzoomItX( r.right() ), unzoomItY( r.bottom() ) );
        return _r;
    }


    //// Support for WYSIWYG text layouting /////

    /** The "[zoomed] view pixel" -> "layout unit pixel" conversions. */
    int pixelToLayoutUnitX( int x ) const;
    int pixelToLayoutUnitY( int y ) const;
    QPoint pixelToLayoutUnit( const QPoint &p ) const
    { return QPoint( pixelToLayoutUnitX( p.x() ),
                     pixelToLayoutUnitY( p.y() ) ); }
    QRect pixelToLayoutUnit( const QRect &r ) const
    { return QRect( pixelToLayoutUnit( r.topLeft() ),
                    pixelToLayoutUnit( r.bottomRight() ) ); }

    /** The "layout unit pixel" -> "[zoomed] view pixel" conversions. */
    int layoutUnitToPixelX( int lupix ) const;
    int layoutUnitToPixelY( int lupix ) const;

    /** This variant converts a width, using a reference X position.
     * This prevents rounding problems. */
    int layoutUnitToPixelX( int x, int w ) const;
    /** This variant converts a height, using a reference Y position.
     * This prevents rounding problems. */
    int layoutUnitToPixelY( int y, int h ) const;

    QPoint layoutUnitToPixel( const QPoint &p ) const
    { return QPoint( layoutUnitToPixelX( p.x() ),
                     layoutUnitToPixelY( p.y() ) ); }
    QRect layoutUnitToPixel( const QRect &r ) const
    { return QRect( layoutUnitToPixel( r.topLeft() ),
                    layoutUnitToPixel( r.bottomRight() ) ); }

    /** Basic pt to pixel and pixel to pt conversions, valid at any zoom level,
        as well as at the Layout Unit level (and mostly useful for Layout Units).
        Don't confuse with zoomIt, which also converts pt to pixels, but applying the zoom! */
    int ptToPixelX( double pt ) const
    { return qRound( pt * m_resolutionX ); }
    int ptToPixelY( double pt ) const
    { return qRound( pt * m_resolutionY ); }
    QPoint ptToPixel( const KoPoint & p ) const {
        return QPoint( ptToPixelX( p.x() ), ptToPixelY( p.y() ) );
    }
    double pixelXToPt( int x ) const
    { return static_cast<double>(x) / m_resolutionX; }
    double pixelYToPt( int y ) const
    { return static_cast<double>(y) / m_resolutionY; }
    KoPoint pixelToPt( const QPoint& p ) const {
        return KoPoint( pixelXToPt( p.x() ), pixelYToPt( p.y() ) );
    }

    /** The "document pt" -> "Layout Unit pixels" conversions, for convenience */
    int ptToLayoutUnitPixX( double x_pt ) const
    { return ptToPixelX( ptToLayoutUnitPt( x_pt ) ); }
    int ptToLayoutUnitPixY( double y_pt ) const
    { return ptToPixelY( ptToLayoutUnitPt( y_pt ) ); }
    QPoint ptToLayoutUnitPix( const KoPoint & p ) const {
        return QPoint( ptToLayoutUnitPixX( p.x() ), ptToLayoutUnitPixY( p.y() ) );
    }

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
