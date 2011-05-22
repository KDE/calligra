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
     * Plays a metafile in @p readKwmf and returns true on success.
     * To draw on a device you have to inherit the class ReadKwmf
     */
    bool play(WmfAbstractBackend* readKwmf);


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

/**
 *  static data
 */
static const struct KoWmfFunc {
    void (WmfParser::*method)(quint32, QDataStream&);
    QString name;
} koWmfFunc[] = {
    //                                    index metafunc
    { &WmfParser::end, "end" }, // 0 0x00
    { &WmfParser::setBkColor, "setBkColor" }, // 1 0x01
    { &WmfParser::setBkMode, "setBkMode" }, // 2 0x02
    { &WmfParser::setMapMode, "setMapMode" }, // 3 0x03
    { &WmfParser::setRop, "setRop" }, // 4 0x04
    { &WmfParser::setRelAbs, "setRelAbs" }, // 5 0x05
    { &WmfParser::setPolyFillMode, "setPolyFillMode" }, // 6 0x06
    { &WmfParser::SetStretchBltMode, "SetStretchBltMode" }, // 7 0x07
    { &WmfParser::notyet, "notyet" }, // 8 0x08
    { &WmfParser::setTextColor, "setTextColor" }, // 9 0x09
    { &WmfParser::notyet, "setTextJustification" }, // 10 0x0a
    { &WmfParser::setWindowOrg, "setWindowOrg" }, // 11 0x0b
    { &WmfParser::setWindowExt, "setWindowExt" }, // 12 0x0c
    { &WmfParser::setViewportOrg, "setViewportOrg" }, // 13 0x0d
    { &WmfParser::setViewportExt, "setViewportExt" }, // 14 0x0e
    { &WmfParser::OffsetWindowOrg, "OffsetWindowOrg" }, // 15 0x0f
    { &WmfParser::ScaleWindowExt, "ScaleWindowExt" }, // 16 0x10
    { &WmfParser::notyet, "notyet" }, // 17 0x11
    { &WmfParser::notyet, "ScaleViewportExt" }, // 18 0x12
    { &WmfParser::lineTo, "lineTo" }, // 19 0x13
    { &WmfParser::moveTo, "moveTo" }, // 20 0x14
    { &WmfParser::excludeClipRect, "excludeClipRect" }, // 21 0x15
    { &WmfParser::intersectClipRect, "intersectClipRect" }, // 22 0x16
    { &WmfParser::arc, "arc" }, // 23 0x17
    { &WmfParser::ellipse, "ellipse" }, // 24 0x18
    { &WmfParser::notyet, "notyet" }, // 25 0x19  floodfill
    { &WmfParser::pie, "pie" }, // 26 0x1a
    { &WmfParser::rectangle, "rectangle" }, // 27 0x1b
    { &WmfParser::roundRect, "roundRect" }, // 28 0x1c
    { &WmfParser::patBlt, "patBlt" }, // 29 0x1d
    { &WmfParser::saveDC, "saveDC" }, // 30 0x1e
    { &WmfParser::setPixel, "setPixel" }, // 31 0x1f
    { &WmfParser::notyet, "notyet" }, // 32 0x20
    { &WmfParser::textOut, "textOut" }, // 33 0x21
    { &WmfParser::bitBlt, "bitBlt" }, // 34 0x22
    { &WmfParser::notyet, "notyet" }, // 35 0x23
    { &WmfParser::polygon, "polygon" }, // 36 0x24
    { &WmfParser::polyline, "polyline" }, // 37 0x25
    { &WmfParser::escape, "escape" }, // 38 0x26
    { &WmfParser::restoreDC, "restoreDC" }, // 39 0x27
    { &WmfParser::region, "region" }, // 40 0x28
    { &WmfParser::region, "region" }, // 41 0x29
    { &WmfParser::region, "region" }, // 42 0x2a
    { &WmfParser::region, "region" }, // 43 0x2b
    { &WmfParser::region, "region" }, // 44 0x2c
    { &WmfParser::selectObject, "selectObject" }, // 45 0x2d
    { &WmfParser::setTextAlign, "setTextAlign" }, // 46 0x2e
    { 0, "unimplemented" }, // 47 0x2f
    { &WmfParser::chord, "chord" }, // 48 0x30
    { &WmfParser::notyet, "notyet" }, // 49 0x31  setmapperflags
    { &WmfParser::extTextOut, "extTextOut" }, // 50 0x32
    { &WmfParser::setDibToDev, "setDibToDev" }, // 51 0x33
    { &WmfParser::palette, "palette" }, // 52 0x34
    { &WmfParser::palette, "palette" }, // 53 0x35
    { &WmfParser::palette, "palette" }, // 54 0x36
    { &WmfParser::palette, "palette" }, // 55 0x37
    { &WmfParser::polyPolygon, "polyPolygon" }, // 56 0x38
    { &WmfParser::palette, "palette" }, // 57 0x39
    { 0, "unimplemented" }, // 58 0x3a
    { 0, "unimplemented" }, // 59 0x3b
    { 0, "unimplemented" }, // 60 0x3c
    { 0, "unimplemented" }, // 61 0x3d
    { 0, "unimplemented" }, // 62 0x3e
    { 0, "unimplemented" }, // 63 0x3f
    { &WmfParser::dibBitBlt, "dibBitBlt" }, // 64 0x40
    { &WmfParser::dibStretchBlt, "dibStretchBlt" }, // 65 0x41
    { &WmfParser::dibCreatePatternBrush, "dibCreatePatternBrush" }, // 66 0x42
    { &WmfParser::stretchDib, "stretchDib" }, // 67 0x43
    { 0, "unimplemented" }, // 68 0x44
    { 0, "unimplemented" }, // 69 0x45
    { 0, "unimplemented" }, // 70 0x46
    { 0, "unimplemented" }, // 71 0x47
    { &WmfParser::extFloodFill, "extFloodFill" }, // 72 0x48
    { &WmfParser::setLayout, "setLayout" }, // 73 0x49
    { 0, "unimplemented" }, // 74 0x4a
    { 0, "unimplemented" }, // 75 0x4b
    { &WmfParser::resetDC, "resetDC" }, // 76 0x4c
    { &WmfParser::startDoc, "startDoc" }, // 77 0x4d
    { 0, "unimplemented" }, // 78 0x4e
    { &WmfParser::startPage, "startPage" }, // 79 0x4f
    { &WmfParser::endPage, "endPage" }, // 80 0x50
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
    { &WmfParser::endDoc, "endDoc" }, // 94 0x5e
    { 0, "unimplemented" }, // 95 0x5f
    { &WmfParser::deleteObject, "deleteObject" }, // 96 0xf0
    { 0, "unimplemented" }, // 97 0xf1
    { 0, "unimplemented" }, // 98 0xf2
    { 0, "unimplemented" }, // 99 0xf3
    { 0, "unimplemented" }, // 100 0xf4
    { 0, "unimplemented" }, // 101 0xf5
    { 0, "unimplemented" }, // 102 0xf6
    { &WmfParser::createPalette, "createPalette" }, // 103 0xf7
    { &WmfParser::createBrush, "createBrush" }, // 104 0xf8
    { &WmfParser::createPatternBrush, "createPatternBrush" }, // 105 0xf9
    { &WmfParser::createPenIndirect, "createPenIndirect" }, // 106 0xfa
    { &WmfParser::createFontIndirect, "createFontIndirect" }, // 107 0xfb
    { &WmfParser::createBrushIndirect, "createBrushIndirect" }, //108 0xfc
    { &WmfParser::createBitmapIndirect, "createBitmapIndirect" }, //109 0xfd
    { &WmfParser::createBitmap, "createBitmap" }, // 110 0xfe
    { &WmfParser::createRegion, "createRegion" } // 111 0xff
};


}

#endif

