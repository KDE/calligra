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
 * This class handles the zooming and DPI stuff (conversions between pt values and pixels).
 * It is now separate from KWDocument so that it's possible to create paragraphs
 * without a KWDocument (for instance, for the previews), and in case another KOffice
 * app wants to reuse this class.
 */
class KoZoomHandler
{
public:
    KoZoomHandler();
    virtual ~KoZoomHandler();

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

    // TODO blah blah about layout units
    void setPtToLayoutUnitFactor( int factor );

    // Not zoom dependent. Simply convert a pt value (e.g. a frame)
    // to high-resolution layout unit coordinates.
    int ptToLayoutUnit( double pt ) const
                   // is qRound needed ?
    { return static_cast<int>( pt * static_cast<double>( m_layoutUnitFactor ) ); }
    QPoint ptToLayoutUnit( const KoPoint &p ) const
    { return QPoint( ptToLayoutUnit( p.x() ),
                     ptToLayoutUnit( p.y() ) ); }
    QRect ptToLayoutUnit( const KoRect &r ) const
    { return QRect( ptToLayoutUnit( r.topLeft() ),
                    ptToLayoutUnit( r.bottomRight() ) ); }

    double layoutUnitToPt( int lu ) const
    { return static_cast<double>( lu ) / static_cast<double>( m_layoutUnitFactor ); }
    KoPoint layoutUnitToPt( const QPoint& p ) const
    { return KoPoint( layoutUnitToPt( p.x() ),
                     layoutUnitToPt( p.y() ) ); }

    // Zoom dependent
    int pixelToLayoutUnitX( int x ) const
    { return ptToLayoutUnit( unzoomItX( x ) ); }
    int pixelToLayoutUnitY( int y ) const
    { return ptToLayoutUnit( unzoomItY( y ) ); }
    QPoint pixelToLayoutUnit( const QPoint &p ) const
    { return QPoint( pixelToLayoutUnitX( p.x() ),
                     pixelToLayoutUnitY( p.y() ) ); }
    QRect pixelToLayoutUnit( const QRect &r ) const
    { return QRect( pixelToLayoutUnit( r.topLeft() ),
                    pixelToLayoutUnit( r.bottomRight() ) ); }

    int layoutUnitToPixelX( int x ) const
    { return zoomItX( layoutUnitToPt( x ) ); }
    int layoutUnitToPixelY( int y ) const
    { return zoomItY( layoutUnitToPt( y ) ); }

    // This variant converts a height, using y as reference.
    // This prevents rounding problems.
    int layoutUnitToPixelY( int y, int h ) const;

    QPoint layoutUnitToPixel( const QPoint &p ) const
    { return QPoint( layoutUnitToPixelX( p.x() ),
                     layoutUnitToPixelY( p.y() ) ); }
    QRect layoutUnitToPixel( const QRect &r ) const
    { return QRect( layoutUnitToPixel( r.topLeft() ),
                    layoutUnitToPixel( r.bottomRight() ) ); }

    // Convert fontsizes from and to the layout units and the _pixels_ (for the current zoom)
    int fontSizeToLayoutUnit( double ptSizeFloat, bool forPrint ) const;
    double layoutUnitToFontSize( int luSize, bool forPrint ) const;
    // For converting fontsizes from/to layout units and pt sizes, use ptToLayoutUnit and layoutUnitToPt

protected:
    int m_zoom;
    int m_layoutUnitFactor;
    double m_resolutionX;
    double m_resolutionY;
    double m_zoomedResolutionX;
    double m_zoomedResolutionY;
};

#endif
