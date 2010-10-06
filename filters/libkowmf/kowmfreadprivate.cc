/* This file is part of the KDE libraries
 * Copyright (c) 1998 Stefan Taferner
 *               2001/2003 thierry lorthiois (lorthioist@wanadoo.fr)
 *               2007-2008 Jan Hambrecht <jaham@gmx.net>
 *               2009-2010 Inge Wallin <inge@lysator.liu.se>
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

#include "kowmfreadprivate.h"
#include "kowmfread.h"

#include <kdebug.h>

#include <QtGui/QImage>
#include <QtGui/QMatrix>
#include <QtCore/QDataStream>
#include <QtCore/QByteArray>
#include <QtCore/QBuffer>
#include <QtGui/QPolygon>

#include <math.h>


#define DEBUG_RECORDS 0


KoWmfReadPrivate::KoWmfReadPrivate()
{
    mNbrFunc = 0;
    mValid = false;
    mStandard = false;
    mPlaceable = false;
    mEnhanced = false;
    mBuffer = 0;
    mObjHandleTab = 0;
}


KoWmfReadPrivate::~KoWmfReadPrivate()
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


bool KoWmfReadPrivate::load(const QByteArray& array)
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
    WmfEnhMetaHeader eheader;
    WmfMetaHeader header;
    WmfPlaceableHeader pheader; // Contains a bounding box
    unsigned short checksum;
    int filePos;

    QDataStream st(mBuffer);
    st.setByteOrder(QDataStream::LittleEndian);
    mStackOverflow = false;
    mWinding = false;
    mLayout = LAYOUT_LTR;
    mTextAlign = 0;
    mTextRotation = 0;
    mTextColor = Qt::black;
    mValid = false;
    mStandard = false;
    mPlaceable = false;
    mEnhanced = false;

    // Initialize the bounding box.
    mBBoxTop = 32767;           // Is there a constant for this?
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
        kDebug(31000) << "KoWmfReadPrivate : incorrect file format !";
    }

    // check bounding rectangle for standard meta file
    if (mStandard && mValid) {
        // Note that this call can change mValid.
        createBoundingBox(st);
    }

    return mValid;
}


bool KoWmfReadPrivate::play(KoWmfRead* readWmf)
{
    if (!(mValid)) {
        kDebug(31000) << "KoWmfReadPrivate::play : invalid WMF file";
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
    mReadWmf = readWmf;

    // Set some initial values.
    mWindowTop    = 0;
    mWindowLeft   = 0;
    mWindowWidth  = 1;
    mWindowHeight = 1;
    mViewportTop    = 0;
    mViewportLeft   = 0;
    mViewportWidth  = 1;
    mViewportHeight = 1;

    if (mReadWmf->begin()) {
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

        mReadWmf->end();
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


void KoWmfReadPrivate::createBoundingBox(QDataStream &st)
{
    // check bounding rectangle for standard meta file
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
    while (numFunction) {

        filePos = mBuffer->pos();
        st >> size >> numFunction;

        if (size == 0) {
            kDebug(31000) << "KoWmfReadPrivate: incorrect file!";
            mValid = 0;
            return;
        }

        bool  isOrgOrExt = true;
        switch (numFunction &= 0xFF) {
        case 11: // setWindowOrg
            {
                st >> windowOrgY >> windowOrgX;
#if DEBUG_RECORDS
                kDebug(31000) << "setWindowOrg" << windowOrgX << windowOrgY;
#endif
                if (!windowExtIsSet)
                    break;

                // If there is actually a viewport, then the bounding
                // box doesn't change just because we get a new
                // window.  Remember we are working in device
                // (viewport) coordinates when deciding the bounding box.
                if (viewportExtIsSet)
                    break;

                // If there is no viewport, then use the window as a bounding box.
                //
                // Note that the windowOrg only will have an effect on
                // the bounding box the first time it appears,
                // immediately after the initialization of it.
                //
                // FIXME: Handle the case where the window is defined
                //        first and then the viewport, without any
                //        drawing in between.  If that happens, I
                //        don't think that the window definition
                //        should influence the bounding box.
                if (viewportOrgY < mBBoxTop)    mBBoxTop = viewportOrgY;
                if (viewportOrgX < mBBoxLeft)   mBBoxLeft = viewportOrgX;
                if (viewportOrgX > mBBoxRight)  mBBoxRight = viewportOrgX;
                if (viewportOrgY > mBBoxBottom) mBBoxBottom = viewportOrgY;

                // FIXME: Handle negative width or height.
            }
            break;

        case 12: // setWindowExt
            {
                qint16 width;
                qint16 height;

                st >> height >> width;
                windowExtIsSet = true;
#if DEBUG_RECORDS
                kDebug(31000) << "setWindowExt" << width << height
                              << "(viewportOrg = " << viewportOrgX << viewportOrgY << ")";
#endif

                // If the viewport is set, then a changed window
                // changes nothing in the bounding box.
                if (viewportExtIsSet)
                    return;

                // Collect the maximum width and height.
                if (abs(width - windowOrgX) > mMaxWidth)
                    mMaxWidth = abs(width - windowOrgX);
                if (abs(height - windowOrgY) > mMaxHeight)
                    mMaxHeight = abs(height - windowOrgY);

                // Negative values are allowed
                if (width < 0) {
                    if (viewportOrgX + width < mBBoxLeft) {
                        mBBoxLeft = viewportOrgX + width;
                    }
                }
                else {
                    if (viewportOrgX + width > mBBoxRight) {
                        mBBoxRight = viewportOrgX + width;
                    }
                }

                if (height < 0) {
                    if (viewportOrgY + height < mBBoxTop) {
                        mBBoxTop = viewportOrgY + height;
                    }
                }
                else {
                    if (viewportOrgY + height > mBBoxBottom) {
                        mBBoxBottom = viewportOrgY + height;
                    }
                }
            }
        break;

        case 13: //setViewportOrg
            {
                st >> viewportOrgY >> viewportOrgX;

#if DEBUG_RECORDS
                kDebug(31000) << "setViewportOrg" << viewportOrgX << viewportOrgY;
#endif
                // Can't do anything without the viewport extensions.
                if (!viewportExtIsSet)
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

#if DEBUG_RECORDS
                kDebug(31000) << "setViewportExt" << viewportWidth << viewportHeight;
#endif
                if (viewportHeight >= 0) {

                    if (viewportOrgY < mBBoxTop)                     mBBoxTop    = viewportOrgY;
                    if (viewportOrgY + viewportHeight > mBBoxBottom) mBBoxBottom = viewportOrgY + viewportHeight;
                }
                else {
                    // FIXME: Handle negative viewport heights
                }

                if (viewportWidth >= 0) {

                    if (viewportOrgX < mBBoxLeft)                   mBBoxLeft  = viewportOrgX;
                    if (viewportOrgX + viewportWidth > mBBoxRight)  mBBoxRight = viewportOrgX + viewportWidth;
                }
                else {
                    // FIXME: Handle negative viewport widths
                }
            }
            break;

        default:
            isOrgOrExt = false;
        }

#if DEBUG_RECORDS
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


//-----------------------------------------------------------------------------
// Metafile painter methods


void KoWmfReadPrivate::setWindowOrg(quint32, QDataStream& stream)
{
    qint16 top, left;

    stream >> top >> left;
    mReadWmf->setWindowOrg(left, top);
    mWindowLeft = left;
    mWindowTop = top;
#if DEBUG_RECORDS
    kDebug(31000) <<"Org: (" << left <<","  << top <<")";
#endif
}

/*  TODO : deeper look in negative width and height
*/

void KoWmfReadPrivate::setWindowExt(quint32, QDataStream& stream)
{
    qint16 width, height;

    // negative value allowed for width and height
    stream >> height >> width;
#if DEBUG_RECORDS
    kDebug(31000) <<"Ext: (" << width <<","  << height <<")";
#endif

    mReadWmf->setWindowExt(width, height);
    mWindowWidth  = width;
    mWindowHeight = height;
}


void KoWmfReadPrivate::OffsetWindowOrg(quint32, QDataStream &stream)
{
    qint16 offTop, offLeft;

    stream >> offTop >> offLeft;
    mReadWmf->setWindowOrg(mWindowLeft + offLeft, mWindowTop + offTop);

    // FIXME: Check if we must move the right and bottom edges too.
    mWindowLeft = mWindowLeft + offLeft;
    mWindowTop  = mWindowTop + offTop;
}


void KoWmfReadPrivate::ScaleWindowExt(quint32, QDataStream &stream)
{
    // Use 32 bits in the calculations to not lose precision.
    qint32 width, height;
    qint16 heightDenum, heightNum, widthDenum, widthNum;

    stream >> heightDenum >> heightNum >> widthDenum >> widthNum;

    if ((widthDenum != 0) && (heightDenum != 0)) {
        width = (qint32(mWindowWidth) * widthNum) / widthDenum;
        height = (qint32(mWindowHeight) * heightNum) / heightDenum;
        mReadWmf->setWindowExt(width, height);

        mWindowWidth  = width;
        mWindowHeight = height;
    }
    //kDebug(31000) <<"KoWmfReadPrivate::ScaleWindowExt :" << widthDenum <<"" << heightDenum;
}


void KoWmfReadPrivate::setViewportOrg(quint32, QDataStream& stream)
{
    qint16 top, left;

    stream >> top >> left;
    mReadWmf->setViewportOrg(left, top);
    mViewportLeft = left;
    mViewportTop = top;

#if DEBUG_RECORDS
    kDebug(31000) <<"Org: (" << left <<","  << top <<")";
#endif
}

/*  TODO : deeper look in negative width and height
*/

void KoWmfReadPrivate::setViewportExt(quint32, QDataStream& stream)
{
    qint16 width, height;

    // Negative value allowed for width and height
    stream >> height >> width;
#if DEBUG_RECORDS
    kDebug(31000) <<"Ext: (" << width <<","  << height <<")";
#endif

    mReadWmf->setViewportExt(width, height);
    mViewportWidth  = width;
    mViewportHeight = height;
}


//-----------------------------------------------------------------------------
// Drawing

void KoWmfReadPrivate::lineTo(quint32, QDataStream& stream)
{
    qint16 top, left;

    stream >> top >> left;
    mReadWmf->lineTo(left, top);
}


void KoWmfReadPrivate::moveTo(quint32, QDataStream& stream)
{
    qint16 top, left;

    stream >> top >> left;
    mReadWmf->moveTo(left, top);
}


void KoWmfReadPrivate::ellipse(quint32, QDataStream& stream)
{
    qint16 top, left, right, bottom;

    stream >> bottom >> right >> top >> left;
    mReadWmf->drawEllipse(left, top, right - left, bottom - top);
}


void KoWmfReadPrivate::polygon(quint32, QDataStream& stream)
{
    quint16 num;

    stream >> num;

    QPolygon pa(num);

    pointArray(stream, pa);
    mReadWmf->drawPolygon(pa, mWinding);
}


void KoWmfReadPrivate::polyPolygon(quint32, QDataStream& stream)
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
    mReadWmf->drawPolyPolygon(listPa, mWinding);
    listPa.clear();
}


void KoWmfReadPrivate::polyline(quint32, QDataStream& stream)
{
    quint16 num;

    stream >> num;
    QPolygon pa(num);

    pointArray(stream, pa);
    mReadWmf->drawPolyline(pa);
}


void KoWmfReadPrivate::rectangle(quint32, QDataStream& stream)
{
    qint16 top, left, right, bottom;

    stream >> bottom >> right >> top >> left;
    kDebug(31000) << left << top << right << bottom;
    mReadWmf->drawRect(left, top, right - left, bottom - top);
}


void KoWmfReadPrivate::roundRect(quint32, QDataStream& stream)
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

    mReadWmf->drawRoundRect(left, top, right - left, bottom - top, xRnd, yRnd);
}


void KoWmfReadPrivate::arc(quint32, QDataStream& stream)
{
    int xCenter, yCenter, angleStart, aLength;
    qint16  topEnd, leftEnd, topStart, leftStart;
    qint16  top, left, right, bottom;

    stream >> topEnd >> leftEnd >> topStart >> leftStart;
    stream >> bottom >> right >> top >> left;

    xCenter = left + ((right - left) / 2);
    yCenter = top + ((bottom - top) / 2);
    xyToAngle(leftStart - xCenter, yCenter - topStart, leftEnd - xCenter, yCenter - topEnd, angleStart, aLength);

    mReadWmf->drawArc(left, top, right - left, bottom - top, angleStart, aLength);
}


void KoWmfReadPrivate::chord(quint32, QDataStream& stream)
{
    int xCenter, yCenter, angleStart, aLength;
    qint16  topEnd, leftEnd, topStart, leftStart;
    qint16  top, left, right, bottom;

    stream >> topEnd >> leftEnd >> topStart >> leftStart;
    stream >> bottom >> right >> top >> left;

    xCenter = left + ((right - left) / 2);
    yCenter = top + ((bottom - top) / 2);
    xyToAngle(leftStart - xCenter, yCenter - topStart, leftEnd - xCenter, yCenter - topEnd, angleStart, aLength);

    mReadWmf->drawChord(left, top, right - left, bottom - top, angleStart, aLength);
}


void KoWmfReadPrivate::pie(quint32, QDataStream& stream)
{
    int xCenter, yCenter, angleStart, aLength;
    qint16  topEnd, leftEnd, topStart, leftStart;
    qint16  top, left, right, bottom;

    stream >> topEnd >> leftEnd >> topStart >> leftStart;
    stream >> bottom >> right >> top >> left;

    xCenter = left + ((right - left) / 2);
    yCenter = top + ((bottom - top) / 2);
    xyToAngle(leftStart - xCenter, yCenter - topStart, leftEnd - xCenter, yCenter - topEnd, angleStart, aLength);

    mReadWmf->drawPie(left, top, right - left, bottom - top, angleStart, aLength);
}


void KoWmfReadPrivate::setPolyFillMode(quint32, QDataStream& stream)
{
    quint16 winding;

    stream >> winding;
    mWinding = (winding != 0);
}


void KoWmfReadPrivate::setBkColor(quint32, QDataStream& stream)
{
    quint32 color;

    stream >> color;
    mReadWmf->setBackgroundColor(qtColor(color));
}


void KoWmfReadPrivate::setBkMode(quint32, QDataStream& stream)
{
    quint16 bkMode;

    stream >> bkMode;
    if (bkMode == 1)
        mReadWmf->setBackgroundMode(Qt::TransparentMode);
    else
        mReadWmf->setBackgroundMode(Qt::OpaqueMode);
}


void KoWmfReadPrivate::setPixel(quint32, QDataStream& stream)
{
    qint16  top, left;
    quint32 color;

    stream >> color >> top >> left;

    QPen oldPen = mReadWmf->pen();
    QPen pen = oldPen;
    pen.setColor(qtColor(color));
    mReadWmf->setPen(pen);
    mReadWmf->moveTo(left, top);
    mReadWmf->lineTo(left, top);
    mReadWmf->setPen(oldPen);
}


void KoWmfReadPrivate::setRop(quint32, QDataStream& stream)
{
    quint16  rop;

    stream >> rop;
    mReadWmf->setCompositionMode(winToQtComposition(rop));
}


void KoWmfReadPrivate::saveDC(quint32, QDataStream&)
{
    mReadWmf->save();
}


void KoWmfReadPrivate::restoreDC(quint32, QDataStream& stream)
{
    qint16  num;

    stream >> num;
    for (int i = 0; i > num ; i--)
        mReadWmf->restore();
}


void KoWmfReadPrivate::intersectClipRect(quint32, QDataStream& stream)
{
    qint16 top, left, right, bottom;

    stream >> bottom >> right >> top >> left;

    QRegion region = mReadWmf->clipRegion();
    QRegion newRegion(left, top, right - left, bottom - top);
    if (region.isEmpty()) {
        region = newRegion;
    } else {
        region = region.intersect(newRegion);
    }

    mReadWmf->setClipRegion(region);
}


void KoWmfReadPrivate::excludeClipRect(quint32, QDataStream& stream)
{
    qint16 top, left, right, bottom;

    stream >> bottom >> right >> top >> left;

    QRegion region = mReadWmf->clipRegion();
    QRegion newRegion(left, top, right - left, bottom - top);
    if (region.isEmpty()) {
        region = newRegion;
    } else {
        region = region.subtract(newRegion);
    }

    mReadWmf->setClipRegion(region);
}


//-----------------------------------------------------------------------------
// Text

void KoWmfReadPrivate::setTextColor(quint32, QDataStream& stream)
{
    quint32 color;

    stream >> color;
    mTextColor = qtColor(color);

    mReadWmf->setTextPen(QPen(mTextColor));
}


void KoWmfReadPrivate::setTextAlign(quint32, QDataStream& stream)
{
    stream >> mTextAlign;
}


void KoWmfReadPrivate::textOut(quint32, QDataStream& stream)
{
    qint16 textLength;

    stream >> textLength;

    QByteArray text;
    text.resize(textLength);

    stream.readRawData(text.data(), textLength);

    qint16 x, y;

    stream >> y;
    stream >> x;

    // FIXME: If we ever want to support vertical text (e.g. japanese),
    //        we need to send the vertical text align as well.
    mReadWmf->drawText(x, y, -1, -1, mTextAlign, text, static_cast<double>(mTextRotation));
}


void KoWmfReadPrivate::extTextOut(quint32 , QDataStream& stream)
{
    qint16 parm[8];
    for (int i = 0; i < 4; ++i)
        stream >> parm[i];

    quint16 textLength = parm[ 2 ];

    QByteArray text;
    text.resize(textLength);

    if (parm[ 3 ] != 0) {       // ETO_CLIPPED flag add 4 parameters
        for (int i = 0; i < 4; ++i)
            stream >> parm[4+i];
        stream.readRawData(text.data(), textLength);
    } else {
        stream.readRawData(text.data(), textLength);
    }

    // FIXME: If we ever want to support vertical text (e.g. japanese),
    //        we need to send the vertical text align as well.
    mReadWmf->drawText(parm[ 1 ], parm[ 0 ], -1, -1, mTextAlign, text, static_cast<double>(mTextRotation));
}



//-----------------------------------------------------------------------------
// Bitmap

void KoWmfReadPrivate::SetStretchBltMode(quint32, QDataStream&)
{
    if (mNbrFunc) {
        kDebug(31000) << "SetStretchBltMode : unimplemented";
    }
}


void KoWmfReadPrivate::dibBitBlt(quint32 size, QDataStream& stream)
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
            mReadWmf->setCompositionMode(winToQtComposition(raster));

            mReadWmf->save();
            if (widthSrc < 0) {
                // negative width => horizontal flip
                QMatrix m(-1.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F);
                mReadWmf->setMatrix(m, true);
            }
            if (heightSrc < 0) {
                // negative height => vertical flip
                QMatrix m(1.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F);
                mReadWmf->setMatrix(m, true);
            }
            mReadWmf->drawImage(leftDst, topDst, bmpSrc, leftSrc, topSrc, widthSrc, heightSrc);
            mReadWmf->restore();
        }
    } else {
        kDebug(31000) << "KoWmfReadPrivate::dibBitBlt without image not implemented";
    }
}


void KoWmfReadPrivate::dibStretchBlt(quint32 size, QDataStream& stream)
{
    quint32 raster;
    qint16  topSrc, leftSrc, widthSrc, heightSrc;
    qint16  topDst, leftDst, widthDst, heightDst;
    QImage   bmpSrc;

    stream >> raster;
    stream >> heightSrc >> widthSrc >> topSrc >> leftSrc;
    stream >> heightDst >> widthDst >> topDst >> leftDst;

    if (dibToBmp(bmpSrc, stream, (size - 13) * 2)) {
        mReadWmf->setCompositionMode(winToQtComposition(raster));

        mReadWmf->save();
        if (widthDst < 0) {
            // negative width => horizontal flip
            QMatrix m(-1.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F);
            mReadWmf->setMatrix(m, true);
        }
        if (heightDst < 0) {
            // negative height => vertical flip
            QMatrix m(1.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F);
            mReadWmf->setMatrix(m, true);
        }
        bmpSrc = bmpSrc.copy(leftSrc, topSrc, widthSrc, heightSrc);
        // TODO: scale the bitmap : QImage::scale(widthDst, heightDst)
        // is actually too slow

        mReadWmf->drawImage(leftDst, topDst, bmpSrc);
        mReadWmf->restore();
    }
}


void KoWmfReadPrivate::stretchDib(quint32 size, QDataStream& stream)
{
    quint32 raster;
    qint16  arg, topSrc, leftSrc, widthSrc, heightSrc;
    qint16  topDst, leftDst, widthDst, heightDst;
    QImage   bmpSrc;

    stream >> raster >> arg;
    stream >> heightSrc >> widthSrc >> topSrc >> leftSrc;
    stream >> heightDst >> widthDst >> topDst >> leftDst;

    if (dibToBmp(bmpSrc, stream, (size - 14) * 2)) {
        mReadWmf->setCompositionMode(winToQtComposition(raster));

        mReadWmf->save();
        if (widthDst < 0) {
            // negative width => horizontal flip
            QMatrix m(-1.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F);
            mReadWmf->setMatrix(m, true);
        }
        if (heightDst < 0) {
            // negative height => vertical flip
            QMatrix m(1.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F);
            mReadWmf->setMatrix(m, true);
        }
        bmpSrc = bmpSrc.copy(leftSrc, topSrc, widthSrc, heightSrc);
        // TODO: scale the bitmap ( QImage::scale(param[ 8 ], param[ 7 ]) is actually too slow )

        mReadWmf->drawImage(leftDst, topDst, bmpSrc);
        mReadWmf->restore();
    }
}


void KoWmfReadPrivate::dibCreatePatternBrush(quint32 size, QDataStream& stream)
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
            kDebug(31000) << "KoWmfReadPrivate::dibCreatePatternBrush : incorrect DIB image";
        }
    }
}


void KoWmfReadPrivate::patBlt(quint32, QDataStream& stream)
{
    quint32 rasterOperation;
    quint16 height, width;
    qint16  y, x;

    stream >> rasterOperation;
    stream >> height >> width;
    stream >> y >> x;

    //kDebug(31000) << "patBlt record" << hex << rasterOperation << dec
    //              << x << y << width << height;

    mReadWmf->patBlt(x, y, width, height, rasterOperation);
}



//-----------------------------------------------------------------------------
// Object handle

void KoWmfReadPrivate::selectObject(quint32, QDataStream& stream)
{
    quint16 idx;

    stream >> idx;
    if ((idx < mNbrObject) && (mObjHandleTab[ idx ] != 0))
        mObjHandleTab[ idx ]->apply(mReadWmf);
    else
        kDebug(31000) << "KoWmfReadPrivate::selectObject : selection of an empty object";
}


void KoWmfReadPrivate::deleteObject(quint32, QDataStream& stream)
{
    quint16 idx;

    stream >> idx;
    deleteHandle(idx);
}


void KoWmfReadPrivate::createEmptyObject()
{
    // allocation of an empty object (to keep object counting in sync)
    KoWmfPenHandle* handle = new KoWmfPenHandle;

    addHandle(handle);
}


void KoWmfReadPrivate::createBrushIndirect(quint32, QDataStream& stream)
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
                kDebug(31000) << "KoWmfReadPrivate::createBrushIndirect: invalid hatched brush" << arg2;
                style = Qt::SolidPattern;
            }
        } else {
            if (sty < 9)
                style = koWmfStyleBrush[ sty ];
            else {
                kDebug(31000) << "KoWmfReadPrivate::createBrushIndirect: invalid brush" << sty;
                style = Qt::SolidPattern;
            }
        }
        handle->brush.setStyle(style);
        handle->brush.setColor(qtColor(color));
    }
}


void KoWmfReadPrivate::createPenIndirect(quint32, QDataStream& stream)
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
            kDebug(31000) << "KoWmfReadPrivate::createPenIndirect: invalid pen" << style;

        quint16 capStyle = (style & PenCapMask) >> 8;
        if (capStyle < 3)
            handle->pen.setCapStyle(koWmfCapStylePen[ capStyle ]);
        else
            kDebug(31000) << "KoWmfReadPrivate::createPenIndirect: invalid pen cap style" << style;

        quint16 joinStyle = (style & PenJoinMask) >> 12;
        if (joinStyle < 3)
            handle->pen.setJoinStyle(koWmfJoinStylePen[ joinStyle ]);
        else
            kDebug(31000) << "KoWmfReadPrivate::createPenIndirect: invalid pen join style" << style;

        handle->pen.setColor(qtColor(color));
        handle->pen.setWidth(width);
    }
}


void KoWmfReadPrivate::createFontIndirect(quint32 size, QDataStream& stream)
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

        kDebug(31000) << height << width << weight << property;
        // text rotation (in 1/10 degree)
        // TODO: memorisation of rotation in object Font
        mTextRotation = -rotation / 10;
        handle->font.setFixedPitch(((fixedPitch & 0x01) == 0));

        // A negative width means to use device units.  This is irrelevant for us here.
        height = qAbs(height);
        // FIXME: For some reason this value needs to be multiplied by
        //        a factor.  0.6 seems to give a good result, but why??
        handle->font.setPointSize(height * 6 / 10);
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

void KoWmfReadPrivate::end(quint32, QDataStream&)
{
}

quint16 KoWmfReadPrivate::calcCheckSum(WmfPlaceableHeader* apmfh)
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


void KoWmfReadPrivate::notyet(quint32, QDataStream&)
{
    if (mNbrFunc) {
        kDebug(31000) << "unimplemented";
    }
}

void KoWmfReadPrivate::region(quint32, QDataStream&)
{
    if (mNbrFunc) {
        kDebug(31000) << "region : unimplemented";
    }
}

void KoWmfReadPrivate::palette(quint32, QDataStream&)
{
    if (mNbrFunc) {
        kDebug(31000) << "palette : unimplemented";
    }
}

void KoWmfReadPrivate::escape(quint32, QDataStream&)
{
    if (mNbrFunc) {
        kDebug(31000) << "escape : unimplemented";
    }
}

void KoWmfReadPrivate::setRelAbs(quint32, QDataStream&)
{
    if (mNbrFunc) {
        kDebug(31000) << "setRelAbs : unimplemented";
    }
}

void KoWmfReadPrivate::setMapMode(quint32, QDataStream&)
{
    if (mNbrFunc) {
        kDebug(31000) << "setMapMode : unimplemented";
    }
}

void KoWmfReadPrivate::extFloodFill(quint32, QDataStream&)
{
    if (mNbrFunc) {
        kDebug(31000) << "extFloodFill : unimplemented";
    }
}

void KoWmfReadPrivate::setLayout(quint32, QDataStream &stream)
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

void KoWmfReadPrivate::startDoc(quint32, QDataStream&)
{
    if (mNbrFunc) {
        kDebug(31000) << "startDoc : unimplemented";
    }
}

void KoWmfReadPrivate::startPage(quint32, QDataStream&)
{
    if (mNbrFunc) {
        kDebug(31000) << "startPage : unimplemented";
    }
}

void KoWmfReadPrivate::endDoc(quint32, QDataStream&)
{
    if (mNbrFunc) {
        kDebug(31000) << "endDoc : unimplemented";
    }
}

void KoWmfReadPrivate::endPage(quint32, QDataStream&)
{
    if (mNbrFunc) {
        kDebug(31000) << "endPage : unimplemented";
    }
}

void KoWmfReadPrivate::resetDC(quint32, QDataStream&)
{
    if (mNbrFunc) {
        kDebug(31000) << "resetDC : unimplemented";
    }
}

void KoWmfReadPrivate::bitBlt(quint32, QDataStream&)
{
    if (mNbrFunc) {
        kDebug(31000) << "bitBlt : unimplemented";
    }
}

void KoWmfReadPrivate::setDibToDev(quint32, QDataStream&)
{
    if (mNbrFunc) {
        kDebug(31000) << "setDibToDev : unimplemented";
    }
}

void KoWmfReadPrivate::createBrush(quint32, QDataStream&)
{
    if (mNbrFunc) {
        kDebug(31000) << "createBrush : unimplemented";
    }
}

void KoWmfReadPrivate::createPatternBrush(quint32, QDataStream&)
{
    if (mNbrFunc) {
        kDebug(31000) << "createPatternBrush : unimplemented";
    }
}

void KoWmfReadPrivate::createBitmap(quint32, QDataStream&)
{
    if (mNbrFunc) {
        kDebug(31000) << "createBitmap : unimplemented";
    }
}

void KoWmfReadPrivate::createBitmapIndirect(quint32, QDataStream&)
{
    createEmptyObject();
    if (mNbrFunc) {
        kDebug(31000) << "createBitmapIndirect : unimplemented";
    }
}

void KoWmfReadPrivate::createPalette(quint32, QDataStream&)
{
    createEmptyObject();
    if (mNbrFunc) {
        kDebug(31000) << "createPalette : unimplemented";
    }
}

void KoWmfReadPrivate::createRegion(quint32, QDataStream&)
{
    createEmptyObject();
    if (mNbrFunc) {
        kDebug(31000) << "createRegion : unimplemented";
    }
}



//-----------------------------------------------------------------------------
// Utilities and conversion Wmf -> Qt

bool KoWmfReadPrivate::addHandle(KoWmfHandle* handle)
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
        kDebug(31000) << "KoWmfReadPrivate::addHandle : stack overflow = broken file !";
        return false;
    }
}


void KoWmfReadPrivate::deleteHandle(int idx)
{
    if ((idx < mNbrObject) && (mObjHandleTab[idx] != 0)) {
        delete mObjHandleTab[ idx ];
        mObjHandleTab[ idx ] = 0;
    } else {
        kDebug(31000) << "KoWmfReadPrivate::deletehandle() : bad index number";
    }
}


void KoWmfReadPrivate::pointArray(QDataStream& stream, QPolygon& pa)
{
    qint16 left, top;
    int  i, max;

    for (i = 0, max = pa.size() ; i < max ; i++) {
        stream >> left >> top;
        pa.setPoint(i, left, top);
    }
}


void KoWmfReadPrivate::xyToAngle(int xStart, int yStart, int xEnd, int yEnd, int& angleStart, int& angleLength)
{
    double aStart, aLength;

    aStart = atan2((double)yStart, (double)xStart);
    aLength = atan2((double)yEnd, (double)xEnd) - aStart;

    angleStart = (int)((aStart * 2880) / 3.14166);
    angleLength = (int)((aLength * 2880) / 3.14166);
    if (angleLength < 0) angleLength = 5760 + angleLength;
}


QPainter::CompositionMode KoWmfReadPrivate::winToQtComposition(quint16 param) const
{
    if (param < 17)
        return koWmfOpTab16[ param ];
    else
        return QPainter::CompositionMode_Source;
}


QPainter::CompositionMode  KoWmfReadPrivate::winToQtComposition(quint32 param) const
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


bool KoWmfReadPrivate::dibToBmp(QImage& bmp, QDataStream& stream, quint32 size)
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
        kDebug(31000) << "KoWmfReadPrivate::dibToBmp: invalid bitmap";
        return false;
    } else {
        return true;
    }
}

