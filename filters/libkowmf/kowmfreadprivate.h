/* This file is part of the KDE libraries
 * Copyright (c) 1998 Stefan Taferner
 *                       2001/2003 thierry lorthiois (lorthioist@wanadoo.fr)
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
#ifndef _KOWMFREADPRIVATE_H_
#define _KOWMFREADPRIVATE_H_

#include "kowmfstruct.h"
#include "kowmfstack.h"

#include <QtGui/QColor>
#include <QtCore/QRect>

class KoWmfRead;
class QBuffer;
class QPolygon;

/**
 * KoWmfReadPrivate allows to read WMF files
 *
 */

class KoWmfReadPrivate
{
public:
    // Taken from the WMF spec.
    typedef enum {
        LAYOUT_LTR = 0x0000,
        LAYOUT_RTL = 0x0001,
        LAYOUT_BITMAPORIENTATIONPRESERVED = 0x0008
    } Layout;

    KoWmfReadPrivate();
    virtual ~KoWmfReadPrivate();

    /**
     * Load WMF file. Returns true on success.
     */
    bool load(const QByteArray& array);

    /**
     * Plays a metafile in @p readKwmf and returns true on success.
     * To draw on a device you have to inherit the class ReadKwmf
     */
    bool play(KoWmfRead* readKwmf);


    /* Metafile painter methods */

    /// set window origin
    void setWindowOrg(quint32 size, QDataStream& stream);
    /// set window extents
    void setWindowExt(quint32 size, QDataStream& stream);
    /// scale window extents
    void ScaleWindowExt(quint32, QDataStream& stream);
    /// offset in window origin
    void OffsetWindowOrg(quint32, QDataStream& stream);

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

    /// bounding rectangle
    QRect  mBBox;     // placeable file : this is the header
    // standard file : this is the value in setWindowOrg and setWindowExt
    /// number of points per inch for the default size
    int mDpi;

    /// number of functions to draw (==0 for all)
    int mNbrFunc;

private:
    // the output
    KoWmfRead *mReadWmf;

    // current coordinate != mBBox
    QRect  mWindow;
    // current state of the drawing
    Layout  mLayout;
    QColor  mTextColor;
    quint16  mTextAlign;
    int     mTextRotation;
    bool    mWinding;

    // memory allocation for WMF file
    QBuffer*  mBuffer;
    int    mOffsetFirstRecord;

    // stack of object handle
    KoWmfHandle**  mObjHandleTab;
    // number of object on the stack
    int    mNbrObject;
    bool   mStackOverflow;
};

/**
 *  static data
 */
static const struct KoWmfFunc {
    void (KoWmfReadPrivate::*method)(quint32, QDataStream&);
    QString name;
} koWmfFunc[] = {
    { &KoWmfReadPrivate::end, "end" }, // 0
    { &KoWmfReadPrivate::setBkColor, "setBkColor" }, // 1
    { &KoWmfReadPrivate::setBkMode, "setBkMode" }, // 2
    { &KoWmfReadPrivate::setMapMode, "setMapMode" }, // 3
    { &KoWmfReadPrivate::setRop, "setRop" }, // 4
    { &KoWmfReadPrivate::setRelAbs, "setRelAbs" }, // 5
    { &KoWmfReadPrivate::setPolyFillMode, "setPolyFillMode" }, // 6
    { &KoWmfReadPrivate::SetStretchBltMode, "SetStretchBltMode" }, // 7
    { &KoWmfReadPrivate::notyet, "notyet" }, // 8
    { &KoWmfReadPrivate::setTextColor, "setTextColor" }, // 9
    { &KoWmfReadPrivate::ScaleWindowExt, "ScaleWindowExt" }, // 10
    { &KoWmfReadPrivate::setWindowOrg, "setWindowOrg" }, // 11
    { &KoWmfReadPrivate::setWindowExt, "setWindowExt" }, // 12
    { &KoWmfReadPrivate::notyet, "notyet" }, // 13
    { &KoWmfReadPrivate::notyet, "notyet" }, // 14
    { &KoWmfReadPrivate::OffsetWindowOrg, "OffsetWindowOrg" }, // 15
    { &KoWmfReadPrivate::notyet, "notyet" }, // 16
    { &KoWmfReadPrivate::notyet, "notyet" }, // 17
    { &KoWmfReadPrivate::notyet, "notyet" }, // 18
    { &KoWmfReadPrivate::lineTo, "lineTo" }, // 19
    { &KoWmfReadPrivate::moveTo, "moveTo" }, // 20
    { &KoWmfReadPrivate::excludeClipRect, "excludeClipRect" }, // 21
    { &KoWmfReadPrivate::intersectClipRect, "intersectClipRect" }, // 22
    { &KoWmfReadPrivate::arc, "arc" }, // 23
    { &KoWmfReadPrivate::ellipse, "ellipse" }, // 24
    { &KoWmfReadPrivate::notyet, "notyet" }, // 25
    { &KoWmfReadPrivate::pie, "pie" }, // 26
    { &KoWmfReadPrivate::rectangle, "rectangle" }, // 27
    { &KoWmfReadPrivate::roundRect, "roundRect" }, // 28
    { &KoWmfReadPrivate::notyet, "notyet" }, // 29
    { &KoWmfReadPrivate::saveDC, "saveDC" }, // 30
    { &KoWmfReadPrivate::setPixel, "setPixel" }, // 31
    { &KoWmfReadPrivate::notyet, "notyet" }, // 32
    { &KoWmfReadPrivate::textOut, "textOut" }, // 33
    { &KoWmfReadPrivate::bitBlt, "bitBlt" }, // 34
    { &KoWmfReadPrivate::notyet, "notyet" }, // 35
    { &KoWmfReadPrivate::polygon, "polygon" }, // 36
    { &KoWmfReadPrivate::polyline, "polyline" }, // 37
    { &KoWmfReadPrivate::escape, "escape" }, // 38
    { &KoWmfReadPrivate::restoreDC, "restoreDC" }, // 39
    { &KoWmfReadPrivate::region, "region" }, // 40
    { &KoWmfReadPrivate::region, "region" }, // 41
    { &KoWmfReadPrivate::region, "region" }, // 42
    { &KoWmfReadPrivate::region, "region" }, // 43
    { &KoWmfReadPrivate::region, "region" }, // 44
    { &KoWmfReadPrivate::selectObject, "selectObject" }, // 45
    { &KoWmfReadPrivate::setTextAlign, "setTextAlign" }, // 46
    { 0, "unimplemented" }, // 47
    { &KoWmfReadPrivate::chord, "chord" }, // 48
    { &KoWmfReadPrivate::notyet, "notyet" }, // 49
    { &KoWmfReadPrivate::extTextOut, "extTextOut" }, // 50
    { &KoWmfReadPrivate::setDibToDev, "setDibToDev" }, // 51
    { &KoWmfReadPrivate::palette, "palette" }, // 52
    { &KoWmfReadPrivate::palette, "palette" }, // 53
    { &KoWmfReadPrivate::palette, "palette" }, // 54
    { &KoWmfReadPrivate::palette, "palette" }, // 55
    { &KoWmfReadPrivate::polyPolygon, "polyPolygon" }, // 56
    { &KoWmfReadPrivate::palette, "palette" }, // 57
    { 0, "unimplemented" }, // 58
    { 0, "unimplemented" }, // 59
    { 0, "unimplemented" }, // 60
    { 0, "unimplemented" }, // 61
    { 0, "unimplemented" }, // 62
    { 0, "unimplemented" }, // 63
    { &KoWmfReadPrivate::dibBitBlt, "dibBitBlt" }, // 64
    { &KoWmfReadPrivate::dibStretchBlt, "dibStretchBlt" }, // 65
    { &KoWmfReadPrivate::dibCreatePatternBrush, "dibCreatePatternBrush" }, // 66
    { &KoWmfReadPrivate::stretchDib, "stretchDib" }, // 67
    { 0, "unimplemented" }, // 68
    { 0, "unimplemented" }, // 69
    { 0, "unimplemented" }, // 70
    { 0, "unimplemented" }, // 71
    { &KoWmfReadPrivate::extFloodFill, "extFloodFill" }, // 72
    { &KoWmfReadPrivate::setLayout, "setLayout" }, // 73
    { 0, "unimplemented" }, // 74
    { 0, "unimplemented" }, // 75
    { &KoWmfReadPrivate::resetDC, "resetDC" }, // 76
    { &KoWmfReadPrivate::startDoc, "startDoc" }, // 77
    { 0, "unimplemented" }, // 78
    { &KoWmfReadPrivate::startPage, "startPage" }, // 79
    { &KoWmfReadPrivate::endPage, "endPage" }, // 80
    { 0, "unimplemented" }, // 81
    { 0, "unimplemented" }, // 82
    { 0, "unimplemented" }, // 83
    { 0, "unimplemented" }, // 84
    { 0, "unimplemented" }, // 85
    { 0, "unimplemented" }, // 86
    { 0, "unimplemented" }, // 87
    { 0, "unimplemented" }, // 88
    { 0, "unimplemented" }, // 89
    { 0, "unimplemented" }, // 90
    { 0, "unimplemented" }, // 91
    { 0, "unimplemented" }, // 92
    { 0, "unimplemented" }, // 93
    { &KoWmfReadPrivate::endDoc, "endDoc" }, // 94
    { 0, "unimplemented" }, // 95
    // 0x5F last function until 0xF0
    { &KoWmfReadPrivate::deleteObject, "deleteObject" }, // 96
    { 0, "unimplemented" }, // 97
    { 0, "unimplemented" }, // 98
    { 0, "unimplemented" }, // 99
    { 0, "unimplemented" }, // 100
    { 0, "unimplemented" }, // 101
    { 0, "unimplemented" }, // 102
    { &KoWmfReadPrivate::createPalette, "createPalette" }, // 103
    { &KoWmfReadPrivate::createBrush, "createBrush" }, // 104
    { &KoWmfReadPrivate::createPatternBrush, "createPatternBrush" }, // 105
    { &KoWmfReadPrivate::createPenIndirect, "createPenIndirect" }, // 106
    { &KoWmfReadPrivate::createFontIndirect, "createFontIndirect" }, // 107
    { &KoWmfReadPrivate::createBrushIndirect, "createBrushIndirect" }, //108
    { &KoWmfReadPrivate::createBitmapIndirect, "createBitmapIndirect" }, //109
    { &KoWmfReadPrivate::createBitmap, "createBitmap" }, // 110
    { &KoWmfReadPrivate::createRegion, "createRegion" } // 111
};


#endif

