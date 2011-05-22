/* This file is part of the KDE libraries
 *
 * Copyright (c) 1998 Stefan Taferner
 *               2001/2003 thierry lorthiois (lorthioist@wanadoo.fr)
 *               2009-2011 Inge Wallin <inge@lysator.liu.se>
 * With the help of WMF documentation by Caolan Mc Namara

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/
#ifndef _WMFPARSER_H_
#define _WMFPARSER_H_

#include <QtGui/QColor>
#include <QtCore/QRect>

#include "WmfEnums.h"
#include "WmfStruct.h"
#include "WmfStack.h"

class WmfAbstractBackend;
class QBuffer;
class QPolygon;

/**
   Namespace for Windows Metafile (WMF) classes
*/
namespace Libwmf
{


/**
 * WmfParser allows to read WMF files
 *
 */

class WmfParser
{
public:
    WmfParser();
    virtual ~WmfParser();

    /**
     * Load WMF file. Returns true on success.
     */
    bool load(const QByteArray& array);

    /**
     * Plays a metafile using @p backend as backend and returns true on success.
     * To draw on a device you have to inherit the class WmfAbstractBackend.
     */
    bool play(WmfAbstractBackend* backend);


    /* Metafile painter methods */

    /// set window origin
    void setWindowOrg(quint32 size, QDataStream& stream);
    /// set window extents
    void setWindowExt(quint32 size, QDataStream& stream);
    /// scale window extents
    void ScaleWindowExt(quint32, QDataStream& stream);
    /// offset in window origin
    void OffsetWindowOrg(quint32, QDataStream& stream);
    /// set viewport origin
    void setViewportOrg(quint32 size, QDataStream& stream);
    /// set viewport extents
    void setViewportExt(quint32 size, QDataStream& stream);

    /****************** Drawing *******************/
    /// draw line to coord
    void lineTo(quint32 size, QDataStream& stream);
    /// move pen to coord
    void moveTo(quint32 size, QDataStream& stream);
    /// draw ellipse
    void ellipse(quint32 size, QDataStream& stream);
    /// draw polygon
    void polygon(quint32 size, QDataStream& stream);
    /// draw a list of polygons
    void polyPolygon(quint32 size, QDataStream& stream);
    /// draw series of lines
    void polyline(quint32 size, QDataStream& stream);
    /// draw a rectangle
    void rectangle(quint32 size, QDataStream& stream);
    /// draw round rectangle
    void roundRect(quint32 size, QDataStream& stream);
    /// draw arc
    void arc(quint32 size, QDataStream& stream);
    /// draw chord
    void chord(quint32 size, QDataStream& stream);
    /// draw pie
    void pie(quint32 size, QDataStream& stream);
    /// set polygon fill mode
    void setPolyFillMode(quint32 size, QDataStream& stream);
    /// set background pen color
    void setBkColor(quint32 size, QDataStream& stream);
    /// set background pen mode
    void setBkMode(quint32 size, QDataStream& stream);
    /// set a pixel
    void setPixel(quint32 size, QDataStream& stream);
    /// set raster operation mode
    void setRop(quint32 size, QDataStream& stream);
    /// save device context */
    void saveDC(quint32 size, QDataStream& stream);
    /// restore device context
    void restoreDC(quint32 size, QDataStream& stream);
    ///  clipping region is the intersection of this region and the original region
    void intersectClipRect(quint32 size, QDataStream& stream);
    /// delete a clipping rectangle of the original region
    void excludeClipRect(quint32 size, QDataStream& stream);

    /****************** Text *******************/
    /// set text color
    void setTextColor(quint32 size, QDataStream& stream);
    /// set text alignment
    void setTextAlign(quint32 size, QDataStream& stream);
    /// draw text
    void textOut(quint32 size, QDataStream& stream);
    void extTextOut(quint32 size, QDataStream& stream);

    /****************** Bitmap *******************/
    void SetStretchBltMode(quint32, QDataStream& stream);
    /// copies a DIB into a dest location
    void dibBitBlt(quint32 size, QDataStream& stream);
    /// stretches a DIB into a dest location
    void dibStretchBlt(quint32 size, QDataStream& stream);
    void stretchDib(quint32 size, QDataStream& stream);
    /// create a pattern brush
    void dibCreatePatternBrush(quint32 size, QDataStream& stream);
    /// ternary bitmap operation
    void patBlt(quint32 size, QDataStream& stream);

    /****************** Object handle *******************/
    /// activate object handle
    void selectObject(quint32 size, QDataStream& stream);
    /// free object handle
    void deleteObject(quint32 size, QDataStream& stream);
    /// create an empty object in the object list
    void createEmptyObject();
    /// create a logical brush
    void createBrushIndirect(quint32 size, QDataStream& stream);
    /// create a logical pen
    void createPenIndirect(quint32 size, QDataStream& stream);
    /// create a logical font
    void createFontIndirect(quint32 size, QDataStream& stream);

    /****************** misc *******************/
    /// end of meta file
    void end(quint32, QDataStream& stream);

    /** Calculate header checksum */
    static quint16 calcCheckSum(WmfPlaceableHeader*);

    // function unimplemented
    void notyet(quint32, QDataStream& stream);
    void region(quint32, QDataStream& stream);
    void palette(quint32, QDataStream& stream);
    void escape(quint32, QDataStream& stream);
    void setRelAbs(quint32, QDataStream& stream);
    void setMapMode(quint32, QDataStream& stream);
    void extFloodFill(quint32, QDataStream& stream);
    void setLayout(quint32, QDataStream& stream);
    void startDoc(quint32, QDataStream& stream);
    void startPage(quint32, QDataStream& stream);
    void endDoc(quint32, QDataStream& stream);
    void endPage(quint32, QDataStream& stream);
    void resetDC(quint32, QDataStream& stream);
    void bitBlt(quint32, QDataStream& stream);
    void setDibToDev(quint32, QDataStream& stream);
    void createBrush(quint32, QDataStream& stream);
    void createPatternBrush(quint32, QDataStream& stream);
    void createBitmap(quint32, QDataStream& stream);
    void createBitmapIndirect(quint32, QDataStream& stream);
    void createPalette(quint32, QDataStream& stream);
    void createRegion(quint32, QDataStream& stream);

private:
    //-----------------------------------------------------------------------------
    // Utilities and conversion Wmf -> Qt

    // Create a boundingbox from all set{Window,Viewport}{Org,Ext} records.
    void createBoundingBox(QDataStream &st);

    /** Handle win-object-handles */
    bool addHandle(KoWmfHandle*);
    void deleteHandle(int);

    /** Convert QINT16 points into QPointArray */
    void pointArray(QDataStream& stream, QPolygon& pa);

    /** Convertion between windows color and QColor */
    QColor qtColor(quint32 color) const {
        return QColor(color & 0xFF, (color >> 8) & 0xFF, (color >> 16) & 0xFF);
    }

    /** Convert (x1,y1) and (x2, y2) positions in angle and angleLength */
    void xyToAngle(int xStart, int yStart, int xEnd, int yEnd, int& angle, int& aLength);

    /** Convert windows rasterOp in QT rasterOp */
    QPainter::CompositionMode winToQtComposition(quint16 param) const;
    QPainter::CompositionMode winToQtComposition(quint32 param) const;

    /** Converts DIB to BMP */
    bool dibToBmp(QImage& bmp, QDataStream& stream, quint32 size);


public:
    // state of the WMF
    bool mValid;
    bool mStandard;
    bool mPlaceable;
    bool mEnhanced;

    // Bounding rectangle.  In a placeable file this is in the header,
    // otherwise its comprised of calls to setWindowOrg and setWindowExt.
    //
    // We can't use a QRect here because width and/or height may be negative.
    qint16 mBBoxTop;
    qint16 mBBoxLeft;
    qint16 mBBoxRight;
    qint16 mBBoxBottom;
    qint16 mMaxWidth;
    qint16 mMaxHeight;

    // standard file : this is the value in setWindowOrg and setWindowExt
    // number of points per inch for the default size
    int mDpi;

    /// number of functions to draw (==0 for all)
    int mNbrFunc;

private:
    // the output strategy
    WmfAbstractBackend *m_backend;

    // The current window.  The union of all windows in the file is the bounding box.
    //
    // We can't use a QRect here because width/height may be negative -- see mBBox* above.
    qint16   mWindowLeft;
    qint16   mWindowTop;
    qint16   mWindowWidth;
    qint16   mWindowHeight;
    qint16   mViewportLeft;
    qint16   mViewportTop;
    qint16   mViewportWidth;
    qint16   mViewportHeight;

    // Current state of the drawing
    WmfLayout   mLayout;
    QColor      mTextColor;
    quint16     mTextAlign;
    int         mTextRotation;
    bool        mWinding;
    quint16     mMapMode;

    // Memory allocation for WMF file
    QBuffer*  mBuffer;
    int    mOffsetFirstRecord;

    // stack of object handle
    KoWmfHandle**  mObjHandleTab;
    int    mNbrObject;          // number of object on the stack
    bool   mStackOverflow;
};


}

#endif

