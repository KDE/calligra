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


static QString
stringAtDataEnd( const QByteArray& data, int stringStartOffset )
{
    const char* stringData = &data.constData()[stringStartOffset];

    // find terminating \0
    int stringLength = data.size() - stringStartOffset;
    for( int i = 0; i < stringLength; ++i )
    {
        if( stringData[i] == 0 )
        {
            stringLength = i;
            break;
        }
    }

    return QString::fromLatin1( stringData, stringLength );
}


CdrParser::CdrParser()
{

}


CdrParser::~CdrParser()
{
}

bool
CdrParser::parse( KarbonDocument* document, QFile& file )
{
    mDocument = document;
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
    quint16 fullVersion = 0;
    const QByteArray versionData = mRiffStreamReader.chunkData();
    if( versionData.count() == 2 )
        fullVersion = *reinterpret_cast<const quint16*>(versionData.constData());
}

void
CdrParser::readDoc()
{
    mRiffStreamReader.openList();

    while( mRiffStreamReader.readNextChunkHeader() )
    {
        const Koralle::FourCharCode chunkId = mRiffStreamReader.chunkId();

        if( (chunkId == stshId) )
            readDisp();
        else if( chunkId == mcfgId )
            readDocMCfg();
        else if( chunkId == stshId )
            readDocStsh();
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

}

void
CdrParser::readDocStsh()
{

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
            const quint16 fontIndex = *reinterpret_cast<const quint16*>(fontData.constData());

            // bytes 2..17 for version not yet known, font info?
            // name
            const int fontNameOffset =
                (mCdrVersion == 4) ? 2 :
                /*mCdrVersion == 5*/ 18;
            const int fontDataSize = fontData.size();
            if( fontDataSize > fontNameOffset )
            {
                const QString fontName =
                    stringAtDataEnd(fontData, fontNameOffset);
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
            const quint16 bmpIndex = *reinterpret_cast<const quint16*>(bmpData.constData());
            // 2..3: "BM" as bitmap type indicator?
            // 4..7: full size of bitmap data (?)
            // 12: offset in bitmap data of palette
            // 20..21: int16 witdh (?)
            // 20..23: int32 witdh (?)
            const quint16 width = *reinterpret_cast<const quint16*>(&bmpData.constData()[20]);
            // 24..25: int16 height(?)
            // 24..27: int32 height(?)
            const quint16 height = *reinterpret_cast<const quint16*>(&bmpData.constData()[24]);
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
            const quint16 fillIndex = *reinterpret_cast<const quint16*>(fillData.constData());
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
            const quint16 outlineIndex = *reinterpret_cast<const quint16*>(outlineData.constData());
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
            const quint16 styleIndex = *reinterpret_cast<const quint16*>(styleData.constData());
            // 2..3: size of style data
            // 4..5: style type (?)
            const quint16 styleType = *reinterpret_cast<const quint16*>(&styleData.constData()[4]);
            // 40..41: size of style data again(?)

            // -: no(?) start of titel with type 2
            // 42: start of title with type 3
            // 74: start of titel with type 6
            // 380: start of titel with type 10
            const int styleNameOffset =
                ( styleType ==  3 ) ?  42 :
                ( styleType ==  6 ) ? (mCdrVersion==4 ?  74 :  78) :
                ( styleType == 10 ) ? (mCdrVersion==4 ? 380 : 392) :
                                       -1;
            const QString styleName =
                stringAtDataEnd(styleData, styleNameOffset);
qDebug() << styleIndex << styleName;
        }
    }

    mRiffStreamReader.closeList();
}

void
CdrParser::readPage()
{
    mRiffStreamReader.openList();

    while( mRiffStreamReader.readNextChunkHeader() )
    {
        const Koralle::FourCharCode chunkId = mRiffStreamReader.chunkId();

        if( (chunkId == flgsId) &&
            (mRiffStreamReader.chunkSize() == 2) )
            readPageFlags();
        else if( (chunkId == gobjId) &&
                 mRiffStreamReader.isListChunk() )
            readPageGObj();
    }

    mRiffStreamReader.closeList();
}

void
CdrParser::readPageFlags()
{

}

void
CdrParser::readPageGObj()
{
    mRiffStreamReader.openList();
qDebug() << "Reading GObj...";
    while( mRiffStreamReader.readNextChunkHeader() )
    {
        if( (mRiffStreamReader.chunkId() == layrId) &&
            mRiffStreamReader.isListChunk() )
        {
            readPageGObjLayer();
        }
    }

    mRiffStreamReader.closeList();
}

void
CdrParser::readPageGObjLayer()
{
    mRiffStreamReader.openList();
qDebug() << "Reading GObj...";
    while( mRiffStreamReader.readNextChunkHeader() )
    {
        const Koralle::FourCharCode chunkId = mRiffStreamReader.chunkId();

        if( (chunkId == flgsId) &&
            (mRiffStreamReader.chunkSize() == 2) )
            readPageGObjLayerFlags();
        else if( (chunkId == lgobId) &&
                 mRiffStreamReader.isListChunk() )
        {
            readPageGObjLayerLGOb();
        }
        else if( (chunkId == grp_Id) &&
                 mRiffStreamReader.isListChunk() )
        {
            readPageGObjLayerGrp();
        }
        else if( (chunkId == obj_Id) &&
                 mRiffStreamReader.isListChunk() )
        {
            readPageGObjLayerObj();
        }
    }

    mRiffStreamReader.closeList();
}

void
CdrParser::readPageGObjLayerFlags()
{

}

void
CdrParser::readPageGObjLayerLGOb()
{
    mRiffStreamReader.openList();

    while( mRiffStreamReader.readNextChunkHeader() )
    {
        const Koralle::FourCharCode chunkId = mRiffStreamReader.chunkId();

        if( (chunkId == lodaId) )
        {
            
        }
        else if( (chunkId == trflId) &&
                 mRiffStreamReader.isListChunk() )
        {
            readPageGObjLayerLGObTrfl();
        }
    }

    mRiffStreamReader.closeList();
}

void
CdrParser::readPageGObjLayerLGObTrfl()
{
    mRiffStreamReader.openList();

    while( mRiffStreamReader.readNextChunkHeader() )
    {
        if( (mRiffStreamReader.chunkId() == trfdId) )
        {
            
        }
    }

    mRiffStreamReader.closeList();
}

void
CdrParser::readPageGObjLayerGrp()
{

}

void
CdrParser::readPageGObjLayerObj()
{

}
