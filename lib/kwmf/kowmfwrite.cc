/* This file is part of the KDE libraries
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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
*/

#include <math.h>
#include <qfile.h>
#include <qdatastream.h>

#include <kdebug.h>

#include "kowmfstruct.h"
#include "kowmfreadprivate.h"
#include "kowmfwrite.h"

/**
 * Private data
 */
class KoWmfWritePrivate
{
public:
    bool     mIsValid;
    QRect    mBBox;      // bounding rectangle
    int      mDpi;       // number of point per inch for the default size
    int      mMaxRecordSize;

    // memory allocation for WMF file
    QBuffer  *mBuffer;
    QDataStream mSt;
    int         mOffsetFirstRecord;
};



KoWmfWrite::KoWmfWrite() {
    d = new KoWmfWritePrivate;    
    d->mDpi = 1024;
    d->mIsValid = false;
    d->mMaxRecordSize = 0;

    // initialize memory buffer
    d->mBuffer = new QBuffer(  );
    d->mBuffer->open( IO_ReadWrite );
    d->mSt.setDevice( d->mBuffer );
    d->mSt.setByteOrder( QDataStream::LittleEndian );
}

KoWmfWrite::~KoWmfWrite() {
    d->mBuffer->close();
    delete d->mBuffer;
    delete d;
}


bool KoWmfWrite::save( const QString &fileName ) {
    QFile file( fileName );

    if ( !d->mIsValid ) {
        kdDebug() << "KWinMetaFile::save : Invalid WMF !" << endl;
        return false;
    }

    if ( !file.open( IO_WriteOnly ) )
    {
        kdDebug() << "KWinMetaFile::save : Cannot open file " << QFile::encodeName(fileName) << endl;
        return false;
    }

    d->mBuffer->at( 0 );
    file.writeBlock( d->mBuffer->buffer() );

    file.close();
    return true;
}

void KoWmfWrite::setDefaultDpi( int dpi ) { 
    d->mDpi = dpi; 
}


//-----------------------------------------------------------------------------
// Virtual Painter => create the WMF

bool KoWmfWrite::begin() {
    // TODO : reset du buffer

    // reserved placeable and standard header
    for ( int i=0 ; i < 10 ; i++ ) {
        d->mSt << (Q_UINT32)0;
    }

    // initialize a stack with 4 objects
    for ( int i=0 ; i < 4 ; i++ ) {
        d->mSt << (Q_UINT32)8 << (Q_UINT16)0x02FA << (Q_UINT16)0 << (Q_UINT32)0 << (Q_UINT32)0;
    }

    return true;
}


bool KoWmfWrite::end() {
    WmfPlaceableHeader pheader = { 0x9AC6CDD7, 0, 0, 0, 0, 0, 0, 0, 0 };
    Q_UINT16  checksum;

    // End of the wmf file
    d->mSt << (Q_UINT32)3 << (Q_UINT16)0;

    // adjust header
    pheader.left = d->mBBox.left();
    pheader.top = d->mBBox.top();
    pheader.right = d->mBBox.right();
    pheader.bottom = d->mBBox.bottom();
    pheader.inch = d->mDpi;
    checksum = KoWmfReadPrivate::calcCheckSum( &pheader );
    
    // write headers
    d->mBuffer->at( 0 );
    d->mSt << (Q_UINT32)0x9AC6CDD7 << (Q_UINT16)0;
    d->mSt << (Q_INT16)d->mBBox.left() << (Q_INT16)d->mBBox.top() << (Q_INT16)d->mBBox.right() << (Q_INT16)d->mBBox.bottom();
    d->mSt << (Q_UINT16)d->mDpi << (Q_UINT32)0 << checksum;
    d->mSt << (Q_UINT16)1 << (Q_UINT16)9 << (Q_UINT16)0x300 << (Q_UINT32)(d->mBuffer->size()/2);
    d->mSt << (Q_UINT16)4 << (Q_UINT32)30 << (Q_UINT16)0;

    d->mIsValid = true;

    return 0;
}


void KoWmfWrite::save() {
    d->mSt << (Q_UINT32)3 << (Q_UINT16)0x001E;
}


void KoWmfWrite::restore() {
    d->mSt << (Q_UINT32)4 << (Q_UINT16)0x0127 << (Q_UINT16)1;
}


void KoWmfWrite::setPen( const QPen &pen ) {
    int style;
    int max = sizeof(koWmfStylePen) / sizeof(Qt::SolidLine);

    // delete object
    d->mSt << (Q_UINT32)4 << (Q_UINT16)0x01f0 << (Q_UINT16)1;

    for ( style=0 ; style < max ; style++ ) {
        if ( koWmfStylePen[ style ] == pen.style() ) break;
    }
    if ( style == max ) {
        // SolidLine
        style = 0;
    }
    d->mSt << (Q_UINT32)8 << (Q_UINT16)0x02FA;
    d->mSt << (Q_UINT16)style << (Q_UINT16)pen.width() << (Q_UINT16)0 << (Q_UINT32)winColor( pen.color() );

    // select object
    d->mSt << (Q_UINT32)4 << (Q_UINT16)0x012D << (Q_UINT16)1;
}


void KoWmfWrite::setBrush( const QBrush &brush ) {
    int style;
    int max = sizeof(koWmfStyleBrush) / sizeof(Qt::NoBrush);

    // delete object
    d->mSt << (Q_UINT32)4 << (Q_UINT16)0x01f0 << (Q_UINT16)2;

    for ( style=0 ; style < max ; style++ ) {
        if ( koWmfStyleBrush[ style ] == brush.style() ) break;
    }
    if ( style == max ) {
        // SolidPattern
        style = 0;
    }
    d->mSt << (Q_UINT32)7 << (Q_UINT16)0x02FC;
    d->mSt << (Q_UINT16)style << (Q_UINT32)winColor( brush.color() ) << (Q_UINT16)0;

    // select object
    d->mSt << (Q_UINT32)4 << (Q_UINT16)0x012D << (Q_UINT16)2;
}


void KoWmfWrite::setFont( const QFont & ) {
}


void KoWmfWrite::setBackgroundColor( const QColor &c ) {
    d->mSt << (Q_UINT32)5 << (Q_UINT16)0x0201 << (Q_UINT32)winColor( c );
}


void KoWmfWrite::setBackgroundMode( Qt::BGMode mode ) {
    d->mSt << (Q_UINT32)4 << (Q_UINT16)0x0102;
    if ( mode == Qt::TransparentMode )
        d->mSt << (Q_UINT16)1;
    else
        d->mSt << (Q_UINT16)0;
}


void KoWmfWrite::setRasterOp( Qt::RasterOp op ) {
    d->mSt << (Q_UINT32)5 << (Q_UINT16)0x0104 << (Q_UINT32)qtRasterToWin32( op );
}


void KoWmfWrite::setWindow( int left, int top, int width, int height ) {
    d->mBBox.setRect( left, top, width, height );

    // windowOrg
    d->mSt << (Q_UINT32)5 << (Q_UINT16)0x020D << (Q_UINT16)top << (Q_UINT16)left;

    // windowExt
    d->mSt << (Q_UINT32)5 << (Q_UINT16)0x020E << (Q_UINT16)height << (Q_UINT16)width;
}


void KoWmfWrite::setClipRegion( const QRegion & ) {

}


void KoWmfWrite::clipping( bool enable ) {
    if ( !enable ) {
        // clipping region == bounding rectangle
        setClipRegion( d->mBBox );
    }
}


void KoWmfWrite::moveTo( int left, int top ) {
    d->mSt << (Q_UINT32)5 << (Q_UINT16)0x0214 << (Q_UINT16)top << (Q_UINT16)left;
}


void KoWmfWrite::lineTo( int left, int top ) {
    d->mSt << (Q_UINT32)5 << (Q_UINT16)0x0213 << (Q_UINT16)top << (Q_UINT16)left;
}


void KoWmfWrite::drawRect( int left, int top, int width, int height ) {
    QRect rec( left, top, width, height );

    d->mSt << (Q_UINT32)7 << (Q_UINT16)0x041B;
    d->mSt << (Q_UINT16)rec.bottom() << (Q_UINT16)rec.right() << (Q_UINT16)rec.top() << (Q_UINT16)rec.left();
}


void KoWmfWrite::drawRoundRect( int left, int top, int width, int height , int roudw, int roudh ) {
    int  widthCorner, heightCorner;
    QRect rec( left, top, width, height );

    // convert percentage (roundw, roudh) in (widthCorner, heightCorner)
    widthCorner = ( roudw * width ) / 100;
    heightCorner = ( roudh * height ) / 100;

    d->mSt << (Q_UINT32)9 << (Q_UINT16)0x061C << (Q_UINT16)heightCorner << (Q_UINT16)widthCorner;
    d->mSt << (Q_UINT16)rec.bottom() << (Q_UINT16)rec.right() << (Q_UINT16)rec.top() << (Q_UINT16)rec.left();
}


void KoWmfWrite::drawEllipse( int left, int top, int width, int height  ) {
    QRect rec( left, top, width, height );

    d->mSt << (Q_UINT32)7 << (Q_UINT16)0x0418;
    d->mSt << (Q_UINT16)rec.bottom() << (Q_UINT16)rec.right() << (Q_UINT16)rec.top() << (Q_UINT16)rec.left();
}


void KoWmfWrite::drawArc( int left, int top, int width, int height , int a, int alen ) {
    int xCenter, yCenter;
    int  offXStart, offYStart, offXEnd, offYEnd;

    angleToxy( offXStart, offYStart, offXEnd, offYEnd, a, alen );
    xCenter = left + (width / 2);
    yCenter = top + (height / 2);
    
    d->mSt << (Q_UINT32)11 << (Q_UINT16)0x0817;
    d->mSt << (Q_UINT16)(yCenter + offYEnd) << (Q_UINT16)(xCenter + offXEnd);
    d->mSt << (Q_UINT16)(yCenter + offYStart) << (Q_UINT16)(xCenter + offXStart);
    d->mSt << (Q_UINT16)(top + height) << (Q_UINT16)(left + width);
    d->mSt << (Q_UINT16)top << (Q_UINT16)left;
}


void KoWmfWrite::drawPie( int left, int top, int width, int height , int a, int alen ) {
    int xCenter, yCenter;
    int  offXStart, offYStart, offXEnd, offYEnd;

    angleToxy( offXStart, offYStart, offXEnd, offYEnd, a, alen );
    xCenter = left + (width / 2);
    yCenter = top + (height / 2);

    d->mSt << (Q_UINT32)11 << (Q_UINT16)0x081A;
    d->mSt << (Q_UINT16)(yCenter + offYEnd) << (Q_UINT16)(xCenter + offXEnd);
    d->mSt << (Q_UINT16)(yCenter + offYStart) << (Q_UINT16)(xCenter + offXStart);
    d->mSt << (Q_UINT16)(top + height) << (Q_UINT16)(left + width);
    d->mSt << (Q_UINT16)top << (Q_UINT16)left;
}


void KoWmfWrite::drawChord( int left, int top, int width, int height , int a, int alen ) {
    int xCenter, yCenter;
    int  offXStart, offYStart, offXEnd, offYEnd;

    angleToxy( offXStart, offYStart, offXEnd, offYEnd, a, alen );
    xCenter = left + (width / 2);
    yCenter = top + (height / 2);
    
    d->mSt << (Q_UINT32)11 << (Q_UINT16)0x0830;
    d->mSt << (Q_UINT16)(yCenter + offYEnd) << (Q_UINT16)(xCenter + offXEnd);
    d->mSt << (Q_UINT16)(yCenter + offYStart) << (Q_UINT16)(xCenter + offXStart);
    d->mSt << (Q_UINT16)(top + height) << (Q_UINT16)(left + width);
    d->mSt << (Q_UINT16)top << (Q_UINT16)left;
}


void KoWmfWrite::drawPolyline( const QPointArray &pa ) {
    d->mSt << (Q_UINT32)(4 + (pa.size() * 2)) << (Q_UINT16)0x0325 << (Q_UINT16)pa.size();
    pointArray( pa );
}


void KoWmfWrite::drawPolygon( const QPointArray &pa, bool  ) {
    d->mSt << (Q_UINT32)(4 + (pa.size() * 2)) << (Q_UINT16)0x0324 << (Q_UINT16)pa.size();
    pointArray( pa );
}


void KoWmfWrite::drawPolyPolygon( int numberPoly, const QPointArray pa[], bool  ) {
    int sizeArrayPoly = 0;

    for ( int i=0 ; i < numberPoly ; i++ ) {
        sizeArrayPoly += (pa[ i ].size() * 2);
    }
    d->mSt << (Q_UINT32)(4 + numberPoly + sizeArrayPoly) << (Q_UINT16)0x0538 << (Q_UINT16)numberPoly;

    // number of point for each Polygon
    for ( int i=0 ; i < numberPoly ; i++ ) {
        d->mSt << (Q_UINT16)pa[ i ].size();
    }

    // list of points
    for ( int i=0 ; i < numberPoly ; i++ ) {
        pointArray( pa[ i ] );
    }
}


void KoWmfWrite::drawImage( int , int , const QImage &, int , int , int , int  ) {
/*
    QImage img;
    
    img = image;
    img.setFormat( "BMP" );

    QIODevice io = img.ioDevice();
    io.at( 14 );   // skip the BMP header
    d->mSt << io.readAll();
*/
}


void KoWmfWrite::drawText( int , int , int , int , int , const QString& , double ) {
//    d->mSt << (Q_UINT32)3 << (Q_UINT16)0x0A32;
}

//-----------------------------------------------------------------------------
// Utilities and conversion Qt --> Wmf

void KoWmfWrite::pointArray( const QPointArray &pa ) {
    int  left, top, i, max;

    for ( i=0, max=pa.size() ; i < max ; i++ ) {
        pa.point( i, &left, &top );
        d->mSt << (Q_INT16)left << (Q_INT16)top;
    }
}


Q_UINT32 KoWmfWrite::winColor( QColor color ) {
    Q_UINT32 c;

    c = (color.red() & 0xFF);
    c += ( (color.green() & 0xFF) << 8 );
    c += ( (color.blue() & 0xFF) << 16 );

    return c;
}


void KoWmfWrite::angleToxy( int &xStart, int &yStart, int &xEnd, int &yEnd, int a, int alen ) {
    double angleStart, angleLength;
    
    angleStart = ((double)a * 3.14166) / 2880;
    angleLength = ((double)alen * 3.14166) / 2880;
    
    xStart = (int)(cos(angleStart) * 50);
    yStart = -(int)(sin(angleStart) * 50);
    xEnd = (int)(cos(angleLength) * 50);
    yEnd = -(int)(sin(angleLength) * 50);
}


Q_UINT16  KoWmfWrite::qtRasterToWin16( Qt::RasterOp op ) const {
    int i;

    for ( i=0 ; i < 17 ; i++ ) {
        if ( koWmfOpTab16[ i ] == op )  break;
    }

    if ( i < 17 )
        return  (Q_UINT16)i;
    else
        return (Q_UINT16)0;
}


Q_UINT32  KoWmfWrite::qtRasterToWin32( Qt::RasterOp op ) const {
    int i;

    for ( i=0 ; i < 15 ; i++ ) {
        if ( koWmfOpTab32[ i ].qtRasterOp == op )  break;
    }

    if ( i < 15 )
        return koWmfOpTab32[ i ].winRasterOp;
    else
        return koWmfOpTab32[ 0 ].winRasterOp;
}

