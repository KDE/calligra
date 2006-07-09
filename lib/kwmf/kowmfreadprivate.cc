/* This file is part of the KDE libraries
 * Copyright (c) 1998 Stefan Taferner
 *                    2001/2003 thierry lorthiois (lorthioist@wanadoo.fr)
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

#include <math.h>
#include <qfileinfo.h>
#include <qimage.h>
#include <qwmatrix.h>
#include <qptrlist.h>
#include <qpointarray.h>
#include <qdatastream.h>
#include <kdebug.h>

#include "kowmfreadprivate.h"
#include "kowmfread.h"


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
    if ( mObjHandleTab != 0 ) {
        for ( int i=0 ; i < mNbrObject ; i++ ) {
            if ( mObjHandleTab[i] != 0 )
                delete mObjHandleTab[i];
        }
        delete[] mObjHandleTab;
    }
    if ( mBuffer != 0 ) {
        mBuffer->close();
        delete mBuffer;
    }
}


bool KoWmfReadPrivate::load( const QByteArray& array )
{
    // delete previous buffer
    if ( mBuffer != 0 ) {
        mBuffer->close();
        delete mBuffer;
    }

    // load into buffer
    mBuffer = new QBuffer( array );
    mBuffer->open( IO_ReadOnly );

    // read and check the header
    WmfEnhMetaHeader eheader;
    WmfMetaHeader header;
    WmfPlaceableHeader pheader;
    unsigned short checksum;
    int filePos;

    QDataStream st( mBuffer );
    st.setByteOrder( QDataStream::LittleEndian );
    mStackOverflow = mWinding = false;
    mTextAlign = mTextRotation = 0;
    mTextColor = Qt::black;
    mValid = false;
    mStandard = false;
    mPlaceable = false;
    mEnhanced = false;

    //----- Read placeable metafile header
    st >> pheader.key;
    if ( pheader.key==( Q_UINT32 )APMHEADER_KEY ) {
        mPlaceable = true;
        st >> pheader.handle;
        st >> pheader.left;
        st >> pheader.top;
        st >> pheader.right;
        st >> pheader.bottom;
        st >> pheader.inch;
        st >> pheader.reserved;
        st >> pheader.checksum;
        checksum = calcCheckSum( &pheader );
        if ( pheader.checksum!=checksum ) {
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
        mBBox.setLeft( pheader.left );
        mBBox.setTop( pheader.top );
        mBBox.setRight( pheader.right );
        mBBox.setBottom( pheader.bottom );
        mDpi = pheader.inch;
    }
    else {
        mBuffer->at( 0 );
        //----- Read as enhanced metafile header
        filePos = mBuffer->at();
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
        if ( eheader.signature==ENHMETA_SIGNATURE ) {
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
        }
        else {
            //----- Read as standard metafile header
            mStandard = true;
            mBuffer->at( filePos );
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
    mOffsetFirstRecord = mBuffer->at();

    //----- Test header validity
    if ( ((header.headerSize == 9) && (header.numOfParameters == 0)) || (mPlaceable) ) {
        // valid wmf file
        mValid = true;
    }
    else {
        kdDebug() << "KoWmfReadPrivate : incorrect file format !" << endl;
    }

    // check bounding rectangle for standard meta file
    if ( (mValid) && (mStandard) ) {
        Q_UINT16 numFunction = 1;
        Q_UINT32 size;
        bool firstOrg=true, firstExt=true;

        // search functions setWindowOrg and setWindowExt
        while ( numFunction ) {
            filePos = mBuffer->at();
            st >> size >> numFunction;

            if ( size == 0 ) {
                kdDebug() << "KoWmfReadPrivate : incorrect file!" << endl;
                mValid = 0;
                break;
            }

            numFunction &= 0xFF;
            if ( numFunction == 11 ) {
                Q_INT16 top, left;

                st >> top >> left;
                if ( firstOrg ) {
                    firstOrg = false;
                    mBBox.setLeft( left );
                    mBBox.setTop( top );
                }
                else {
                    if ( left < mBBox.left() ) mBBox.setLeft( left );
                    if ( top < mBBox.top() ) mBBox.setTop( top );
                }
            }
            if ( numFunction == 12 ) {
                Q_INT16 width, height;

                st >> height >> width;
                if ( width < 0 ) width = -width;
                if ( height < 0 ) height = -height;
                if ( firstExt ) {
                    firstExt = false;
                    mBBox.setWidth( width );
                    mBBox.setHeight( height );
                }
                else {
                    if ( width > mBBox.width() ) mBBox.setWidth( width );
                    if ( height > mBBox.height() ) mBBox.setHeight( height );
                }
            }
            mBuffer->at( filePos + (size<<1) );
            // ## shouldn't we break from the loop as soon as we found what we were looking for?
        }
    }

    return (mValid);
}


bool KoWmfReadPrivate::play( KoWmfRead* readWmf )
{
    if ( !(mValid) ) {
        kdDebug() << "KoWmfReadPrivate::play : invalid WMF file" << endl;
        return false;
    }

    if ( mNbrFunc ) {
        if ( (mStandard) ) {
            kdDebug() << "Standard : " << mBBox.left() << "  "  << mBBox.top() << "  "  << mBBox.width() << "  "  << mBBox.height() << endl;
        }
        else {
            kdDebug() << "DPI : " << mDpi << " : " << mBBox.left() << "  "  << mBBox.top() << "  "  << mBBox.width() << "  "  << mBBox.height() << endl;
            kdDebug() << "inch : " << mBBox.width()/mDpi << "  " << mBBox.height()/mDpi << endl;
            kdDebug() << "mm : " << mBBox.width()*25.4/mDpi << "  " << mBBox.height()*25.4/mDpi << endl;
        }
        kdDebug() << mValid << "  " << mStandard << "  " << mPlaceable << endl;
    }

    // stack of handle
    mObjHandleTab = new KoWmfHandle* [ mNbrObject ];
    for ( int i=0; i < mNbrObject ; i++ ) {
        mObjHandleTab[ i ] = 0;
    }

    Q_UINT16 numFunction;
    Q_UINT32 size;
    int  bufferOffset, j;

    // buffer with functions
    QDataStream st( mBuffer );
    st.setByteOrder( QDataStream::LittleEndian );

    mReadWmf = readWmf;
    mWindow = mBBox;
    if ( mReadWmf->begin() ) {
        // play wmf functions
        mBuffer->at( mOffsetFirstRecord );
        numFunction = j = 1;
        mWinding = false;

        while ( ( numFunction ) && ( !mStackOverflow ) ) {
            bufferOffset = mBuffer->at();
            st >> size >> numFunction;

            /**
             * mapping between n° function and index of table 'metaFuncTab'
             * lower 8 digits of the function => entry in the table
             */
            numFunction &= 0xFF;
            if ( numFunction > 0x5F ) {
                numFunction -= 0x90;
            }
            if ( (numFunction > 111) || (koWmfFunc[ numFunction ].method == 0) ) {
                // function outside WMF specification
                kdDebug() << "KoWmfReadPrivate::paint : BROKEN WMF file" << endl;
                mValid = false;    
                break;
            }

            if ( mNbrFunc ) {
                // debug mode
                if ( (j+12) > mNbrFunc ) {
                    // output last 12 functions
                    int offBuff = mBuffer->at();
                    Q_UINT16 param;

                    kdDebug() <<  j << " : " << numFunction << " : ";
                    for ( Q_UINT16 i=0 ; i < (size-3) ; i++ ) {
                        st >> param;
                        kdDebug() <<  param << " ";
                    }
                    kdDebug() <<  endl;
                    mBuffer->at( offBuff );
                }
                if ( j >= mNbrFunc ) {
                    break;
                }
                j++;
            }

            // execute the function
            (this->*koWmfFunc[ numFunction ].method)( size, st );

            mBuffer->at( bufferOffset + (size<<1) );
        }

        mReadWmf->end();
    }

    for ( int i=0 ; i < mNbrObject ; i++ ) {
        if ( mObjHandleTab[ i ] != 0 )
            delete mObjHandleTab[ i ];
    }
    delete[] mObjHandleTab;
    mObjHandleTab = 0;

    return true;
}


//-----------------------------------------------------------------------------
// Metafile painter methods

void KoWmfReadPrivate::setWindowOrg( Q_UINT32, QDataStream& stream )
{
    Q_INT16 top, left;

    stream >> top >> left;
    mReadWmf->setWindowOrg( left, top );
    mWindow.setLeft( left );
    mWindow.setTop( top );
//    kdDebug() << "Org : (" << left << ", "  << top << ")  " << endl;
}

/*  TODO : deeper look in negative width and height
*/

void KoWmfReadPrivate::setWindowExt( Q_UINT32, QDataStream& stream )
{
    Q_INT16 width, height;

    // negative value allowed for width and height
    stream >> height >> width;
    mReadWmf->setWindowExt( width, height );
    mWindow.setWidth( width );
    mWindow.setHeight( height );
//    kdDebug() << "Ext : (" << width << ", "  << height << ") "<< endl;
}


void KoWmfReadPrivate::OffsetWindowOrg( Q_UINT32, QDataStream &stream )
{
    Q_INT16 offTop, offLeft;
    
    stream >> offTop >> offLeft;
    mReadWmf->setWindowOrg( mWindow.left() + offLeft, mWindow.top() + offTop );
    mWindow.setLeft( mWindow.left() + offLeft );
    mWindow.setTop( mWindow.top() + offTop );
}


void KoWmfReadPrivate::ScaleWindowExt( Q_UINT32, QDataStream &stream )
{
    Q_INT16 width, height;
    Q_INT16 heightDenom, heightNum, widthDenom, widthNum;
    
    stream >> heightDenom >> heightNum >> widthDenom >> widthNum;

    if ( ( widthDenom != 0 ) && ( heightDenom != 0 ) ) {
        width = (mWindow.width() * widthNum) / widthDenom;
        height = (mWindow.height() * heightNum) / heightDenom;
        mReadWmf->setWindowExt( width, height );
        mWindow.setWidth( width );
        mWindow.setHeight( height );
    }
//    kdDebug() << "KoWmfReadPrivate::ScaleWindowExt : " << widthDenom << "  " << heightDenom << endl;
}


//-----------------------------------------------------------------------------
// Drawing

void KoWmfReadPrivate::lineTo( Q_UINT32, QDataStream& stream )
{
    Q_INT16 top, left;

    stream >> top >> left;
    mReadWmf->lineTo( left, top );
}


void KoWmfReadPrivate::moveTo( Q_UINT32, QDataStream& stream )
{
    Q_INT16 top, left;

    stream >> top >> left;
    mReadWmf->moveTo( left, top );
}


void KoWmfReadPrivate::ellipse( Q_UINT32, QDataStream& stream )
{
    Q_INT16 top, left, right, bottom;

    stream >> bottom >> right >> top >> left;
    mReadWmf->drawEllipse( left, top, right-left, bottom-top );
}


void KoWmfReadPrivate::polygon( Q_UINT32, QDataStream& stream )
{
    Q_UINT16 num;

    stream >> num;

    QPointArray pa( num );

    pointArray( stream, pa );
    mReadWmf->drawPolygon( pa, mWinding );
}


void KoWmfReadPrivate::polyPolygon( Q_UINT32, QDataStream& stream )
{
    Q_UINT16 numberPoly;
    Q_UINT16 sizePoly;
    QPtrList<QPointArray> listPa;

    stream >> numberPoly;
    
    listPa.setAutoDelete( true );
    for ( int i=0 ; i < numberPoly ; i++ ) {
        stream >> sizePoly;
        listPa.append( new QPointArray( sizePoly ) );
    }

    // list of point array
    QPointArray *pa;
    for ( pa = listPa.first() ; pa ; pa = listPa.next() ) {
        pointArray( stream, *pa );
    }

    // draw polygon's
    mReadWmf->drawPolyPolygon( listPa, mWinding );
    listPa.clear();
}


void KoWmfReadPrivate::polyline( Q_UINT32, QDataStream& stream )
{
    Q_UINT16 num;

    stream >> num;
    QPointArray pa( num );

    pointArray( stream, pa );
    mReadWmf->drawPolyline( pa );
}


void KoWmfReadPrivate::rectangle( Q_UINT32, QDataStream& stream )
{
    Q_INT16 top, left, right, bottom;

    stream >> bottom >> right >> top >> left;
    mReadWmf->drawRect( left, top, right-left, bottom-top );
}


void KoWmfReadPrivate::roundRect( Q_UINT32, QDataStream& stream )
{
    int xRnd = 0, yRnd = 0;
    Q_UINT16 widthCorner, heightCorner;
    Q_INT16  top, left, right, bottom;

    stream >> heightCorner >> widthCorner;
    stream >> bottom >> right >> top >> left;

    // convert (widthCorner, heightCorner) in percentage
    if ( (right - left) != 0  )
        xRnd = (widthCorner * 100) / (right - left);
    if ( (bottom - top) != 0  )
        yRnd = (heightCorner * 100) / (bottom - top);

    mReadWmf->drawRoundRect( left, top, right-left, bottom-top, xRnd, yRnd );
}


void KoWmfReadPrivate::arc( Q_UINT32, QDataStream& stream )
{
    int xCenter, yCenter, angleStart, aLength;
    Q_INT16  topEnd, leftEnd, topStart, leftStart;
    Q_INT16  top, left, right, bottom;

    stream >> topEnd >> leftEnd >> topStart >> leftStart;
    stream >> bottom >> right >> top >> left;

    xCenter = left + ((right-left) / 2);
    yCenter = top + ((bottom-top) / 2);
    xyToAngle ( leftStart-xCenter, yCenter-topStart, leftEnd-xCenter, yCenter-topEnd, angleStart, aLength );

    mReadWmf->drawArc( left, top, right-left, bottom-top, angleStart, aLength);
}


void KoWmfReadPrivate::chord( Q_UINT32, QDataStream& stream )
{
    int xCenter, yCenter, angleStart, aLength;
    Q_INT16  topEnd, leftEnd, topStart, leftStart;
    Q_INT16  top, left, right, bottom;

    stream >> topEnd >> leftEnd >> topStart >> leftStart;
    stream >> bottom >> right >> top >> left;

    xCenter = left + ((right-left) / 2);
    yCenter = top + ((bottom-top) / 2);
    xyToAngle ( leftStart-xCenter, yCenter-topStart, leftEnd-xCenter, yCenter-topEnd, angleStart, aLength );

    mReadWmf->drawChord( left, top, right-left, bottom-top, angleStart, aLength);
}


void KoWmfReadPrivate::pie( Q_UINT32, QDataStream& stream )
{
    int xCenter, yCenter, angleStart, aLength;
    Q_INT16  topEnd, leftEnd, topStart, leftStart;
    Q_INT16  top, left, right, bottom;

    stream >> topEnd >> leftEnd >> topStart >> leftStart;
    stream >> bottom >> right >> top >> left;

    xCenter = left + ((right-left) / 2);
    yCenter = top + ((bottom-top) / 2);
    xyToAngle ( leftStart-xCenter, yCenter-topStart, leftEnd-xCenter, yCenter-topEnd, angleStart, aLength );

    mReadWmf->drawPie( left, top, right-left, bottom-top, angleStart, aLength);
}


void KoWmfReadPrivate::setPolyFillMode( Q_UINT32, QDataStream& stream )
{
    Q_UINT16 winding;

    stream >> winding;
    mWinding = (winding != 0);
}


void KoWmfReadPrivate::setBkColor( Q_UINT32, QDataStream& stream )
{
    Q_UINT32 color;

    stream >> color;
    mReadWmf->setBackgroundColor( qtColor( color ) );
}


void KoWmfReadPrivate::setBkMode( Q_UINT32, QDataStream& stream )
{
    Q_UINT16 bkMode;

    stream >> bkMode;
    if ( bkMode == 1 )
        mReadWmf->setBackgroundMode( Qt::TransparentMode );
    else
        mReadWmf->setBackgroundMode( Qt::OpaqueMode );
}


void KoWmfReadPrivate::setPixel( Q_UINT32, QDataStream& stream )
{
    Q_INT16  top, left;
    Q_UINT32 color;

    stream >> color >> top >> left;

    QPen oldPen = mReadWmf->pen();    
    QPen pen = oldPen;    
    pen.setColor( qtColor( color ) );
    mReadWmf->setPen( pen );
    mReadWmf->moveTo( left, top );
    mReadWmf->lineTo( left, top );
    mReadWmf->setPen( oldPen );
}


void KoWmfReadPrivate::setRop( Q_UINT32, QDataStream& stream )
{
    Q_UINT16  rop;

    stream >> rop;
    mReadWmf->setRasterOp( winToQtRaster( rop ) );
}


void KoWmfReadPrivate::saveDC( Q_UINT32, QDataStream& )
{
    mReadWmf->save();
}


void KoWmfReadPrivate::restoreDC( Q_UINT32, QDataStream& stream )
{
    Q_INT16  num;

    stream >> num;
    for ( int i=0; i > num ; i-- )
        mReadWmf->restore();
}


void KoWmfReadPrivate::intersectClipRect( Q_UINT32, QDataStream& stream )
{
    Q_INT16 top, left, right, bottom;

    stream >> bottom >> right >> top >> left;

    QRegion region = mReadWmf->clipRegion();
    QRegion newRegion( left, top, right-left, bottom-top );
    if ( region.isEmpty() ) {
        region = newRegion;
    }
    else {
        region = region.intersect( newRegion );
    }

    mReadWmf->setClipRegion( region );
}


void KoWmfReadPrivate::excludeClipRect( Q_UINT32, QDataStream& stream )
{
    Q_INT16 top, left, right, bottom;

    stream >> bottom >> right >> top >> left;

    QRegion region = mReadWmf->clipRegion();
    QRegion newRegion( left, top, right-left, bottom-top );
    if ( region.isEmpty() ) {
        region = newRegion;
    }
    else {
        region = region.subtract( newRegion );
    }

    mReadWmf->setClipRegion( region );
}


//-----------------------------------------------------------------------------
// Text

void KoWmfReadPrivate::setTextColor( Q_UINT32, QDataStream& stream )
{
    Q_UINT32 color;
    
    stream >> color;
    mTextColor = qtColor( color );
}


void KoWmfReadPrivate::setTextAlign( Q_UINT32, QDataStream& stream )
{
    stream >> mTextAlign;
}


void KoWmfReadPrivate::textOut( Q_UINT32, QDataStream& )
{
    if ( mNbrFunc ) {
        kdDebug() << "textOut : unimplemented " << endl;
    }
}


void KoWmfReadPrivate::extTextOut( Q_UINT32 , QDataStream&  )
{
    if ( mNbrFunc ) {
        kdDebug() << "extTextOut : unimplemented " << endl;
    }
}



//-----------------------------------------------------------------------------
// Bitmap

void KoWmfReadPrivate::SetStretchBltMode( Q_UINT32, QDataStream& )
{
    if ( mNbrFunc ) {
        kdDebug() << "SetStretchBltMode : unimplemented " << endl;
    }
}


void KoWmfReadPrivate::dibBitBlt( Q_UINT32 size, QDataStream& stream )
{
    Q_UINT32 raster;
    Q_INT16  topSrc, leftSrc, widthSrc, heightSrc;
    Q_INT16  topDst, leftDst;

    stream >> raster;
    stream >> topSrc >> leftSrc >> heightSrc >> widthSrc;
    stream >> topDst >> leftDst;

    if ( size > 11 ) {    // DIB image
        QImage bmpSrc;

        if ( dibToBmp( bmpSrc, stream, (size - 11) * 2 ) ) {
            mReadWmf->setRasterOp( winToQtRaster( raster )  );

            mReadWmf->save();
            if ( widthSrc < 0 ) {
                // negative width => horizontal flip
                QWMatrix m( -1.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F );
                mReadWmf->setWorldMatrix( m, true );
            }
            if ( heightSrc < 0 ) {
                // negative height => vertical flip
                QWMatrix m( 1.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F );
                mReadWmf->setWorldMatrix( m, true );
            }
            mReadWmf->drawImage( leftDst, topDst, bmpSrc, leftSrc, topSrc, widthSrc, heightSrc );
            mReadWmf->restore();
        }
    }
    else {
        kdDebug() << "KoWmfReadPrivate::dibBitBlt without image not implemented " << endl;
    }
}


void KoWmfReadPrivate::dibStretchBlt( Q_UINT32 size, QDataStream& stream )
{
    Q_UINT32 raster;
    Q_INT16  topSrc, leftSrc, widthSrc, heightSrc;
    Q_INT16  topDst, leftDst, widthDst, heightDst;
    QImage   bmpSrc;

    stream >> raster;
    stream >> heightSrc >> widthSrc >> topSrc >> leftSrc;
    stream >> heightDst >> widthDst >> topDst >> leftDst;

    if ( dibToBmp( bmpSrc, stream, (size - 13) * 2 ) ) {
        mReadWmf->setRasterOp( winToQtRaster( raster )  );

        mReadWmf->save();
        if ( widthDst < 0 ) {
            // negative width => horizontal flip
            QWMatrix m( -1.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F );
            mReadWmf->setWorldMatrix( m, true );
        }
        if ( heightDst < 0 ) {
            // negative height => vertical flip
            QWMatrix m( 1.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F );
            mReadWmf->setWorldMatrix( m, true );
        }
        bmpSrc = bmpSrc.copy( leftSrc, topSrc, widthSrc, heightSrc );
        // TODO: scale the bitmap : QImage::scale(widthDst, heightDst)
        // is actually too slow

        mReadWmf->drawImage( leftDst, topDst, bmpSrc );
        mReadWmf->restore();
    }
}


void KoWmfReadPrivate::stretchDib( Q_UINT32 size, QDataStream& stream )
{
    Q_UINT32 raster;
    Q_INT16  arg, topSrc, leftSrc, widthSrc, heightSrc;
    Q_INT16  topDst, leftDst, widthDst, heightDst;
    QImage   bmpSrc;

    stream >> raster >> arg;
    stream >> heightSrc >> widthSrc >> topSrc >> leftSrc;
    stream >> heightDst >> widthDst >> topDst >> leftDst;

    if ( dibToBmp( bmpSrc, stream, (size - 14) * 2 ) ) {
        mReadWmf->setRasterOp( winToQtRaster( raster )  );

        mReadWmf->save();
        if ( widthDst < 0 ) {
            // negative width => horizontal flip
            QWMatrix m( -1.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F );
            mReadWmf->setWorldMatrix( m, true );
        }
        if ( heightDst < 0 ) {
            // negative height => vertical flip
            QWMatrix m( 1.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F );
            mReadWmf->setWorldMatrix( m, true );
        }
        bmpSrc = bmpSrc.copy( leftSrc, topSrc, widthSrc, heightSrc );
        // TODO: scale the bitmap ( QImage::scale(param[ 8 ], param[ 7 ]) is actually too slow )

        mReadWmf->drawImage( leftDst, topDst, bmpSrc );
        mReadWmf->restore();
    }
}


void KoWmfReadPrivate::dibCreatePatternBrush( Q_UINT32 size, QDataStream& stream )
{
    KoWmfPatternBrushHandle* handle = new KoWmfPatternBrushHandle;
    
    if ( addHandle( handle ) ) {
        Q_UINT32 arg;
        QImage bmpSrc;

        stream >> arg;
        if ( dibToBmp( bmpSrc, stream, (size - 5) * 2 ) ) {
            handle->image = bmpSrc;
            handle->brush.setPixmap( handle->image );
        }
        else {
            kdDebug() << "KoWmfReadPrivate::dibCreatePatternBrush : incorrect DIB image" << endl;
        }
    }
}


//-----------------------------------------------------------------------------
// Object handle

void KoWmfReadPrivate::selectObject( Q_UINT32, QDataStream& stream )
{
    Q_UINT16 idx;

    stream >> idx;
    if ( (idx < mNbrObject) && (mObjHandleTab[ idx ] != 0) )
        mObjHandleTab[ idx ]->apply( mReadWmf );
    else
        kdDebug() << "KoWmfReadPrivate::selectObject : selection of an empty object" << endl;
}


void KoWmfReadPrivate::deleteObject( Q_UINT32, QDataStream& stream )
{
    Q_UINT16 idx;

    stream >> idx;
    deleteHandle( idx );
}


void KoWmfReadPrivate::createEmptyObject()
{
    // allocation of an empty object (to keep object counting in sync)
    KoWmfPenHandle* handle = new KoWmfPenHandle;

    addHandle( handle );
}


void KoWmfReadPrivate::createBrushIndirect( Q_UINT32, QDataStream& stream )
{
    Qt::BrushStyle style;
    Q_UINT16 sty, arg2;
    Q_UINT32 color;
    KoWmfBrushHandle* handle = new KoWmfBrushHandle;

    if ( addHandle( handle ) ) {
        stream >> sty >> color >> arg2;

        if ( sty == 2 ) {
            if ( arg2 < 6 )
                style = koWmfHatchedStyleBrush[ arg2 ];
            else
            {
                kdDebug() << "KoWmfReadPrivate::createBrushIndirect: invalid hatched brush " << arg2 << endl;
                style = Qt::SolidPattern;
            }
        }
        else {
            if ( sty < 9 )
                style = koWmfStyleBrush[ sty ];
            else {
                kdDebug() << "KoWmfReadPrivate::createBrushIndirect: invalid brush " << sty << endl;
                style = Qt::SolidPattern;
            }
        }
        handle->brush.setStyle( style );
        handle->brush.setColor( qtColor( color ) );
    }
}


void KoWmfReadPrivate::createPenIndirect( Q_UINT32, QDataStream& stream )
{
    // TODO: userStyle and alternateStyle
    Qt::PenStyle penStyle;
    Q_UINT32 color;
    Q_UINT16 style, width, arg;

    KoWmfPenHandle* handle = new KoWmfPenHandle;

    if ( addHandle( handle ) ) {
        stream >> style >> width >> arg >> color;

        if ( style < 7 )
            penStyle=koWmfStylePen[ style ];
        else {
            kdDebug() << "KoWmfReadPrivate::createPenIndirect: invalid pen " << style << endl;
            penStyle = Qt::SolidLine;
        }

        handle->pen.setStyle( penStyle );
        handle->pen.setColor( qtColor( color ) );
        handle->pen.setCapStyle( Qt::RoundCap );
        handle->pen.setWidth( width );
    }
}


void KoWmfReadPrivate::createFontIndirect( Q_UINT32 size, QDataStream& stream )
{
    Q_INT16  pointSize, rotation;
    Q_UINT16 weight, property, fixedPitch, arg;

    KoWmfFontHandle* handle = new KoWmfFontHandle;

    if ( addHandle( handle ) ) {
        stream >> pointSize >> arg;
        stream >> rotation >> arg;
        stream >> weight >> property >> arg >> arg;
        stream >> fixedPitch;

        // text rotation (in 1/10 degree)
        // TODO: memorisation of rotation in object Font
        mTextRotation = -rotation / 10;
        handle->font.setFixedPitch( ((fixedPitch & 0x01) == 0) );
        // TODO: investigation why some test case need -2
        // size of font in logical point
        handle->font.setPointSize( QABS(pointSize) - 2 );
        handle->font.setWeight( (weight >> 3) );
        handle->font.setItalic( (property & 0x01) );
        handle->font.setUnderline( (property & 0x100) );

        // font name
        int    maxChar = (size-12) * 2;
        char*  nameFont = new char[maxChar];
        stream.readRawBytes( nameFont, maxChar );
        handle->font.setFamily( nameFont );
        delete[] nameFont;
    }
}


//-----------------------------------------------------------------------------
// Misc functions

void KoWmfReadPrivate::end( Q_UINT32, QDataStream& )
{
}

Q_UINT16 KoWmfReadPrivate::calcCheckSum( WmfPlaceableHeader* apmfh )
{
    Q_UINT16*  lpWord;
    Q_UINT16   wResult, i;

    // Start with the first word
    wResult = *( lpWord = ( Q_UINT16* )( apmfh ) );
    // XOR in each of the other 9 words
    for( i=1; i<=9; i++ )
    {
        wResult ^= lpWord[ i ];
    }
    return wResult;
}


void KoWmfReadPrivate::notyet( Q_UINT32, QDataStream& )
{
    if ( mNbrFunc ) {
        kdDebug() << "unimplemented " << endl;
    }
}

void KoWmfReadPrivate::region( Q_UINT32, QDataStream& )
{
    if ( mNbrFunc ) {
        kdDebug() << "region : unimplemented " << endl;
    }
}

void KoWmfReadPrivate::palette( Q_UINT32, QDataStream& )
{
    if ( mNbrFunc ) {
        kdDebug() << "palette : unimplemented " << endl;
    }
}

void KoWmfReadPrivate::escape( Q_UINT32, QDataStream& )
{
    if ( mNbrFunc ) {
        kdDebug() << "escape : unimplemented " << endl;
    }
}

void KoWmfReadPrivate::setRelAbs( Q_UINT32, QDataStream& )
{
    if ( mNbrFunc ) {
        kdDebug() << "setRelAbs : unimplemented " << endl;
    }
}

void KoWmfReadPrivate::setMapMode( Q_UINT32, QDataStream& )
{
    if ( mNbrFunc ) {
        kdDebug() << "setMapMode : unimplemented " << endl;
    }
}

void KoWmfReadPrivate::extFloodFill( Q_UINT32, QDataStream&  )
{
    if ( mNbrFunc ) {
        kdDebug() << "extFloodFill : unimplemented " << endl;
    }
}

void KoWmfReadPrivate::startDoc( Q_UINT32, QDataStream&  )
{
    if ( mNbrFunc ) {
        kdDebug() << "startDoc : unimplemented " << endl;
    }
}

void KoWmfReadPrivate::startPage( Q_UINT32, QDataStream&  )
{
    if ( mNbrFunc ) {
        kdDebug() << "startPage : unimplemented " << endl;
    }
}

void KoWmfReadPrivate::endDoc( Q_UINT32, QDataStream&  )
{
    if ( mNbrFunc ) {
        kdDebug() << "endDoc : unimplemented " << endl;
    }
}

void KoWmfReadPrivate::endPage( Q_UINT32, QDataStream&  )
{
    if ( mNbrFunc ) {
        kdDebug() << "endPage : unimplemented " << endl;
    }
}

void KoWmfReadPrivate::resetDC( Q_UINT32, QDataStream&  )
{
    if ( mNbrFunc ) {
        kdDebug() << "resetDC : unimplemented " << endl;
    }
}

void KoWmfReadPrivate::bitBlt( Q_UINT32, QDataStream&  )
{
    if ( mNbrFunc ) {
        kdDebug() << "bitBlt : unimplemented " << endl;
    }
}

void KoWmfReadPrivate::setDibToDev( Q_UINT32, QDataStream&  )
{
    if ( mNbrFunc ) {
        kdDebug() << "setDibToDev : unimplemented " << endl;
    }
}

void KoWmfReadPrivate::createBrush( Q_UINT32, QDataStream&  )
{
    if ( mNbrFunc ) {
        kdDebug() << "createBrush : unimplemented " << endl;
    }
}

void KoWmfReadPrivate::createPatternBrush( Q_UINT32, QDataStream&  )
{
    if ( mNbrFunc ) {
        kdDebug() << "createPatternBrush : unimplemented " << endl;
    }
}

void KoWmfReadPrivate::createBitmap( Q_UINT32, QDataStream&  )
{
    if ( mNbrFunc ) {
        kdDebug() << "createBitmap : unimplemented " << endl;
    }
}

void KoWmfReadPrivate::createBitmapIndirect( Q_UINT32, QDataStream&  )
{
    createEmptyObject();
    if ( mNbrFunc ) {
        kdDebug() << "createBitmapIndirect : unimplemented " << endl;
    }
}

void KoWmfReadPrivate::createPalette( Q_UINT32, QDataStream&  )
{
    createEmptyObject();
    if ( mNbrFunc ) {
        kdDebug() << "createPalette : unimplemented " << endl;
    }
}

void KoWmfReadPrivate::createRegion( Q_UINT32, QDataStream&  )
{
    createEmptyObject();
    if ( mNbrFunc ) {
        kdDebug() << "createRegion : unimplemented " << endl;
    }
}



//-----------------------------------------------------------------------------
// Utilities and conversion Wmf -> Qt

bool KoWmfReadPrivate::addHandle( KoWmfHandle* handle )
{
    int idx;

    for ( idx=0; idx < mNbrObject ; idx++ ) {
        if ( mObjHandleTab[ idx ] == 0 )  break;
    }

    if ( idx < mNbrObject ) {
        mObjHandleTab[ idx ] = handle;
        return true;
    }
    else {
        delete handle;
        mStackOverflow = true;
        kdDebug() << "KoWmfReadPrivate::addHandle : stack overflow = broken file !" << endl;
        return false;
    }
}


void KoWmfReadPrivate::deleteHandle( int idx )
{
    if ( (idx < mNbrObject) && (mObjHandleTab[idx] != 0) ) {
        delete mObjHandleTab[ idx ];
        mObjHandleTab[ idx ] = 0;
    }
    else {
        kdDebug() << "KoWmfReadPrivate::deletehandle() : bad index number" << endl;
    }
}


void KoWmfReadPrivate::pointArray( QDataStream& stream, QPointArray& pa )
{
    Q_INT16 left, top;
    int  i, max;

    for ( i=0, max=pa.size() ; i < max ; i++ ) {
        stream >> left >> top;
        pa.setPoint( i, left, top );
    }
}


void KoWmfReadPrivate::xyToAngle( int xStart, int yStart, int xEnd, int yEnd, int& angleStart, int& angleLength )
{
    double aStart, aLength;

    aStart = atan2( yStart,  xStart );
    aLength = atan2( yEnd, xEnd ) - aStart;

    angleStart = (int)((aStart * 2880) / 3.14166);
    angleLength = (int)((aLength * 2880) / 3.14166);
    if ( angleLength < 0 ) angleLength = 5760 + angleLength;
}


Qt::RasterOp  KoWmfReadPrivate::winToQtRaster( Q_UINT16 param ) const
{
    if ( param < 17 )
        return koWmfOpTab16[ param ];
    else
        return Qt::CopyROP;
}


Qt::RasterOp  KoWmfReadPrivate::winToQtRaster( Q_UINT32 param ) const
{
    /* TODO: Ternary raster operations
    0x00C000CA  dest = (source AND pattern)
    0x00F00021  dest = pattern
    0x00FB0A09  dest = DPSnoo
    0x005A0049  dest = pattern XOR dest   */
    int i;

    for ( i=0 ; i < 15 ; i++ ) {
        if ( koWmfOpTab32[ i ].winRasterOp == param )  break;
    }

    if ( i < 15 )
        return koWmfOpTab32[ i ].qtRasterOp;
    else
        return Qt::CopyROP;
}


bool KoWmfReadPrivate::dibToBmp( QImage& bmp, QDataStream& stream, Q_UINT32 size )
{
    typedef struct _BMPFILEHEADER {
        Q_UINT16 bmType;
        Q_UINT32 bmSize;
        Q_UINT16 bmReserved1;
        Q_UINT16 bmReserved2;
        Q_UINT32 bmOffBits;
    }  BMPFILEHEADER;

    int sizeBmp = size + 14;

    QByteArray pattern( sizeBmp );       // BMP header and DIB data
    pattern.fill(0);
    stream.readRawBytes( &pattern[ 14 ], size );

    // add BMP header
    BMPFILEHEADER* bmpHeader;
    bmpHeader = (BMPFILEHEADER*)(pattern.data());
    bmpHeader->bmType = 0x4D42;
    bmpHeader->bmSize = sizeBmp;

//    if ( !bmp.loadFromData( (const uchar*)bmpHeader, pattern.size(), "BMP" ) ) {
    if ( !bmp.loadFromData( pattern, "BMP" ) ) {
        kdDebug() << "KoWmfReadPrivate::dibToBmp: invalid bitmap " << endl;
        return false;
    }
    else {
        return true;
    }
}

