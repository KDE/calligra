/* This file is part of the Calligra project, made within the KDE community.

   Copyright 2012 Friedrich W. H. Kossebau <kossebau@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "cdrparser.h"

// Koralle
#include <Koralle0/FourCharCode>
// Karbon
#include <KarbonDocument.h>
// Calligra
#include <KoShapeUngroupCommand.h>
#include <KoShapeGroup.h>
#include <KoShapeLayer.h>
#include <KoPathShape.h>
#include <KoLineBorder.h>
// Qt
#include <QtCore/QFile>
#include <QtCore/QByteArray>

#include <QDebug>

static const Koralle::FourCharCode bmp_Id('b','m','p',' ');
static const Koralle::FourCharCode bmptId('b','m','p','t');
static const Koralle::FourCharCode dispId('D','I','S','P');
static const Koralle::FourCharCode doc_Id('d','o','c',' ');
static const Koralle::FourCharCode fillId('f','i','l','l');
static const Koralle::FourCharCode filtId('f','i','l','t');
static const Koralle::FourCharCode flgsId('f','l','g','s');
static const Koralle::FourCharCode fnttId('f','n','t','t');
static const Koralle::FourCharCode fontId('f','o','n','t');
static const Koralle::FourCharCode gobjId('g','o','b','j');
static const Koralle::FourCharCode grp_Id('g','r','p',' ');
static const Koralle::FourCharCode infoId('I','N','F','O');
static const Koralle::FourCharCode layrId('l','a','y','r');
static const Koralle::FourCharCode lgobId('l','g','o','b');
static const Koralle::FourCharCode lodaId('l','o','d','a');
static const Koralle::FourCharCode mcfgId('m','c','f','g');
static const Koralle::FourCharCode obj_Id('o','b','j',' ');
static const Koralle::FourCharCode otltId('o','t','l','t');
static const Koralle::FourCharCode outlId('o','u','t','l');
static const Koralle::FourCharCode pageId('p','a','g','e');
static const Koralle::FourCharCode stltId('s','t','l','t');
static const Koralle::FourCharCode stshId('s','t','s','h');
static const Koralle::FourCharCode stylId('s','t','y','l');
static const Koralle::FourCharCode stydId('s','t','y','d');
static const Koralle::FourCharCode trfdId('t','r','f','d');
static const Koralle::FourCharCode trflId('t','r','f','l');
static const Koralle::FourCharCode vrsnId('v','r','s','n');


/// Returns the string starting at @arg startOffset, until the first '\0' or the
/// end of the @arg data.
static QString
stringData( const QByteArray& data, int startOffset = 0 )
{
    QString result;

    const char* string = &data.constData()[startOffset];

    // find terminating \0
    int stringLength = data.size() - startOffset;
    if( stringLength > 0 )
    {
        for( int i = 0; i < stringLength; ++i )
        {
            if( string[i] == 0 )
            {
                stringLength = i;
                break;
            }
        }
        result = QString::fromLatin1( string, stringLength );
    }

    return result;
}

template<typename T>
T
data( const QByteArray& data, int offset = 0 )
{
    return *reinterpret_cast<const T*>(&data.constData()[offset]);
}
template<typename T>
const T*
dataPtr( const QByteArray& data, int offset = 0 )
{
    return reinterpret_cast<const T*>(&data.constData()[offset]);
}



CdrParser::CdrParser()
{

}


CdrParser::~CdrParser()
{
}


qreal
CdrParser::koXCoord( cdr4Coord cdrCoord ) const
{
    if( ! mDocument ) return 0.0;

    return xCDR_TO_POINT(static_cast<qreal>(cdrCoord));
}

qreal
CdrParser::koYCoord( cdr4Coord cdrCoord ) const
{
    if( ! mDocument ) return 0.0;

    return yCDR_TO_POINT(static_cast<qreal>(cdrCoord));
}


QPointF
CdrParser::koCoords( Cdr4Point cdrCoords) const
{
    return QPointF( koXCoord(cdrCoords.mX), koYCoord(cdrCoords.mY) );
}


bool
CdrParser::parse( KarbonDocument* document, QFile& file )
{
    mDocument = document;

    KoShapeLayer* oldLayer =
        (! mDocument->layers().isEmpty()) ? mDocument->layers().first() : 0;

    mRiffStreamReader.setDevice( &file );

    if( mRiffStreamReader.readNextChunkHeader() )
    {
        // check format
        const Koralle::FourCharCode formatId = mRiffStreamReader.chunkId();

        // check for "CDR", TODO: check also supported version on last byte
        const bool isCDR =
            ((formatId.mCode.asChars[0] == 'C') &&
             (formatId.mCode.asChars[1] == 'D') &&
             (formatId.mCode.asChars[2] == 'R'));

        mCdrVersion = formatId.mCode.asChars[3] - '0';

        if( mRiffStreamReader.isFileChunk() &&
            isCDR &&
            (4 <= mCdrVersion) && (mCdrVersion <= 5) )
        {
            readCDR();
        }
        else
        {
//             mRiffStreamReader.raiseError("The file is not an CDR file in a supported version."));
            return false;
        }
    }

    if (oldLayer) {
        mDocument->removeLayer(oldLayer);
        delete oldLayer;
    }

    return (! mRiffStreamReader.hasError() );
}


void
CdrParser::readCDR()
{
    mRiffStreamReader.openList();

    while( mRiffStreamReader.readNextChunkHeader() )
    {
        const Koralle::FourCharCode chunkId = mRiffStreamReader.chunkId();

        // TODO: where to check fixed sizes for correctness?
        if( (chunkId == vrsnId) &&
            (mRiffStreamReader.chunkSize() == 2) )
            readVersion();
        else if( (chunkId == dispId) )
            readDisp();
        else if( (chunkId == infoId) &&
                 mRiffStreamReader.isListChunk() )
            readInfo();
        else if( (chunkId == doc_Id) &&
                 mRiffStreamReader.isListChunk() )
            readDoc();
        else if( (chunkId == pageId) &&
                 mRiffStreamReader.isListChunk() )
            readPage();
    }

    mRiffStreamReader.closeList();
}

void
CdrParser::readVersion()
{
    const QByteArray versionData = mRiffStreamReader.chunkData();
    const quint16 fullVersion = data<quint16>( versionData );

qDebug() << "Version:" << static_cast<qreal>(fullVersion)/100;
}

void
CdrParser::readDoc()
{
    mRiffStreamReader.openList();

    while( mRiffStreamReader.readNextChunkHeader() )
    {
        const Koralle::FourCharCode chunkId = mRiffStreamReader.chunkId();

        if( (chunkId == stshId) )
            readDocStsh();
        else if( chunkId == mcfgId )
            readDocMCfg();
        else if( (chunkId == bmptId) &&
                 mRiffStreamReader.isListChunk() )
            readDocBitmapTable();
        else if( (chunkId == fnttId) &&
                 mRiffStreamReader.isListChunk() )
            readDocFontTable();
        else if( (chunkId == filtId) &&
                 mRiffStreamReader.isListChunk() )
            readDocFillTable();
        else if( (chunkId == otltId) &&
                 mRiffStreamReader.isListChunk() )
            readDocOutlineTable();
        else if( (chunkId == stltId) &&
                 mRiffStreamReader.isListChunk() )
            readDocStyleTable();
    }

    mRiffStreamReader.closeList();
}

void
CdrParser::readDisp()
{
}

void
CdrParser::readInfo()
{
}

void
CdrParser::readDocMCfg()
{
    const QByteArray mcfgData = mRiffStreamReader.chunkData();
    const MCfgData* mcfg = dataPtr<MCfgData>( mcfgData );

    // set the page size
    const QSizeF pageSize( xCDR_TO_POINT(mcfg->width), yCDR_TO_POINT(mcfg->height) );
    mDocument->setPageSize(pageSize);
}

void
CdrParser::readDocStsh()
{
    // 0..end: a \0 terminated string with some file name, e.g. "CORELDRW.CDT"
    const QByteArray mcfgData = mRiffStreamReader.chunkData();
    const QString fileName = stringData( mcfgData );
qDebug() << "Stsh:" << fileName;
}

void
CdrParser::readDocFontTable()
{
    mRiffStreamReader.openList();
qDebug() << "Reading fonts...";
    while( mRiffStreamReader.readNextChunkHeader() )
    {
        if( mRiffStreamReader.chunkId() == fontId )
        {
            const QByteArray fontData = mRiffStreamReader.chunkData();

            // bytes 0..1: some index/id/key which seems sorted
            // TODO: endianness
            const quint16 fontIndex = data<quint16>( fontData );

            // bytes 2..17 for version not yet known, font info?
            // name
            const int fontNameOffset =
                (mCdrVersion == 4) ? 2 :
                /*mCdrVersion == 5*/ 18;
            const int fontDataSize = fontData.size();
            if( fontDataSize > fontNameOffset )
            {
                const QString fontName =
                    stringData(fontData, fontNameOffset);
qDebug() << fontIndex << fontName;
            }
        }
    }

    mRiffStreamReader.closeList();
}

void
CdrParser::readDocBitmapTable()
{
    mRiffStreamReader.openList();
qDebug() << "Reading Bitmaps...";
    while( mRiffStreamReader.readNextChunkHeader() )
    {
        if( mRiffStreamReader.chunkId() == bmp_Id )
        {
            const QByteArray bmpData = mRiffStreamReader.chunkData();
/* Found with CDR4:
0000:0000 | 02 00 42 4D  36 04 0C 00  00 00 00 00  36 04 00 00 | ..BM6.......6...
0000:0010 | 28 00 00 00  00 04 00 00  00 03 00 00  01 00 08 00 | (...............
0000:0020 | 00 00 00 00  00 00 0C 00  00 00 00 00  00 00 00 00 | ................
0000:0030 | 00 01 00 00  00 00 00 00                           | ........
*/
            // 0..1: int16 index/key/id?
            const quint16 bmpIndex = data<quint16>( bmpData );
            // 2..3: "BM" as bitmap type indicator?
            // 4..7: full size of bitmap data (?)
            // 12: offset in bitmap data of palette
            // 20..21: int16 witdh (?)
            // 20..23: int32 witdh (?)
            const quint16 width = data<quint16>( bmpData, 20 );
            // 24..25: int16 height(?)
            // 24..27: int32 height(?)
            const quint16 height = data<quint16>( bmpData, 24 );
            // 48..50: int16 size of palette (?)
            // 48..52: int32 size of palette (?)
            // 56..1079: palette with 256 RGBA(?)
            // 1080..end: pixmap with byte into palette
qDebug() << bmpIndex << "width" << width << "height" << height;
        }
    }

    mRiffStreamReader.closeList();
}

enum FillType { Transparent = 0, Solid = 0, Gradient = 0 };

void
CdrParser::readDocFillTable()
{
    mRiffStreamReader.openList();
qDebug() << "Reading Fills...";
    while( mRiffStreamReader.readNextChunkHeader() )
    {
        if( mRiffStreamReader.chunkId() == fillId )
        {
            const QByteArray fillData = mRiffStreamReader.chunkData();
            // 0..1: int16 index/key/id?
            const quint16 fillIndex = data<quint16>( fillData );
            // 4: filltype (?)
            // found with 8 (filltype transparent) and 30 bytes (filltype solid)
qDebug() << fillIndex;
        }
    }

    mRiffStreamReader.closeList();
}

void
CdrParser::readDocOutlineTable()
{
    mRiffStreamReader.openList();
qDebug() << "Reading Outlines...";
    while( mRiffStreamReader.readNextChunkHeader() )
    {
        if( mRiffStreamReader.chunkId() == outlId )
        {
            const QByteArray outlineData = mRiffStreamReader.chunkData();
            // 0..1: int16 index/key/id?
            const quint16 outlineIndex = data<quint16>( outlineData );
            // 4: filltype (?)
            // found with 8 (filltype transparent) and 30 bytes (filltype solid)
qDebug() << outlineIndex;
        }
    }

    mRiffStreamReader.closeList();
}

void
CdrParser::readDocStyleTable()
{
    mRiffStreamReader.openList();
qDebug() << "Reading Styles...";
    while( mRiffStreamReader.readNextChunkHeader() )
    {
        if( (mRiffStreamReader.chunkId() == stylId) &&
            mRiffStreamReader.isListChunk() )
        {
            readDocStyle();
        }
    }

    mRiffStreamReader.closeList();
}

void
CdrParser::readDocStyle()
{
    mRiffStreamReader.openList();

    while( mRiffStreamReader.readNextChunkHeader() )
    {
        if( mRiffStreamReader.chunkId() == stydId )
        {
            const QByteArray styleData = mRiffStreamReader.chunkData();
            // 0..1: int16 index/key/id?
            const quint16 styleIndex = data<quint16>( styleData );
            // 2..3: size of style data
            // 4..5: style type (?)
            const quint16 styleType = data<quint16>( styleData, 4 );
            // 40..41: size of style data again(?)

            // -: no(?) start of titel with type 2
            // 42: start of title with type 3
            // 74: start of title with type 6
            // 380: start of titel with type 10
            const int styleNameOffset =
                ( styleType ==  3 ) ?  42 :
                ( styleType ==  6 ) ? (mCdrVersion==4 ?  74 :  78) :
                ( styleType == 10 ) ? (mCdrVersion==4 ? 380 : 392) :
                                       -1;
            const QString styleName =
                stringData(styleData, styleNameOffset);
qDebug() << styleIndex << styleName;
        }
    }

    mRiffStreamReader.closeList();
}

void
CdrParser::readPage()
{
    mRiffStreamReader.openList();

qDebug() << "Reading Page...";
    while( mRiffStreamReader.readNextChunkHeader() )
    {
        const Koralle::FourCharCode chunkId = mRiffStreamReader.chunkId();

        if( (chunkId == flgsId) &&
            (mRiffStreamReader.chunkSize() == 2) )
        {
            readPageFlags();
        }
        else if( (chunkId == gobjId) &&
                 mRiffStreamReader.isListChunk() )
        {
            mRiffStreamReader.openList();

            while( mRiffStreamReader.readNextChunkHeader() )
            {
                if( (mRiffStreamReader.chunkId() == layrId) &&
                    mRiffStreamReader.isListChunk() )
                {
                    readLayer();
                }
            }

            mRiffStreamReader.closeList();
        }
    }

    mRiffStreamReader.closeList();
}

void
CdrParser::readPageFlags()
{
// 000C:3990 |       40 00  01 90                                 |   @...          
// 000C:3B00 |                                 40 00  00 90       |           @...  
// 000C:8FE0 |                           00 00 00 90              |         ....    

}


void
CdrParser::readLayer()
{
    mRiffStreamReader.openList();

    KoShapeLayer* layer = new KoShapeLayer();

qDebug() << "Layer <<<";
    while( mRiffStreamReader.readNextChunkHeader() )
    {
        const Koralle::FourCharCode chunkId = mRiffStreamReader.chunkId();

        if( (chunkId == flgsId) &&
            (mRiffStreamReader.chunkSize() == 2) )
            readLayerFlags();
        else if( (chunkId == lgobId) &&
                 mRiffStreamReader.isListChunk() )
        {
            readLayerLGOb();
        }
        else if( (chunkId == grp_Id) &&
                 mRiffStreamReader.isListChunk() )
        {
            KoShapeGroup* group = readObjectGroup();
            if( group )
                layer->addShape( group );
        }
        else if( (chunkId == obj_Id) &&
                 mRiffStreamReader.isListChunk() )
        {
            KoShape* object = readObject();
            if( object )
                layer->addShape( object );
        }
    }

qDebug() << "Layer >>>";
    mDocument->insertLayer(layer);
    mRiffStreamReader.closeList();
}

void
CdrParser::readLayerFlags()
{
// 000C:39B0 |                    5A 01  01 98                    |       Z...      
// 000C:3A00 |                                 4A 00  01 98       |           J...  
// 000C:3A60 |       48 00  01 98                                 |   H...          
// 000C:3AB0 |                    40 00  00 98                    |       @...      
// 000C:3B00 |                                 40 00  00 90       |           @...  
// 000C:3B20 |                                              5A 01 |               Z.
// 000C:3B30 | 01 98                                              | ..              
// 000C:3B70 |                    4A 00  01 98                    |       J...      
// 000C:3BB0 |                                              48 00 |               H.
// 000C:3BC0 | 01 98                                              | ..              
// 000C:3C00 |                    40 00  00 98                    |       @...      
// 000C:3C40 |                                              00 02 |               ..
// 000C:3C50 | 00 08                                              | ..              

}
// 000C:3D40 |       00 02  00 08                                 |   ....          

void
CdrParser::readLayerLGOb()
{
    mRiffStreamReader.openList();
qDebug() << "LGOb <<<";

    while( mRiffStreamReader.readNextChunkHeader() )
    {
        const Koralle::FourCharCode chunkId = mRiffStreamReader.chunkId();

        if( (chunkId == lodaId) )
        {
            readLoda();
        }
    }
qDebug() << "LGOb >>>";
    mRiffStreamReader.closeList();
}

KoShapeGroup*
CdrParser::readObjectGroup()
{
    KoShapeGroup* group = new KoShapeGroup();
//     group->setZIndex(m_context.nextZIndex());
// TODO: register all created in a lookup table, or not needed?
//     shape->setName(id);
//     m_context.registerShape(id, shape);

    mRiffStreamReader.openList();
qDebug() << "Group <<<";

    while( mRiffStreamReader.readNextChunkHeader() )
    {
        const Koralle::FourCharCode chunkId = mRiffStreamReader.chunkId();

        if( mRiffStreamReader.chunkId() == flgsId )
        {
            readObjectGroupFlags();
        }
        else if( (chunkId == obj_Id) &&
                 mRiffStreamReader.isListChunk() )
        {
            KoShape* object = readObject();
            if( object )
                group->addShape( object );
        }
    }

    mDocument->add(group);
qDebug() << "Group >>>...";
    mRiffStreamReader.closeList();
    return group;
}

void
CdrParser::readObjectGroupFlags()
{
}


KoShape*
CdrParser::readObject()
{
    KoShape* object = 0;

    mRiffStreamReader.openList();
qDebug() << "Object <<<";

    while( mRiffStreamReader.readNextChunkHeader() )
    {
        const Koralle::FourCharCode chunkId = mRiffStreamReader.chunkId();

        if( (mRiffStreamReader.chunkId() == flgsId) )
        {
            readObjectFlags();
        }
        else if( (chunkId == lgobId) &&
                 mRiffStreamReader.isListChunk() )
        {
            object = readObjectLGOb();
        }
    }

qDebug() << "Object >>>...";
    mRiffStreamReader.closeList();

    return object;
}

void
CdrParser::readObjectFlags()
{
}

KoShape*
CdrParser::readObjectLGOb()
{
    KoShape* object = 0;
    mRiffStreamReader.openList();
qDebug() << "LGOb <<<";

    while( mRiffStreamReader.readNextChunkHeader() )
    {
        const Koralle::FourCharCode chunkId = mRiffStreamReader.chunkId();

        if( (chunkId == lodaId) )
        {
            object = readLoda();
        }
        else if( (chunkId == trflId) &&
                 mRiffStreamReader.isListChunk() )
        {
            readTrfl();
        }
    }
qDebug() << "LGOb >>>";
    mRiffStreamReader.closeList();
    return object;
}

void
CdrParser::readTrfl()
{
    mRiffStreamReader.openList();

    while( mRiffStreamReader.readNextChunkHeader() )
    {
        if( (mRiffStreamReader.chunkId() == trfdId) )
        {
            const QByteArray trfdData = mRiffStreamReader.chunkData();
            // 6..7: start of arguments types, here always set to FF FF, kind of null pointer?
            // 8..9: type of chunk, here always 00 00
            const CdrArgumentData* argsData = dataPtr<CdrArgumentData>( trfdData );
qDebug() << "Reading Trfd" << argsData->count << "args";
    for (int i=0; i < argsData->count; i++)
    {
// qDebug() << i << ": type" << argsData->argPtr<>()<TransformData>(i);
    }
        }
    }

    mRiffStreamReader.closeList();
}


KoShape*
CdrParser::readLoda()
{
    KoShape* object = 0;

    const QByteArray lodaData = mRiffStreamReader.chunkData();

    const CdrArgumentData* argsData = dataPtr<CdrArgumentData>( lodaData );

qDebug() << "Reading Loda" << argsData->count << "args, loda type" << argsData->chunkType;
    if( argsData->chunkType == CdrPathObjectId )
        object = readPathObject( argsData );
    else
    for (int i=0; i < argsData->count; i++)
    {
qDebug() << i << ": type" << argsData->argType(i);

// each page has as start types 0B, 0C, 11, 0
// first set has both arg type 1000 and 2000, with text set for 1000, other have just 2000, no text
// types 3 and 4 are on second, type 5 on last
// type 3 and 5 have args 10, 20, 30, 100, 200, 1010 (1010 sometimes missing)

if( argsData->argType(i) == 1000 )
qDebug() << stringData( lodaData, argsData->argOffsets()[i] );
// Arg types:
// 10  32bit (outline?)
// 20  32bit (fill?)
// 30  coordinates data?
// 100 32bit
// 200 16 bit  (index to bitmap?)
// 1000: text/title
// 1010 16 bit
// 2000: data 01 00 64 00 64 00 00 00 00 00 00 00

// type 5: bitmap?
// 1010:  320    (40 01)
//  200:    5    (05 00)
//  100: 2756    (C4 0A 00 00)
//   30:         (...) 44 bytes
//   20:    2    (02 00 00 00)
//   10:    2    (02 00 00 00)

// type 3: line or curve?
//  200:    5    (05 00)
//  100: 1972    (B4 07 00 00)
//   30:         (...) 64 bytes
//   20:    1    (01 00 00 00)
//   10:    2    (02 00 00 00)

// type 3: line or curve?
// 1010: 32768   (01 80)
//  200:    5    (05 00)
//  100: 1940    (94 07 00 00)
//   30:         (...) 872 bytes
//   20:  256    (00 01 00 00)
//   10: 1024    (00 04 00 00)

    }
    if( object )
        mDocument->add( object );
    return object;
}

KoPathShape*
CdrParser::readPathObject( const CdrArgumentData* argsData )
{
    KoPathShape* path = new KoPathShape();
    for (int i=0; i < argsData->count; i++)
    {
        if( argsData->argType(i) == 30 )
        {
            const Cdr4PointList* points = argsData->argPtr<Cdr4PointList>( i );
qDebug() << "line coords:" << points->count;
            for (unsigned int j=0; j<points->count; j++)
            {
                const QPointF point = koCoords( points->point(j) );
                const PointType pointType = points->pointType(j);
qDebug() << point.x()<<","<<point.y()<<":"<< QString::number(pointType,16);
                if(j==0) // is first point
                    path->moveTo( point );
                else
                {
                    const bool isLineStarting = (pointType == 0x0C);

                    if( isLineStarting )
                        path->moveTo( point );
                    else
                    {
                        path->lineTo( point );

                        const bool isLineEnding = (pointType == 0x48);
                        if( isLineEnding )
                            path->close();
                    }
                }
            }
            path->normalize();
            path->setBorder( new KoLineBorder(1.0) );
        }
    }
    return path;
}
