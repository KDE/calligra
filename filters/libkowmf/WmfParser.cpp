/* This file is part of the KDE libraries
 *
 * Copyright (c) 1998 Stefan Taferner
 *               2001/2003 thierry lorthiois (lorthioist@wanadoo.fr)
 *               2007-2008 Jan Hambrecht <jaham@gmx.net>
 *               2009-2011 Inge Wallin <inge@lysator.liu.se>
 * With the help of WMF documentation by Caolan Mc Namara
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

#include "WmfParser.h"
#include "WmfAbstractBackend.h"

#include <kdebug.h>

#include <QtGui/QImage>
#include <QtGui/QMatrix>
#include <QtCore/QDataStream>
#include <QtCore/QByteArray>
#include <QtCore/QBuffer>
#include <QtGui/QPolygon>

#include <math.h>


#define DEBUG_BBOX    0
#define DEBUG_RECORDS 0


/**
   Namespace for Windows Metafile (WMF) classes
*/
namespace Libwmf
{


// This table is used to call the correct parse function for each record.
//
// FIXME: This should be inline instead so that we don't have to
//        define so very many unnecessary functions.

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


// ----------------------------------------------------------------


WmfParser::WmfParser()
{
    mNbrFunc = 0;
    mValid = false;
    mStandard = false;
    mPlaceable = false;
    mEnhanced = false;
    mBuffer = 0;
    mObjHandleTab = 0;
}


WmfParser::~WmfParser()
{
    if (mObjHandleTab != 0) {
        for (int i = 0 ; i < mNbrObject ; i++) {
            if (mObjHandleTab[i] != 0)
                delete mObjHandleTab[i];
        }
        delete[] mObjHandleTab;
    }
    if (mBuffer != 0) {
        mBuffer->close();
        delete mBuffer;
    }
}


bool WmfParser::load(const QByteArray& array)
{
    // delete previous buffer
    if (mBuffer != 0) {
        mBuffer->close();
        delete mBuffer;
        mBuffer = 0;
    }

    if (array.size() == 0)
        return false;

    // load into buffer
    mBuffer = new QBuffer();
    mBuffer->setData(array);
    mBuffer->open(QIODevice::ReadOnly);

    // read and check the header
    WmfMetaHeader      header;
    WmfEnhMetaHeader   eheader;
    WmfPlaceableHeader pheader; // Contains a bounding box
    unsigned short checksum;
    int filePos;

    QDataStream st(mBuffer);
    st.setByteOrder(QDataStream::LittleEndian);
    mStackOverflow = false;
    mLayout = LAYOUT_LTR;
    mTextAlign = 0;
    mTextRotation = 0;
    mTextColor = Qt::black;
    mWinding = false;
    mMapMode = MM_ANISOTROPIC;

    mValid = false;
    mStandard = false;
    mPlaceable = false;
    mEnhanced = false;

    // Initialize the bounding box.
    //mBBoxTop = 0;           // The default origin is (0, 0).
    //mBBoxLeft = 0;
    mBBoxTop = 32767;
    mBBoxLeft = 32767;
    mBBoxRight = -32768;
    mBBoxBottom = -32768;
    mMaxWidth = 0;
    mMaxHeight = 0;

#if DEBUG_RECORDS
    kDebug(31000) << "--------------------------- Starting parsing WMF ---------------------------";
#endif
    st >> pheader.key;
    if (pheader.key == (quint32)APMHEADER_KEY) {
        //----- Read placeable metafile header
        mPlaceable = true;
#if DEBUG_RECORDS
        kDebug(31000) << "Placeable header!  Yessss!";
#endif

        st >> pheader.handle;
        st >> pheader.left;
        st >> pheader.top;
        st >> pheader.right;
        st >> pheader.bottom;
        st >> pheader.inch;
        st >> pheader.reserved;
        st >> pheader.checksum;
        checksum = calcCheckSum(&pheader);
        if (pheader.checksum != checksum) {
            kWarning() << "Checksum for placeable metafile header is incorrect ( actual checksum" << pheader.checksum << ", expected checksum" << checksum << ")";
            return false;
        }
        st >> header.fileType;
        st >> header.headerSize;
        st >> header.version;
        st >> header.fileSize;
        st >> header.numOfObjects;
        st >> header.maxRecordSize;
        st >> header.numOfParameters;

        mNbrObject = header.numOfObjects;

        // The bounding box of the WMF
        mBBoxLeft   = pheader.left;
        mBBoxTop    = pheader.top;
        mBBoxRight  = pheader.right;
        mBBoxBottom = pheader.bottom;
#if DEBUG_RECORDS
        kDebug(31000) << "bounding box in header: " << mBBoxLeft << mBBoxTop << mBBoxRight << mBBoxBottom
                      << "width, height: " << mBBoxRight - mBBoxLeft << mBBoxBottom - mBBoxTop;
#endif
        mMaxWidth   = abs(pheader.right - pheader.left);
        mMaxHeight  = abs(pheader.bottom - pheader.top);

        mDpi = pheader.inch;
    } else {
        mBuffer->reset();
        //----- Read as enhanced metafile header
        filePos = mBuffer->pos();
        st >> eheader.recordType;
        st >> eheader.recordSize;
        st >> eheader.boundsLeft;
        st >> eheader.boundsTop;
        st >> eheader.boundsRight;
        st >> eheader.boundsBottom;
        st >> eheader.frameLeft;
        st >> eheader.frameTop;
        st >> eheader.frameRight;
        st >> eheader.frameBottom;

        st >> eheader.signature;
        if (eheader.signature == ENHMETA_SIGNATURE) {
            mEnhanced = true;
            st >> eheader.version;
            st >> eheader.size;
            st >> eheader.numOfRecords;
            st >> eheader.numHandles;
            st >> eheader.reserved;
            st >> eheader.sizeOfDescription;
            st >> eheader.offsetOfDescription;
            st >> eheader.numPaletteEntries;
            st >> eheader.widthDevicePixels;
            st >> eheader.heightDevicePixels;
            st >> eheader.widthDeviceMM;
            st >> eheader.heightDeviceMM;
        } else {
            //----- Read as standard metafile header
            mStandard = true;
            mBuffer->seek(filePos);
            st >> header.fileType;
            st >> header.headerSize;
            st >> header.version;
            st >> header.fileSize;
            st >> header.numOfObjects;
            st >> header.maxRecordSize;
            st >> header.numOfParameters;
            mNbrObject = header.numOfObjects;
        }
    }
    mOffsetFirstRecord = mBuffer->pos();

    //----- Test header validity
    if (((header.headerSize == 9) && (header.numOfParameters == 0)) || (mPlaceable)) {
        // valid wmf file
        mValid = true;
    } else {
        kDebug(31000) << "WmfParser : incorrect file format !";
    }

    // check bounding rectangle for standard meta file
    if (mStandard && mValid) {
        // Note that this call can change mValid.
        createBoundingBox(st);

#if DEBUG_RECORDS
        kDebug(31000) << "bounding box created by going through all records: "
                      << mBBoxLeft << mBBoxTop << mBBoxRight << mBBoxBottom
                      << "width, height: " << mBBoxRight - mBBoxLeft << mBBoxBottom - mBBoxTop;
#endif
    }

    return mValid;
}


bool WmfParser::play(WmfAbstractBackend* backend)
{
    if (!(mValid)) {
        kDebug(31000) << "WmfParser::play : invalid WMF file";
        return false;
    }

    if (mNbrFunc) {
#if DEBUG_RECORDS
        if ((mStandard)) {
            kDebug(31000) << "Standard :" << mBBoxLeft << ""  << mBBoxTop << ""  << mBBoxRight - mBBoxLeft << ""  << mBBoxBottom - mBBoxTop;
        } else {
            kDebug(31000) << "DPI :" << mDpi;
            kDebug(31000) << "inch :" << (mBBoxRight - mBBoxLeft) / mDpi
                          << "" << (mBBoxBottom - mBBoxTop) / mDpi;
            kDebug(31000) << "mm :" << (mBBoxRight - mBBoxLeft) * 25.4 / mDpi
                          << "" << (mBBoxBottom - mBBoxTop) * 25.4 / mDpi;
        }
        kDebug(31000) << mValid << "" << mStandard << "" << mPlaceable;
#endif
    }

    // Stack of handles
    mObjHandleTab = new KoWmfHandle* [ mNbrObject ];
    for (int i = 0; i < mNbrObject ; i++) {
        mObjHandleTab[ i ] = 0;
    }

    quint16 numFunction;
    quint32 size;
    int  bufferOffset, j;

    // buffer with functions
    QDataStream st(mBuffer);
    st.setByteOrder(QDataStream::LittleEndian);

    // Set the output strategy.
    m_backend = backend;

    // Set some initial values.
    mWindowTop    = 0;
    mWindowLeft   = 0;
    mWindowWidth  = 1;
    mWindowHeight = 1;

    QRect bbox(QPoint(mBBoxLeft,mBBoxTop),
               QSize(mBBoxRight - mBBoxLeft, mBBoxBottom - mBBoxTop));
    if (m_backend->begin(bbox)) {
        // play wmf functions
        mBuffer->seek(mOffsetFirstRecord);
        numFunction = j = 1;
        mWinding = false;

        while ((numFunction) && (!mStackOverflow)) {
            bufferOffset = mBuffer->pos();
            st >> size >> numFunction;
            
            // mapping between n function and index of table 'metaFuncTab'
            // lower 8 digits of the function => entry in the table
            quint16 index = numFunction & 0xFF;
            if (index > 0x5F) {
                index -= 0x90;
            }
            
#if DEBUG_RECORDS
            kDebug(31000) << "Record = " << koWmfFunc[ index ].name
                          << " (" << hex << numFunction
                          << ", index" << dec << index << ")";
#endif

            if ((index > 111) || (koWmfFunc[ index ].method == 0)) {
                // function outside WMF specification
                kError(31000) << "BROKEN WMF file: Record number" << hex << numFunction << dec
                              << " index " << index;
                mValid = false;
                break;
            }

            if (mNbrFunc) {
                // debug mode
                if ((j + 12) > mNbrFunc) {
                    // output last 12 functions
                    int offBuff = mBuffer->pos();
                    quint16 param;

                    kDebug(31000) <<  j << " :" << index << " :";
                    for (quint16 i = 0 ; i < (size - 3) ; i++) {
                        st >> param;
                        kDebug(31000) <<  param << "";
                    }
                    kDebug(31000);
                    mBuffer->seek(offBuff);
                }
                if (j >= mNbrFunc) {
                    break;
                }
                j++;
            }

            // Execute the function.
            (this->*koWmfFunc[ index ].method)(size, st);

            mBuffer->seek(bufferOffset + (size << 1));
        }

        // Let the backend clean up it's internal state.
        m_backend->end();
    }

    for (int i = 0 ; i < mNbrObject ; i++) {
        if (mObjHandleTab[ i ] != 0)
            delete mObjHandleTab[ i ];
    }
    delete[] mObjHandleTab;
    mObjHandleTab = 0;

    return true;
}


//-----------------------------------------------------------------------------


void WmfParser::createBoundingBox(QDataStream &st)
{
    // Check bounding rectangle for standard meta file.
    // This calculation is done in device coordinates.
    if (!mStandard || !mValid) 
        return;

    bool windowExtIsSet = false;
    bool viewportExtIsSet = false;

    quint16 numFunction = 1;
    quint32 size;

    int filePos;

    // Search for records setWindowOrg and setWindowExt to
    // determine what the total bounding box of this WMF is.
    // This initialization assumes that setWindowOrg comes before setWindowExt.
    qint16 windowOrgX = 0;
    qint16 windowOrgY = 0;
    qint16 windowWidth = 0;
    qint16 windowHeight = 0;
    qint16 viewportOrgX = 0;
    qint16 viewportOrgY = 0;
    qint16 viewportWidth = 0;
    qint16 viewportHeight = 0;
    bool   bboxRecalculated = false;
    while (numFunction) {

        filePos = mBuffer->pos();
        st >> size >> numFunction;

        if (size == 0) {
            kDebug(31000) << "WmfParser: incorrect file!";
            mValid = 0;
            return;
        }

        bool  isOrgOrExt = true;
        bool  doRecalculateBBox = false;
        qint16  orgX = 0;
        qint16  orgY = 0;
        qint16  extX = 0;
        qint16  extY = 0;
        switch (numFunction &= 0xFF) {
        case 11: // setWindowOrg
            {
                st >> windowOrgY >> windowOrgX;
#if DEBUG_BBOX
                kDebug(31000) << "setWindowOrg" << windowOrgX << windowOrgY;
#endif
                if (!windowExtIsSet)
                    break;

                // The bounding box doesn't change just because we get
                // a new window.  Remember we are working in device
                // (viewport) coordinates when deciding the bounding
                // box.
                if (viewportExtIsSet)
                    break;

                // If there is no viewport, then use the window ext as
                // size, and (0, 0) as origin.
                //
                // FIXME: Handle the case where the window is defined
                //        first and then the viewport, without any
                //        drawing in between.  If that happens, I
                //        don't think that the window definition
                //        should influence the bounding box.
                orgX = 0;
                orgY = 0;
                extX = windowWidth;
                extY = windowHeight;
            }
            break;

        case 12: // setWindowExt
            {
                st >> windowHeight >> windowWidth;
                windowExtIsSet = true;
                bboxRecalculated = false;

#if DEBUG_BBOX
                kDebug(31000) << "setWindowExt" << windowWidth << windowHeight
                              << "(viewportOrg = " << viewportOrgX << viewportOrgY << ")";
#endif

                // If the viewport is set, then a changed window
                // changes nothing in the bounding box.
                if (viewportExtIsSet)
                    break;

                bboxRecalculated = false;

                // Collect the maximum width and height.
                if (abs(windowWidth - windowOrgX) > mMaxWidth)
                    mMaxWidth = abs(windowWidth - windowOrgX);
                if (abs(windowHeight - windowOrgY) > mMaxHeight)
                    mMaxHeight = abs(windowHeight - windowOrgY);

                orgX = 0;
                orgY = 0;
                extX = windowWidth;
                extY = windowHeight;
            }
            break;

        case 13: //setViewportOrg
            {
                st >> viewportOrgY >> viewportOrgX;
                bboxRecalculated = false;

#if DEBUG_BBOX
                kDebug(31000) << "setViewportOrg" << viewportOrgX << viewportOrgY;
#endif
                orgX = viewportOrgX;
                orgY = viewportOrgY;
                if (viewportExtIsSet) {
                    extX = viewportWidth;
                    extY = viewportHeight;
                }
                else {
                    // If the viewportExt is not set, then either a
                    // subsequent setViewportExt will set it, or the
                    // windowExt will be used instead.  
                    extX = windowWidth;
                    extY = windowHeight;
                }
                    break;

                // FIXME: Handle the case where the org changes but
                //        there is no subsequent Ext change (should be
                //        rather uncommon).
            }
            break;

        case 14: //setViewportExt
            {
                st >> viewportHeight >> viewportWidth;
                viewportExtIsSet = true;
                bboxRecalculated = false;

#if DEBUG_BBOX
                kDebug(31000) << "setViewportExt" << viewportWidth << viewportHeight;
#endif
                orgX = viewportOrgX;
                orgY = viewportOrgY;
                extX = viewportWidth;
                extY = viewportHeight;
            }
            break;

            // FIXME: Also support:
            //          ScaleWindowExt, ScaleViewportExt, 
            //          OffsetWindowOrg, OffsetViewportOrg

            // The following are drawing commands.  It is only when
            // there is an actual drawing command that we should check
            // the bounding box. It seems that some WMF files have
            // lots of changes of the window or viewports but without
            // any drawing commands in between. These changes should
            // not affect the bounding box.
        case 19: // lineTo
        //case 20: // moveTo
        case 23: // arc
        case 24: // ellipse
        case 26: // pie
        case 27: // rectangle
        case 28: // roundRect
        case 29: // patBlt
        case 31: // setPixel
        case 33: // textOut
        case 34: // bitBlt
        case 36: // polygon
        case 37: // polyline
        //case 38: // escape  FIXME: is this a drawing commmand?
        case 40: // fillRegion
        case 41:
        case 42:
        case 43:
        case 44:
        case 48: // chord
        case 50: // extTextOut
        case 56: // polyPolygon
        case 64: // dibBitBlt
        case 65: // dibStretchBlt
        case 67: // stretchDib
        case 72: // extFloodFill
#if DEBUG_BBOX
            kDebug(31000) << "drawing record: " << (numFunction & 0xff);
#endif
            doRecalculateBBox = true;
            break;

        default:
            isOrgOrExt = false;
        }

        // Recalculate the BBox if it was indicated above that it should be.
        if (doRecalculateBBox && !bboxRecalculated) {
#if DEBUG_BBOX
            kDebug(31000) << "Recalculating BBox";
#endif
            // If we have a viewport, always use that one.
            if (viewportExtIsSet) {
                orgX = viewportOrgX;
                orgY = viewportOrgY;
                extX = viewportWidth;
                extY = viewportHeight;
            }
            else {
                // If there is no defined viewport, then use the
                // window as the fallback viewport. But only the size,
                // the origin is always (0, 0).
                orgX = 0;
                orgY = 0;
                extX = qAbs(windowWidth);
                extY = qAbs(windowHeight);
            }

            // If ext < 0, switch the org and org+ext
            if (extX < 0) {
                orgX += extX;
                extX = -extX;
            }
            if (extY < 0) {
                orgY += extY;
                extY = -extY;
            }

            // At this point, the ext is always >= 0, i.e. org <= org+ext
#if DEBUG_BBOX
            kDebug(31000) << orgX << orgY << extX << extY;
#endif
            if (orgX < mBBoxLeft)          mBBoxLeft = orgX;
            if (orgY < mBBoxTop)           mBBoxTop  = orgY;
            if (orgX + extX > mBBoxRight)  mBBoxRight  = orgX + extX;
            if (orgY + extY > mBBoxBottom) mBBoxBottom = orgY + extY;

            bboxRecalculated = true;
        }

#if DEBUG_BBOX
        if (isOrgOrExt) {
            kDebug(31000) << "              mBBoxTop = " << mBBoxTop;
            kDebug(31000) << "mBBoxLeft = " << mBBoxLeft << "  mBBoxRight = " << mBBoxRight;
            kDebug(31000) << "           MBBoxBotton = " << mBBoxBottom;
            kDebug(31000) << "Max width,height = " << mMaxWidth << mMaxHeight;
        }
#endif

        mBuffer->seek(filePos + (size << 1));
    }
}


// ----------------------------------------------------------------
//                         Transform methods


void WmfParser::setWindowOrg(quint32, QDataStream& stream)
{
    qint16 top, left;

    stream >> top >> left;
    m_backend->setWindowOrg(left, top);
    mWindowLeft = left;
    mWindowTop = top;
#if DEBUG_RECORDS
    kDebug(31000) <<"Org: (" << left <<","  << top <<")";
#endif
}

/*  TODO : deeper look in negative width and height
*/

void WmfParser::setWindowExt(quint32, QDataStream& stream)
{
    qint16 width, height;

    // negative value allowed for width and height
    stream >> height >> width;
#if DEBUG_RECORDS
    kDebug(31000) <<"Ext: (" << width <<","  << height <<")";
#endif

    m_backend->setWindowExt(width, height);
    mWindowWidth  = width;
    mWindowHeight = height;
}


void WmfParser::OffsetWindowOrg(quint32, QDataStream &stream)
{
    qint16 offTop, offLeft;

    stream >> offTop >> offLeft;
    m_backend->setWindowOrg(mWindowLeft + offLeft, mWindowTop + offTop);

    // FIXME: Check if we must move the right and bottom edges too.
    mWindowLeft = mWindowLeft + offLeft;
    mWindowTop  = mWindowTop + offTop;
}


void WmfParser::ScaleWindowExt(quint32, QDataStream &stream)
{
    // Use 32 bits in the calculations to not lose precision.
    qint32 width, height;
    qint16 heightDenum, heightNum, widthDenum, widthNum;

    stream >> heightDenum >> heightNum >> widthDenum >> widthNum;

    if ((widthDenum != 0) && (heightDenum != 0)) {
        width = (qint32(mWindowWidth) * widthNum) / widthDenum;
        height = (qint32(mWindowHeight) * heightNum) / heightDenum;
        m_backend->setWindowExt(width, height);

        mWindowWidth  = width;
        mWindowHeight = height;
    }
    //kDebug(31000) <<"WmfParser::ScaleWindowExt :" << widthDenum <<"" << heightDenum;
}


void WmfParser::setViewportOrg(quint32, QDataStream& stream)
{
    qint16 top, left;

    stream >> top >> left;
    m_backend->setViewportOrg(left, top);

#if DEBUG_RECORDS
    kDebug(31000) <<"Org: (" << left <<","  << top <<")";
#endif
}

/*  TODO : deeper look in negative width and height
*/

void WmfParser::setViewportExt(quint32, QDataStream& stream)
{
    qint16 width, height;

    // Negative value allowed for width and height
    stream >> height >> width;
#if DEBUG_RECORDS
    kDebug(31000) <<"Ext: (" << width <<","  << height <<")";
#endif

    m_backend->setViewportExt(width, height);
}


//-----------------------------------------------------------------------------
// Drawing

void WmfParser::lineTo(quint32, QDataStream& stream)
{
    qint16 top, left;

    stream >> top >> left;
    m_backend->lineTo(left, top);
}


void WmfParser::moveTo(quint32, QDataStream& stream)
{
    qint16 top, left;

    stream >> top >> left;
    m_backend->moveTo(left, top);
}


void WmfParser::ellipse(quint32, QDataStream& stream)
{
    qint16 top, left, right, bottom;

    stream >> bottom >> right >> top >> left;
    m_backend->drawEllipse(left, top, right - left, bottom - top);
}


void WmfParser::polygon(quint32, QDataStream& stream)
{
    quint16 num;

    stream >> num;

    QPolygon pa(num);

    pointArray(stream, pa);
    m_backend->drawPolygon(pa, mWinding);
}


void WmfParser::polyPolygon(quint32, QDataStream& stream)
{
    quint16 numberPoly;
    quint16 sizePoly;
    QList<QPolygon> listPa;

    stream >> numberPoly;

    for (int i = 0 ; i < numberPoly ; i++) {
        stream >> sizePoly;
        listPa.append(QPolygon(sizePoly));
    }

    // list of point array
    for (int i = 0; i < numberPoly; i++) {
        pointArray(stream, listPa[i]);
    }

    // draw polygon's
    m_backend->drawPolyPolygon(listPa, mWinding);
    listPa.clear();
}


void WmfParser::polyline(quint32, QDataStream& stream)
{
    quint16 num;

    stream >> num;
    QPolygon pa(num);

    pointArray(stream, pa);
    m_backend->drawPolyline(pa);
}


void WmfParser::rectangle(quint32, QDataStream& stream)
{
    qint16 top, left, right, bottom;

    stream >> bottom >> right >> top >> left;
    kDebug(31000) << left << top << right << bottom;
    m_backend->drawRect(left, top, right - left, bottom - top);
}


void WmfParser::roundRect(quint32, QDataStream& stream)
{
    int xRnd = 0, yRnd = 0;
    quint16 widthCorner, heightCorner;
    qint16  top, left, right, bottom;

    stream >> heightCorner >> widthCorner;
    stream >> bottom >> right >> top >> left;

    // convert (widthCorner, heightCorner) in percentage
    if ((right - left) != 0)
        xRnd = (widthCorner * 100) / (right - left);
    if ((bottom - top) != 0)
        yRnd = (heightCorner * 100) / (bottom - top);

    m_backend->drawRoundRect(left, top, right - left, bottom - top, xRnd, yRnd);
}


void WmfParser::arc(quint32, QDataStream& stream)
{
    int xCenter, yCenter, angleStart, aLength;
    qint16  topEnd, leftEnd, topStart, leftStart;
    qint16  top, left, right, bottom;

    stream >> topEnd >> leftEnd >> topStart >> leftStart;
    stream >> bottom >> right >> top >> left;

    xCenter = left + ((right - left) / 2);
    yCenter = top + ((bottom - top) / 2);
    xyToAngle(leftStart - xCenter, yCenter - topStart, leftEnd - xCenter, yCenter - topEnd, angleStart, aLength);

    m_backend->drawArc(left, top, right - left, bottom - top, angleStart, aLength);
}


void WmfParser::chord(quint32, QDataStream& stream)
{
    int xCenter, yCenter, angleStart, aLength;
    qint16  topEnd, leftEnd, topStart, leftStart;
    qint16  top, left, right, bottom;

    stream >> topEnd >> leftEnd >> topStart >> leftStart;
    stream >> bottom >> right >> top >> left;

    xCenter = left + ((right - left) / 2);
    yCenter = top + ((bottom - top) / 2);
    xyToAngle(leftStart - xCenter, yCenter - topStart, leftEnd - xCenter, yCenter - topEnd, angleStart, aLength);

    m_backend->drawChord(left, top, right - left, bottom - top, angleStart, aLength);
}


void WmfParser::pie(quint32, QDataStream& stream)
{
    int xCenter, yCenter, angleStart, aLength;
    qint16  topEnd, leftEnd, topStart, leftStart;
    qint16  top, left, right, bottom;

    stream >> topEnd >> leftEnd >> topStart >> leftStart;
    stream >> bottom >> right >> top >> left;

    xCenter = left + ((right - left) / 2);
    yCenter = top + ((bottom - top) / 2);
    xyToAngle(leftStart - xCenter, yCenter - topStart, leftEnd - xCenter, yCenter - topEnd, angleStart, aLength);

    m_backend->drawPie(left, top, right - left, bottom - top, angleStart, aLength);
}


void WmfParser::setPolyFillMode(quint32, QDataStream& stream)
{
    quint16 winding;

    stream >> winding;
    mWinding = (winding != 0);
}


void WmfParser::setBkColor(quint32, QDataStream& stream)
{
    quint32 color;

    stream >> color;
    m_backend->setBackgroundColor(qtColor(color));
}


void WmfParser::setBkMode(quint32, QDataStream& stream)
{
    quint16 bkMode;

    stream >> bkMode;
    if (bkMode == 1)
        m_backend->setBackgroundMode(Qt::TransparentMode);
    else
        m_backend->setBackgroundMode(Qt::OpaqueMode);
}


void WmfParser::setPixel(quint32, QDataStream& stream)
{
    qint16  top, left;
    quint32 color;

    stream >> color >> top >> left;

    QPen oldPen = m_backend->pen();
    QPen pen = oldPen;
    pen.setColor(qtColor(color));
    m_backend->setPen(pen);
    m_backend->moveTo(left, top);
    m_backend->lineTo(left, top);
    m_backend->setPen(oldPen);
}


void WmfParser::setRop(quint32, QDataStream& stream)
{
    quint16  rop;

    stream >> rop;
    m_backend->setCompositionMode(winToQtComposition(rop));
}


void WmfParser::saveDC(quint32, QDataStream&)
{
    m_backend->save();
}


void WmfParser::restoreDC(quint32, QDataStream& stream)
{
    qint16  num;

    stream >> num;
    for (int i = 0; i > num ; i--)
        m_backend->restore();
}


void WmfParser::intersectClipRect(quint32, QDataStream& stream)
{
    qint16 top, left, right, bottom;

    stream >> bottom >> right >> top >> left;

    QRegion region = m_backend->clipRegion();
    QRegion newRegion(left, top, right - left, bottom - top);
    if (region.isEmpty()) {
        region = newRegion;
    } else {
        region = region.intersect(newRegion);
    }

    m_backend->setClipRegion(region);
}


void WmfParser::excludeClipRect(quint32, QDataStream& stream)
{
    qint16 top, left, right, bottom;

    stream >> bottom >> right >> top >> left;

    QRegion region = m_backend->clipRegion();
    QRegion newRegion(left, top, right - left, bottom - top);
    if (region.isEmpty()) {
        region = newRegion;
    } else {
        region = region.subtract(newRegion);
    }

    m_backend->setClipRegion(region);
}


//-----------------------------------------------------------------------------
// Text

void WmfParser::setTextColor(quint32, QDataStream& stream)
{
    quint32 color;

    stream >> color;
    mTextColor = qtColor(color);

    m_backend->setTextPen(QPen(mTextColor));
}


void WmfParser::setTextAlign(quint32, QDataStream& stream)
{
    stream >> mTextAlign;
    //kDebug(31000) << "new textalign: " << mTextAlign;
}


void WmfParser::textOut(quint32, QDataStream& stream)
{
    qint16 textLength;

    stream >> textLength;

    QByteArray text;
    text.resize(textLength);

    stream.readRawData(text.data(), textLength);
    // The string is always of even length, so if the actual data is
    // of uneven length, read an extra byte.
    if (textLength & 0x01) {
        qint8 dummy;
        stream >> dummy;
    }
        

    qint16 x, y;

    stream >> y;
    stream >> x;

    // FIXME: If we ever want to support vertical text (e.g. japanese),
    //        we need to send the vertical text align as well.
    m_backend->drawText(x, y, -1, -1, mTextAlign, text, static_cast<double>(mTextRotation));
}


void WmfParser::extTextOut(quint32 , QDataStream& stream)
{
#if 0
    qint16 parm[8];
    for (int i = 0; i < 4; ++i)
        stream >> parm[i];
    quint16 stringLength = parm[ 2 ];
    quint16 fwOpts = parm [ 3 ];
#else
    qint16 y, x;
    qint16 stringLength;
    quint16 fwOpts;
    qint16 top, left, right, bottom; // optional cliprect

    stream >> y;
    stream >> x;
    stream >> stringLength;
    stream >> fwOpts;
#endif

    QByteArray text;
    text.resize(stringLength);

    // ETO_CLIPPED flag adds 4 parameters
    if (fwOpts & (ETO_CLIPPED | ETO_OPAQUE)) {
        // read the optional clip rect
        stream >> bottom >> right >> top >> left;
    }
    stream.readRawData(text.data(), stringLength);

    // FIXME: If we ever want to support vertical text (e.g. japanese),
    //        we need to send the vertical text align as well.
    m_backend->drawText(x, y, -1, -1, mTextAlign, text, static_cast<double>(mTextRotation));
}



//-----------------------------------------------------------------------------
// Bitmap

void WmfParser::SetStretchBltMode(quint32, QDataStream&)
{
    if (mNbrFunc) {
        kDebug(31000) << "SetStretchBltMode : unimplemented";
    }
}


void WmfParser::dibBitBlt(quint32 size, QDataStream& stream)
{
    quint32 raster;
    qint16  topSrc, leftSrc, widthSrc, heightSrc;
    qint16  topDst, leftDst;

    stream >> raster;
    stream >> topSrc >> leftSrc >> heightSrc >> widthSrc;
    stream >> topDst >> leftDst;

    if (size > 11) {      // DIB image
        QImage bmpSrc;

        if (dibToBmp(bmpSrc, stream, (size - 11) * 2)) {
            m_backend->setCompositionMode(winToQtComposition(raster));

            m_backend->save();
            if (widthSrc < 0) {
                // negative width => horizontal flip
                QMatrix m(-1.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F);
                m_backend->setMatrix(m, true);
            }
            if (heightSrc < 0) {
                // negative height => vertical flip
                QMatrix m(1.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F);
                m_backend->setMatrix(m, true);
            }
            m_backend->drawImage(leftDst, topDst, bmpSrc, leftSrc, topSrc, widthSrc, heightSrc);
            m_backend->restore();
        }
    } else {
        kDebug(31000) << "WmfParser::dibBitBlt without image not implemented";
    }
}


void WmfParser::dibStretchBlt(quint32 size, QDataStream& stream)
{
    quint32 raster;
    qint16  topSrc, leftSrc, widthSrc, heightSrc;
    qint16  topDst, leftDst, widthDst, heightDst;
    QImage   bmpSrc;

    stream >> raster;
    stream >> heightSrc >> widthSrc >> topSrc >> leftSrc;
    stream >> heightDst >> widthDst >> topDst >> leftDst;

    if (dibToBmp(bmpSrc, stream, (size - 13) * 2)) {
        m_backend->setCompositionMode(winToQtComposition(raster));

        m_backend->save();
        if (widthDst < 0) {
            // negative width => horizontal flip
            QMatrix m(-1.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F);
            m_backend->setMatrix(m, true);
        }
        if (heightDst < 0) {
            // negative height => vertical flip
            QMatrix m(1.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F);
            m_backend->setMatrix(m, true);
        }
        bmpSrc = bmpSrc.copy(leftSrc, topSrc, widthSrc, heightSrc);
        // TODO: scale the bitmap : QImage::scale(widthDst, heightDst)
        // is actually too slow

        m_backend->drawImage(leftDst, topDst, bmpSrc);
        m_backend->restore();
    }
}


void WmfParser::stretchDib(quint32 size, QDataStream& stream)
{
    quint32 raster;
    qint16  arg, topSrc, leftSrc, widthSrc, heightSrc;
    qint16  topDst, leftDst, widthDst, heightDst;
    QImage   bmpSrc;

    stream >> raster >> arg;
    stream >> heightSrc >> widthSrc >> topSrc >> leftSrc;
    stream >> heightDst >> widthDst >> topDst >> leftDst;

    if (dibToBmp(bmpSrc, stream, (size - 14) * 2)) {
        m_backend->setCompositionMode(winToQtComposition(raster));

        m_backend->save();
        if (widthDst < 0) {
            // negative width => horizontal flip
            QMatrix m(-1.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F);
            m_backend->setMatrix(m, true);
        }
        if (heightDst < 0) {
            // negative height => vertical flip
            QMatrix m(1.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F);
            m_backend->setMatrix(m, true);
        }
        bmpSrc = bmpSrc.copy(leftSrc, topSrc, widthSrc, heightSrc);
        // TODO: scale the bitmap ( QImage::scale(param[ 8 ], param[ 7 ]) is actually too slow )

        m_backend->drawImage(leftDst, topDst, bmpSrc);
        m_backend->restore();
    }
}


void WmfParser::dibCreatePatternBrush(quint32 size, QDataStream& stream)
{
    KoWmfPatternBrushHandle* handle = new KoWmfPatternBrushHandle;

    if (addHandle(handle)) {
        quint32 arg;
        QImage bmpSrc;

        stream >> arg;
        if (dibToBmp(bmpSrc, stream, (size - 5) * 2)) {
            handle->image = QPixmap::fromImage(bmpSrc);
            handle->brush.setTexture(handle->image);
        } else {
            kDebug(31000) << "WmfParser::dibCreatePatternBrush : incorrect DIB image";
        }
    }
}


void WmfParser::patBlt(quint32, QDataStream& stream)
{
    quint32 rasterOperation;
    quint16 height, width;
    qint16  y, x;

    stream >> rasterOperation;
    stream >> height >> width;
    stream >> y >> x;

    //kDebug(31000) << "patBlt record" << hex << rasterOperation << dec
    //              << x << y << width << height;

    m_backend->patBlt(x, y, width, height, rasterOperation);
}



//-----------------------------------------------------------------------------
// Object handle

void WmfParser::selectObject(quint32, QDataStream& stream)
{
    quint16 idx;

    stream >> idx;
    if ((idx < mNbrObject) && (mObjHandleTab[ idx ] != 0))
        mObjHandleTab[ idx ]->apply(m_backend);
    else
        kDebug(31000) << "WmfParser::selectObject : selection of an empty object";
}


void WmfParser::deleteObject(quint32, QDataStream& stream)
{
    quint16 idx;

    stream >> idx;
    deleteHandle(idx);
}


void WmfParser::createEmptyObject()
{
    // allocation of an empty object (to keep object counting in sync)
    KoWmfPenHandle* handle = new KoWmfPenHandle;

    addHandle(handle);
}


void WmfParser::createBrushIndirect(quint32, QDataStream& stream)
{
    Qt::BrushStyle style;
    quint16 sty, arg2;
    quint32 color;
    KoWmfBrushHandle* handle = new KoWmfBrushHandle;

    if (addHandle(handle)) {
        stream >> sty >> color >> arg2;

        if (sty == 2) {
            if (arg2 < 6)
                style = koWmfHatchedStyleBrush[ arg2 ];
            else {
                kDebug(31000) << "WmfParser::createBrushIndirect: invalid hatched brush" << arg2;
                style = Qt::SolidPattern;
            }
        } else {
            if (sty < 9)
                style = koWmfStyleBrush[ sty ];
            else {
                kDebug(31000) << "WmfParser::createBrushIndirect: invalid brush" << sty;
                style = Qt::SolidPattern;
            }
        }
        handle->brush.setStyle(style);
        handle->brush.setColor(qtColor(color));
    }
}


void WmfParser::createPenIndirect(quint32, QDataStream& stream)
{
    // TODO: userStyle and alternateStyle
    quint32 color;
    quint16 style, width, arg;

    KoWmfPenHandle* handle = new KoWmfPenHandle;

    if (addHandle(handle)) {
        stream >> style >> width >> arg >> color;

        // set the style defaults
        handle->pen.setStyle(Qt::SolidLine);
        handle->pen.setCapStyle(Qt::RoundCap);
        handle->pen.setJoinStyle(Qt::RoundJoin);

        const int PenStyleMask = 0x0000000F;
        const int PenCapMask   = 0x00000F00;
        const int PenJoinMask  = 0x0000F000;

        quint16 penStyle = style & PenStyleMask;
        if (penStyle < 7)
            handle->pen.setStyle(koWmfStylePen[ penStyle ]);
        else
            kDebug(31000) << "WmfParser::createPenIndirect: invalid pen" << style;

        quint16 capStyle = (style & PenCapMask) >> 8;
        if (capStyle < 3)
            handle->pen.setCapStyle(koWmfCapStylePen[ capStyle ]);
        else
            kDebug(31000) << "WmfParser::createPenIndirect: invalid pen cap style" << style;

        quint16 joinStyle = (style & PenJoinMask) >> 12;
        if (joinStyle < 3)
            handle->pen.setJoinStyle(koWmfJoinStylePen[ joinStyle ]);
        else
            kDebug(31000) << "WmfParser::createPenIndirect: invalid pen join style" << style;

        handle->pen.setColor(qtColor(color));
        handle->pen.setWidth(width);
    }
}


void WmfParser::createFontIndirect(quint32 size, QDataStream& stream)
{
    qint16  height;             // Height of the character cell
    qint16  width;              // Average width (not used)
    qint16  rotation;           // The rotation of the text in 1/10th degrees
    qint16  orientation;        // The rotation of each character
    quint16 weight, property, fixedPitch, arg;

    KoWmfFontHandle* handle = new KoWmfFontHandle;

    if (addHandle(handle)) {
        stream >> height >> width;
        stream >> rotation >> orientation;
        stream >> weight >> property >> arg >> arg;
        stream >> fixedPitch;

        //kDebug(31000) << height << width << weight << property;
        // text rotation (in 1/10 degree)
        // TODO: memorisation of rotation in object Font
        mTextRotation = -rotation / 10;
        handle->font.setFixedPitch(((fixedPitch & 0x01) == 0));

        // A negative width means to use device units.
        kDebug(31000) << "Font height:" << height;
        handle->height = height;
        // FIXME: For some reason this value needs to be multiplied by
        //        a factor.  0.6 seems to give a good result, but why??
        // ANSWER(?): The doc says the height is the height of the character cell.
        //            But normally the font height is only the height above the baseline,
        //            isn't it?
        handle->font.setPointSize(qAbs(height) * 6 / 10);
        if (weight == 0)
            weight = QFont::Normal;
        else {
            // Linear transform between MS weights to Qt weights
            // MS: 400=normal, 700=bold
            // Qt: 50=normal, 75=bold
            // This makes the line cross x=0 at y=50/3.  (x=MS weight, y=Qt weight)
            //
            // FIXME: Is this a linear relationship?
            weight = (50 + 3 * ((weight * (75-50))/(700-400))) / 3;
        }
        handle->font.setWeight(weight);
        handle->font.setItalic((property & 0x01));
        handle->font.setUnderline((property & 0x100));
        // TODO: Strikethrough

        // font name
        int    maxChar = (size - 12) * 2;
        char*  nameFont = new char[maxChar];
        stream.readRawData(nameFont, maxChar);
        handle->font.setFamily(nameFont);
        delete[] nameFont;
    }
}


//-----------------------------------------------------------------------------
// Misc functions

void WmfParser::end(quint32, QDataStream&)
{
}

quint16 WmfParser::calcCheckSum(WmfPlaceableHeader* apmfh)
{
    quint16*  lpWord;
    quint16   wResult, i;

    // Start with the first word
    wResult = *(lpWord = (quint16*)(apmfh));
    // XOR in each of the other 9 words
    for (i = 1; i <= 9; i++) {
        wResult ^= lpWord[ i ];
    }
    return wResult;
}


void WmfParser::notyet(quint32, QDataStream&)
{
    if (mNbrFunc) {
        kDebug(31000) << "unimplemented";
    }
}

void WmfParser::region(quint32, QDataStream&)
{
    if (mNbrFunc) {
        kDebug(31000) << "region : unimplemented";
    }
}

void WmfParser::palette(quint32, QDataStream&)
{
    if (mNbrFunc) {
        kDebug(31000) << "palette : unimplemented";
    }
}

void WmfParser::escape(quint32, QDataStream&)
{
    if (mNbrFunc) {
        kDebug(31000) << "escape : unimplemented";
    }
}

void WmfParser::setRelAbs(quint32, QDataStream&)
{
    if (mNbrFunc) {
        kDebug(31000) << "setRelAbs : unimplemented";
    }
}

void WmfParser::setMapMode(quint32, QDataStream& stream)
{
    stream >> mMapMode;
    //kDebug(31000) << "New mapmode: " << mMapMode;
}

void WmfParser::extFloodFill(quint32, QDataStream&)
{
    if (mNbrFunc) {
        kDebug(31000) << "extFloodFill : unimplemented";
    }
}

void WmfParser::setLayout(quint32, QDataStream &stream)
{
    quint16 layout;
    quint16 reserved;

    // negative value allowed for width and height
    stream >> layout >> reserved;
    mLayout = (WmfLayout)layout;

#if DEBUG_RECORDS
    kDebug(31000) << "setLayout: layout=" << layout;
#endif
}

void WmfParser::startDoc(quint32, QDataStream&)
{
    if (mNbrFunc) {
        kDebug(31000) << "startDoc : unimplemented";
    }
}

void WmfParser::startPage(quint32, QDataStream&)
{
    if (mNbrFunc) {
        kDebug(31000) << "startPage : unimplemented";
    }
}

void WmfParser::endDoc(quint32, QDataStream&)
{
    if (mNbrFunc) {
        kDebug(31000) << "endDoc : unimplemented";
    }
}

void WmfParser::endPage(quint32, QDataStream&)
{
    if (mNbrFunc) {
        kDebug(31000) << "endPage : unimplemented";
    }
}

void WmfParser::resetDC(quint32, QDataStream&)
{
    if (mNbrFunc) {
        kDebug(31000) << "resetDC : unimplemented";
    }
}

void WmfParser::bitBlt(quint32, QDataStream&)
{
    if (mNbrFunc) {
        kDebug(31000) << "bitBlt : unimplemented";
    }
}

void WmfParser::setDibToDev(quint32, QDataStream&)
{
    if (mNbrFunc) {
        kDebug(31000) << "setDibToDev : unimplemented";
    }
}

void WmfParser::createBrush(quint32, QDataStream&)
{
    if (mNbrFunc) {
        kDebug(31000) << "createBrush : unimplemented";
    }
}

void WmfParser::createPatternBrush(quint32, QDataStream&)
{
    if (mNbrFunc) {
        kDebug(31000) << "createPatternBrush : unimplemented";
    }
}

void WmfParser::createBitmap(quint32, QDataStream&)
{
    if (mNbrFunc) {
        kDebug(31000) << "createBitmap : unimplemented";
    }
}

void WmfParser::createBitmapIndirect(quint32, QDataStream&)
{
    createEmptyObject();
    if (mNbrFunc) {
        kDebug(31000) << "createBitmapIndirect : unimplemented";
    }
}

void WmfParser::createPalette(quint32, QDataStream&)
{
    createEmptyObject();
    if (mNbrFunc) {
        kDebug(31000) << "createPalette : unimplemented";
    }
}

void WmfParser::createRegion(quint32, QDataStream&)
{
    createEmptyObject();
    if (mNbrFunc) {
        kDebug(31000) << "createRegion : unimplemented";
    }
}



//-----------------------------------------------------------------------------
// Utilities and conversion Wmf -> Qt

bool WmfParser::addHandle(KoWmfHandle* handle)
{
    int idx;

    for (idx = 0; idx < mNbrObject ; idx++) {
        if (mObjHandleTab[ idx ] == 0)  break;
    }

    if (idx < mNbrObject) {
        mObjHandleTab[ idx ] = handle;
        return true;
    } else {
        delete handle;
        mStackOverflow = true;
        kDebug(31000) << "WmfParser::addHandle : stack overflow = broken file !";
        return false;
    }
}


void WmfParser::deleteHandle(int idx)
{
    if ((idx < mNbrObject) && (mObjHandleTab[idx] != 0)) {
        delete mObjHandleTab[ idx ];
        mObjHandleTab[ idx ] = 0;
    } else {
        kDebug(31000) << "WmfParser::deletehandle() : bad index number";
    }
}


void WmfParser::pointArray(QDataStream& stream, QPolygon& pa)
{
    qint16 left, top;
    int  i, max;

    for (i = 0, max = pa.size() ; i < max ; i++) {
        stream >> left >> top;
        pa.setPoint(i, left, top);
    }
}


void WmfParser::xyToAngle(int xStart, int yStart, int xEnd, int yEnd, int& angleStart, int& angleLength)
{
    double aStart, aLength;

    aStart = atan2((double)yStart, (double)xStart);
    aLength = atan2((double)yEnd, (double)xEnd) - aStart;

    angleStart = (int)((aStart * 2880) / 3.14166);
    angleLength = (int)((aLength * 2880) / 3.14166);
    if (angleLength < 0) angleLength = 5760 + angleLength;
}


QPainter::CompositionMode WmfParser::winToQtComposition(quint16 param) const
{
    if (param < 17)
        return koWmfOpTab16[ param ];
    else
        return QPainter::CompositionMode_Source;
}


QPainter::CompositionMode  WmfParser::winToQtComposition(quint32 param) const
{
    /* TODO: Ternary raster operations
    0x00C000CA  dest = (source AND pattern)
    0x00F00021  dest = pattern
    0x00FB0A09  dest = DPSnoo
    0x005A0049  dest = pattern XOR dest   */
    int i;

    for (i = 0 ; i < 15 ; i++) {
        if (koWmfOpTab32[ i ].winRasterOp == param)  break;
    }

    if (i < 15)
        return koWmfOpTab32[ i ].qtRasterOp;
    else
        return QPainter::CompositionMode_SourceOver;
}


bool WmfParser::dibToBmp(QImage& bmp, QDataStream& stream, quint32 size)
{
    typedef struct _BMPFILEHEADER {
        quint16 bmType;
        quint32 bmSize;
        quint16 bmReserved1;
        quint16 bmReserved2;
        quint32 bmOffBits;
    }  BMPFILEHEADER;

    int sizeBmp = size + 14;

    QByteArray pattern;           // BMP header and DIB data
    pattern.resize(sizeBmp);
    pattern.fill(0);
    stream.readRawData(pattern.data() + 14, size);

    // add BMP header
    BMPFILEHEADER* bmpHeader;
    bmpHeader = (BMPFILEHEADER*)(pattern.data());
    bmpHeader->bmType = 0x4D42;
    bmpHeader->bmSize = sizeBmp;

//    if ( !bmp.loadFromData( (const uchar*)bmpHeader, pattern.size(), "BMP" ) ) {
    if (!bmp.loadFromData(pattern, "BMP")) {
        kDebug(31000) << "WmfParser::dibToBmp: invalid bitmap";
        return false;
    } else {
        return true;
    }
}


}
