/* This file is part of the KDE project
 * Copyright (c) 2003 thierry lorthiois (lorthioist@wanadoo.fr)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License version 2 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/
#ifndef _WMFIMPORTPARSER_H_
#define _WMFIMPORTPARSER_H_

#include <QPainter>
#include <qdom.h>
//Added by qt3to4:
#include <Q3PointArray>
#include <Q3PtrList>
#include <core/vdocument.h>
#include <core/vcomposite.h>
#include <kowmfread.h>


/**
 * WMFImportParser inherit KoWmfRead
 * and translate WMF functions
 *
 */
 
class WMFImportParser : public KoWmfRead
{
public:
    WMFImportParser();
    ~WMFImportParser() { }

    /**
     * play WMF file on a VDocument. Return true on success.
     */
    bool play( VDocument& doc );


private:
    // -------------------------------------------------------------------------
    // A virtual QPainter 
    bool  begin();
    bool  end();
    void  save();
    void  restore();

    // Drawing tools
    void  setFont( const QFont &font );
    // the pen : the width of the pen is in logical coordinate
    void  setPen( const QPen &pen );
    const QPen &pen() const;
    void  setBrush( const QBrush &brush );

    // Drawing attributes/modes
    void  setBackgroundColor( const QColor &c );
    void  setBackgroundMode( Qt::BGMode mode );
    void  setRasterOp( Qt::RasterOp op );

    /**
     * Change logical Coordinate
     * some wmf files call those functions several times in the middle of a drawing
     * others wmf files doesn't call setWindow* at all
     * negative width and height are possible
     */
    void  setWindowOrg( int left, int top );
    void  setWindowExt( int width, int height );
    
    // Clipping
    // the 'CoordinateMode' is ommitted : always CoordPainter in wmf
    // setClipRegion() is often used with save() and restore() => implement all or none
    void  setClipRegion( const QRegion &rec );
    QRegion clipRegion();

    // Graphics drawing functions
    void  moveTo( int x, int y );
    void  lineTo( int x, int y );
    void  drawRect( int x, int y, int w, int h );
    void  drawRoundRect( int x, int y, int w, int h, int = 25, int = 25 );
    void  drawEllipse( int x, int y, int w, int h );
    void  drawArc( int x, int y, int w, int h, int a, int alen );
    void  drawPie( int x, int y, int w, int h, int a, int alen );
    void  drawChord( int x, int y, int w, int h, int a, int alen );
    void  drawPolyline( const Q3PointArray &pa );
    void  drawPolygon( const Q3PointArray &pa, bool winding=FALSE );
    /**
     * drawPolyPolygon draw the XOR of a list of polygons
     * listPa : list of polygons
     */
    void  drawPolyPolygon( Q3PtrList<Q3PointArray>& listPa, bool winding=FALSE );
    void  drawImage( int x, int y, const QImage &, int sx = 0, int sy = 0, int sw = -1, int sh = -1 );

    // Text drawing 
    // rotation = the degrees of rotation in counterclockwise
    // not yet implemented in KWinMetaFile
    void  drawText( int x, int y, int w, int h, int flags, const QString &s, double rotation );

    // matrix transformation : only used in some bitmap manipulation
    void  setMatrix( const QMatrix &, bool combine=FALSE );

    //-----------------------------------------------------------------------------
    // Utilities 
    // Add pen, brush and points to a path
    void appendPen( VObject& obj );
    void appendBrush( VObject& obj );
    void appendPoints(VPath& path, const Q3PointArray& pa);
    // coordinate transformation
    // translate wmf to (0,0) -> scale to document size -> translate to karbon (0,0)
    // Wmf origin is (left,top) corner
    // Karbon origin is (left,bottom) corner
    double coordX( int left ) 
            { return ((double)(left - mCurrentOrg.x()) * mScaleX); }
    double coordY( int top ) 
            { return (mDoc->height() - ((double)(top - mCurrentOrg.y()) * mScaleY)); }
    double scaleW( int width ) 
            { return (width * mScaleX); }
    double scaleH( int height ) 
            { return (height * mScaleY); }

private:
    VDocument *mDoc;
    QRegion   mClippingRegion;
    QPen      mPen;
    QBrush    mBrush;
    Qt::BGMode mBackgroundMode;
    QColor    mBackgroundColor;
    // current origin of WMF logical coordinate
    QPoint    mCurrentOrg;
    double    mScaleX;
    double    mScaleY;
    // current position in WMF coordinate (INT16)
    QPoint    mCurrentPoint;
};

#endif
