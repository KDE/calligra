/* This file is part of the KDE libraries
 * Copyright (c) 1998 Stefan Taferner
 *               2001/2003 thierry lorthiois (lorthioist@wanadoo.fr)
 *               2009-2010 Inge Wallin <inge@lysator.liu.se>
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

#include <QtGui/QColor>
#include <QtCore/QRect>

#include "kowmfenums.h"
#include "kowmfstruct.h"
#include "kowmfstack.h"

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
    KoWmfRead *mReadWmf;

    // The current window.  The union of all windows in the file is the bounding box.
    //
    // We can't use a QRect here because width/height may be negative -- see mBBox* above.
    qint16   mWindowTop;
    qint16   mWindowLeft;
    qint16   mWindowWidth;
    qint16   mWindowHeight;

    // Current state of the drawing
    WmfLayout   mLayout;
    QColor      mTextColor;
    quint16     mTextAlign;
    int         mTextRotation;
    bool        mWinding;

    // Memory allocation for WMF file
    QBuffer*  mBuffer;
    int    mOffsetFirstRecord;

    // stack of object handle
    KoWmfHandle**  mObjHandleTab;
    int    mNbrObject;          // number of object on the stack
    bool   mStackOverflow;
};

/**
 *  static data
 */
static const struct KoWmfFunc {
    void (KoWmfReadPrivate::*method)(quint32, QDataStream&);
    QString name;
} koWmfFunc[] = {
    //                                    index metafunc
    { &KoWmfReadPrivate::end, "end" }, // 0 0x00
    { &KoWmfReadPrivate::setBkColor, "setBkColor" }, // 1 0x01
    { &KoWmfReadPrivate::setBkMode, "setBkMode" }, // 2 0x02
    { &KoWmfReadPrivate::setMapMode, "setMapMode" }, // 3 0x03
    { &KoWmfReadPrivate::setRop, "setRop" }, // 4 0x04
    { &KoWmfReadPrivate::setRelAbs, "setRelAbs" }, // 5 0x05
    { &KoWmfReadPrivate::setPolyFillMode, "setPolyFillMode" }, // 6 0x06
    { &KoWmfReadPrivate::SetStretchBltMode, "SetStretchBltMode" }, // 7 0x07
    { &KoWmfReadPrivate::notyet, "notyet" }, // 8 0x08
    { &KoWmfReadPrivate::setTextColor, "setTextColor" }, // 9 0x09
    { &KoWmfReadPrivate::ScaleWindowExt, "ScaleWindowExt" }, // 10 0x0a
    { &KoWmfReadPrivate::setWindowOrg, "setWindowOrg" }, // 11 0x0b
    { &KoWmfReadPrivate::setWindowExt, "setWindowExt" }, // 12 0x0c
    { &KoWmfReadPrivate::setViewportOrg, "setViewportOrg" }, // 13 0x0d
    { &KoWmfReadPrivate::setViewportExt, "setViewportExt" }, // 14 0x0e
    { &KoWmfReadPrivate::OffsetWindowOrg, "OffsetWindowOrg" }, // 15 0x0f
    { &KoWmfReadPrivate::notyet, "notyet" }, // 16 0x10
    { &KoWmfReadPrivate::notyet, "notyet" }, // 17 0x11
    { &KoWmfReadPrivate::notyet, "notyet" }, // 18 0x12
    { &KoWmfReadPrivate::lineTo, "lineTo" }, // 19 0x13
    { &KoWmfReadPrivate::moveTo, "moveTo" }, // 20 0x14
    { &KoWmfReadPrivate::excludeClipRect, "excludeClipRect" }, // 21 0x15
    { &KoWmfReadPrivate::intersectClipRect, "intersectClipRect" }, // 22 0x16
    { &KoWmfReadPrivate::arc, "arc" }, // 23 0x17
    { &KoWmfReadPrivate::ellipse, "ellipse" }, // 24 0x18
    { &KoWmfReadPrivate::notyet, "notyet" }, // 25 0x19
    { &KoWmfReadPrivate::pie, "pie" }, // 26 0x1a
    { &KoWmfReadPrivate::rectangle, "rectangle" }, // 27 0x1b
    { &KoWmfReadPrivate::roundRect, "roundRect" }, // 28 0x1c
    { &KoWmfReadPrivate::patBlt, "patBlt" }, // 29 0x1d
    { &KoWmfReadPrivate::saveDC, "saveDC" }, // 30 0x1e
    { &KoWmfReadPrivate::setPixel, "setPixel" }, // 31 0x1f
    { &KoWmfReadPrivate::notyet, "notyet" }, // 32 0x20
    { &KoWmfReadPrivate::textOut, "textOut" }, // 33 0x21
    { &KoWmfReadPrivate::bitBlt, "bitBlt" }, // 34 0x22
    { &KoWmfReadPrivate::notyet, "notyet" }, // 35 0x23
    { &KoWmfReadPrivate::polygon, "polygon" }, // 36 0x24
    { &KoWmfReadPrivate::polyline, "polyline" }, // 37 0x25
    { &KoWmfReadPrivate::escape, "escape" }, // 38 0x26
    { &KoWmfReadPrivate::restoreDC, "restoreDC" }, // 39 0x27
    { &KoWmfReadPrivate::region, "region" }, // 40 0x28
    { &KoWmfReadPrivate::region, "region" }, // 41 0x29
    { &KoWmfReadPrivate::region, "region" }, // 42 0x2a
    { &KoWmfReadPrivate::region, "region" }, // 43 0x2b
    { &KoWmfReadPrivate::region, "region" }, // 44 0x2c
    { &KoWmfReadPrivate::selectObject, "selectObject" }, // 45 0x2d
    { &KoWmfReadPrivate::setTextAlign, "setTextAlign" }, // 46 0x2e
    { 0, "unimplemented" }, // 47 0x2f
    { &KoWmfReadPrivate::chord, "chord" }, // 48 0x30
    { &KoWmfReadPrivate::notyet, "notyet" }, // 49 0x31
    { &KoWmfReadPrivate::extTextOut, "extTextOut" }, // 50 0x32
    { &KoWmfReadPrivate::setDibToDev, "setDibToDev" }, // 51 0x33
    { &KoWmfReadPrivate::palette, "palette" }, // 52 0x34
    { &KoWmfReadPrivate::palette, "palette" }, // 53 0x35
    { &KoWmfReadPrivate::palette, "palette" }, // 54 0x36
    { &KoWmfReadPrivate::palette, "palette" }, // 55 0x37
    { &KoWmfReadPrivate::polyPolygon, "polyPolygon" }, // 56 0x38
    { &KoWmfReadPrivate::palette, "palette" }, // 57 0x39
    { 0, "unimplemented" }, // 58 0x3a
    { 0, "unimplemented" }, // 59 0x3b
    { 0, "unimplemented" }, // 60 0x3c
    { 0, "unimplemented" }, // 61 0x3d
    { 0, "unimplemented" }, // 62 0x3e
    { 0, "unimplemented" }, // 63 0x3f
    { &KoWmfReadPrivate::dibBitBlt, "dibBitBlt" }, // 64 0x40
    { &KoWmfReadPrivate::dibStretchBlt, "dibStretchBlt" }, // 65 0x41
    { &KoWmfReadPrivate::dibCreatePatternBrush, "dibCreatePatternBrush" }, // 66 0x42
    { &KoWmfReadPrivate::stretchDib, "stretchDib" }, // 67 0x43
    { 0, "unimplemented" }, // 68 0x44
    { 0, "unimplemented" }, // 69 0x45
    { 0, "unimplemented" }, // 70 0x46
    { 0, "unimplemented" }, // 71 0x47
    { &KoWmfReadPrivate::extFloodFill, "extFloodFill" }, // 72 0x48
    { &KoWmfReadPrivate::setLayout, "setLayout" }, // 73 0x49
    { 0, "unimplemented" }, // 74 0x4a
    { 0, "unimplemented" }, // 75 0x4b
    { &KoWmfReadPrivate::resetDC, "resetDC" }, // 76 0x4c
    { &KoWmfReadPrivate::startDoc, "startDoc" }, // 77 0x4d
    { 0, "unimplemented" }, // 78 0x4e
    { &KoWmfReadPrivate::startPage, "startPage" }, // 79 0x4f
    { &KoWmfReadPrivate::endPage, "endPage" }, // 80 0x50
    { 0, "unimplemented" }, // 81 0x51
    { 0, "unimplemented" }, // 82 0x52
    { 0, "unimplemented" }, // 83 0x53
    { 0, "unimplemented" }, // 84 0x54
    { 0, "unimplemented" }, // 85 0x55
    { 0, "unimplemented" }, // 86 0x56
    { 0, "unimplemented" }, // 87 0x57
    { 0, "unimplemented" }, // 88 0x58
    { 0, "unimplemented" }, // 89 0x59
    { 0, "unimplemented" }, // 90 0x5a
    { 0, "unimplemented" }, // 91 0x5b
    { 0, "unimplemented" }, // 92 0x5c
    { 0, "unimplemented" }, // 93 0x5d
    { &KoWmfReadPrivate::endDoc, "endDoc" }, // 94 0x5e
    { 0, "unimplemented" }, // 95 0x5f
    // 0x5F last function until 0xF0
    { &KoWmfReadPrivate::deleteObject, "deleteObject" }, // 96 0xf0
    { 0, "unimplemented" }, // 97 0xf1
    { 0, "unimplemented" }, // 98 0xf2
    { 0, "unimplemented" }, // 99 0xf3
    { 0, "unimplemented" }, // 100 0xf4
    { 0, "unimplemented" }, // 101 0xf5
    { 0, "unimplemented" }, // 102 0xf6
    { &KoWmfReadPrivate::createPalette, "createPalette" }, // 103 0xf7
    { &KoWmfReadPrivate::createBrush, "createBrush" }, // 104 0xf8
    { &KoWmfReadPrivate::createPatternBrush, "createPatternBrush" }, // 105 0xf9
    { &KoWmfReadPrivate::createPenIndirect, "createPenIndirect" }, // 106 0xfa
    { &KoWmfReadPrivate::createFontIndirect, "createFontIndirect" }, // 107 0xfb
    { &KoWmfReadPrivate::createBrushIndirect, "createBrushIndirect" }, //108 0xfc
    { &KoWmfReadPrivate::createBitmapIndirect, "createBitmapIndirect" }, //109 0xfd
    { &KoWmfReadPrivate::createBitmap, "createBitmap" }, // 110 0xfe
    { &KoWmfReadPrivate::createRegion, "createRegion" } // 111 0xff
};


#endif

