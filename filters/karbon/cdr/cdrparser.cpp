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

// filter
#include "cdrdocument.h"
// Koralle
#include <Koralle0/FourCharCode>
// Qt
#include <QtCore/QFile>
#include <QtCore/QByteArray>
#include <QtCore/QSizeF>

#include <QDebug>

static const Koralle::FourCharCode bboxId('b','b','o','x');
static const Koralle::FourCharCode bmp_Id('b','m','p',' ');
static const Koralle::FourCharCode bmptId('b','m','p','t');
static const Koralle::FourCharCode bnchId('b','n','c','h');
static const Koralle::FourCharCode bschId('b','s','c','h');
static const Koralle::FourCharCode btidId('b','t','i','d');
static const Koralle::FourCharCode btxtId('b','t','x','t');
static const Koralle::FourCharCode dispId('D','I','S','P');
static const Koralle::FourCharCode doc_Id('d','o','c',' ');
static const Koralle::FourCharCode fillId('f','i','l','l');
static const Koralle::FourCharCode filtId('f','i','l','t');
static const Koralle::FourCharCode flgsId('f','l','g','s');
static const Koralle::FourCharCode fnttId('f','n','t','t');
static const Koralle::FourCharCode fontId('f','o','n','t');
static const Koralle::FourCharCode gobjId('g','o','b','j');
static const Koralle::FourCharCode grp_Id('g','r','p',' ');
static const Koralle::FourCharCode guidId('g','u','i','d');
static const Koralle::FourCharCode infoId('I','N','F','O');
static const Koralle::FourCharCode layrId('l','a','y','r');
static const Koralle::FourCharCode lgobId('l','g','o','b');
static const Koralle::FourCharCode lnkgId('l','n','k','g');
static const Koralle::FourCharCode lnktId('l','n','k','t');
static const Koralle::FourCharCode lodaId('l','o','d','a');
static const Koralle::FourCharCode mcfgId('m','c','f','g');
static const Koralle::FourCharCode obj_Id('o','b','j',' ');
static const Koralle::FourCharCode otltId('o','t','l','t');
static const Koralle::FourCharCode outlId('o','u','t','l');
static const Koralle::FourCharCode pageId('p','a','g','e');
static const Koralle::FourCharCode paraId('p','a','r','a');
static const Koralle::FourCharCode parlId('p','a','r','l');
static const Koralle::FourCharCode spndId('s','p','d','d');
static const Koralle::FourCharCode stltId('s','t','l','t');
static const Koralle::FourCharCode strlId('s','t','r','l');
static const Koralle::FourCharCode stshId('s','t','s','h');
static const Koralle::FourCharCode stylId('s','t','y','l');
static const Koralle::FourCharCode stydId('s','t','y','d');
static const Koralle::FourCharCode trfdId('t','r','f','d');
static const Koralle::FourCharCode trflId('t','r','f','l');
static const Koralle::FourCharCode vectId('v','e','c','t');
static const Koralle::FourCharCode vrsnId('v','r','s','n');


/// Returns the string starting at @arg startOffset, until the first '\0' or the
/// end of the @arg data.
static QByteArray
baStringData( const QByteArray& data, int startOffset = 0 )
{
    QByteArray result;

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
        result = QByteArray( string, stringLength );
    }

    return result;
}

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

// TODO: make this a static method, and have parsing done by CdrParser constructor,
// keeping results as members.
CdrDocument*
CdrParser::parse( QFile& file )
{
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
            mDocument = new CdrDocument;
            readCDR();
        }
        else
        {
//             mRiffStreamReader.raiseError("The file is not an CDR file in a supported version."));
            return 0;
        }
    }

    if( mRiffStreamReader.hasError() )
    {
        delete mDocument;
        mDocument = 0;
    }

    return mDocument;
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
        {
            CdrPage* page = readPage();
            if( page )
                mDocument->addPage( page );
        }
    }

    mRiffStreamReader.closeList();
}

void
CdrParser::readVersion()
{
    const QByteArray versionData = mRiffStreamReader.chunkData();
    mDocument->setFullVersion( data<quint16>(versionData) );

qDebug() << "Version:" << static_cast<qreal>(mDocument->fullVersion())/100;
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
        else if( chunkId == guidId )
            readDocGuid();
        else if( (chunkId == fnttId) &&
                 mRiffStreamReader.isListChunk() )
            readDocFontTable();
        else if( (chunkId == bmptId) &&
                 mRiffStreamReader.isListChunk() )
            readDocBitmapTable();
        else if( chunkId == lnktId )
            readDocLnkTable();
        else if( (chunkId == vectId) &&
                 mRiffStreamReader.isListChunk() )
            readDocVecTable();
        else if( (chunkId == filtId) &&
                 mRiffStreamReader.isListChunk() )
            readDocFillTable();
        else if( (chunkId == otltId) &&
                 mRiffStreamReader.isListChunk() )
            readDocOutlineTable();
        else if( (chunkId == stltId) &&
                 mRiffStreamReader.isListChunk() )
            readDocStyleTable();
        else if( (chunkId == btxtId) &&
                 mRiffStreamReader.isListChunk() )
            readDocBtxTable();
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
    mDocument->setSize( mcfg->width, mcfg->height );
}

void
CdrParser::readDocGuid()
{
    const QByteArray guidData = mRiffStreamReader.chunkData();
}

void
CdrParser::readDocStsh()
{
    // 0..end: a \0 terminated string with some file name, e.g. "CORELDRW.CDT"
    const QByteArray mcfgData = mRiffStreamReader.chunkData();
    mDocument->setStyleSheetFileName( baStringData(mcfgData) );
qDebug() << "Stsh:" << mDocument->styleSheetFileName();
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
CdrParser::readDocLnkTable()
{
    const QByteArray lnkTableData = mRiffStreamReader.chunkData();
    const CdrArgumentData* argsData = dataPtr<CdrArgumentData>( lnkTableData );
qDebug() << "Reading LnkTable" << argsData->count << "args";
    for (int i=0; i < argsData->count; i++)
    {
// qDebug() << i << ": type" << argsData->argPtr<LnkData>(i);
    }
}

void
CdrParser::readDocVecTable()
{
qDebug() << "Reading Vec Table";
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

void
CdrParser::readDocFillTable()
{
    mRiffStreamReader.openList();
qDebug() << "Reading Fills...";
    while( mRiffStreamReader.readNextChunkHeader() )
    {
        if( mRiffStreamReader.chunkId() == fillId )
        {
            CdrAbstractFill* fill = 0;

            const QByteArray fillData = mRiffStreamReader.chunkData();
            // 0..3: uint32 index/key/id? would match loda
            const quint32 fillIndex = data<quint32>( fillData );
            // 4: filltype (?)
            const quint32 fillType = data<quint32>( fillData, 4 );
            if( fillType == CdrTransparent )
            {
                fill = new CdrTransparentFill;
                // transparent has no other data stored
            }
            else if( fillType == CdrSolid )
            {
                fill = new CdrSolidFill;
                // 8: filltype (?)
                const CdrSolidFillData* solidFillData = dataPtr<CdrSolidFillData>( fillData, 8 );
            }
            const QString fillTypeName =
                QLatin1String(fillType == CdrTransparent ? "Transparent" :
                              fillType == CdrSolid ? "Solid" :
                              /*other*/              "UNKNOWN!");
qDebug() << fillIndex << fillTypeName;

            if( fill )
                mDocument->insertFill( fillIndex, fill );
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
            CdrOutline* outline = new CdrOutline;

            const QByteArray outlineBlob = mRiffStreamReader.chunkData();
            const CdrOutlineData* outlineData = dataPtr<CdrOutlineData>( outlineBlob );
            outline->setType( outlineData->mType );

qDebug() << outlineData->mIndex << outline->type();

            mDocument->insertOutline( outlineData->mIndex, outline );
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
            CdrStyle* style = new CdrStyle;

            const QByteArray styleData = mRiffStreamReader.chunkData();
            // 0..1: int16 index/key/id?
            const quint16 styleIndex = data<quint16>( styleData );
            // 2..end: data
            const CdrStyleArgumentData* styleArgs =
                dataPtr<CdrStyleArgumentData>( styleData, 2 );
qDebug()<<"Style id:"<<styleIndex<<"args:"<<styleArgs->count<<"d:"<<styleArgs->_unknown0<<styleArgs->_unknown1<<styleArgs->_unknown2
                                              <<styleArgs->_unknown3<<styleArgs->_unknown4;

// Arg types:
// 200   text/title/name
// 205: 32bit 02 00 00 00
// 210: 32bit 02 00 00 00
// 220: 6 bytes CdrStyleFontArgumentData
// 225: 16 bit
// 230: 20 bytes (all 00)
// 235: 12 bytes (00 00 64 00 64 00 64 00 00 00 00 00)
// 240: 258 bytes (all 00)
// 245: 8 bytes (all 00)
// 250: 32bit (00 00 00 00)
            for (int i=0; i < styleArgs->count; i++)
            {
                const quint16 argType = styleArgs->argType(i);
QString argAsString;
QString argTypeAsString;
switch(argType)
{
    case 205 :
    case 210 :
    case 250 :
        argTypeAsString = QLatin1String("some 32-bit");
        argAsString = QString::number( data<quint32>(styleData, styleArgs->argOffsets()[i]+2) );
        break;
    case 225 :
        argTypeAsString = QLatin1String("some 16-bit");
        argAsString = QString::number( data<quint16>(styleData, styleArgs->argOffsets()[i]+2) );
        break;
    case 200 :
    {
        const QString title = stringData( styleData, styleArgs->argOffsets()[i]+2 );
        style->setTitle( title );

        argTypeAsString = QLatin1String("title");
        argAsString = title;
        break;
    }
    case 220:
    {
        const CdrStyleFontArgumentData* fontData =
            dataPtr<CdrStyleFontArgumentData>( styleData, styleArgs->argOffsets()[i]+2 );
        style->setFontId( fontData->mFontIndex );

        argTypeAsString = QLatin1String("font");
        argAsString = QString::number( fontData->mFontIndex ) + QLatin1Char(' ') +
                      QString::number( fontData->_unknown0 ) + QLatin1Char(' ') +
                      QString::number( fontData->_unknown1 );
        break;
    }
    case 230:
    case 235:
    case 240:
    case 245:
        argTypeAsString = QLatin1String("larger data");
        break;
    default:
        argTypeAsString = QLatin1String("UNKNOWN!");
        break;
}
qDebug() << i << ": type" << argType << argTypeAsString << argAsString;
            }
            mDocument->insertStyle( styleIndex, style );
        }
    }

    mRiffStreamReader.closeList();
}


void
CdrParser::readDocBtxTable()
{
    mRiffStreamReader.openList();
qDebug() << "Reading Btx Table...";
    while( mRiffStreamReader.readNextChunkHeader() )
    {
        if( (mRiffStreamReader.chunkId() == strlId) &&
            mRiffStreamReader.isListChunk() )
        {
            readStrl();
        }
    }

    mRiffStreamReader.closeList();
}


void
CdrParser::readStrl()
{
    mRiffStreamReader.openList();
qDebug() << "Reading Strl...";
    while( mRiffStreamReader.readNextChunkHeader() )
    {
        if( (mRiffStreamReader.chunkId() == btidId) )
        {
            const QByteArray btidData = mRiffStreamReader.chunkData();
            // 0..1: ?
            const quint16 btid = data<quint16>( btidData );
        }
        else if( (mRiffStreamReader.chunkId() == parlId) &&
            mRiffStreamReader.isListChunk() )
        {
            readParl();
        }
    }

    mRiffStreamReader.closeList();
}

void
CdrParser::readParl()
{
    mRiffStreamReader.openList();
qDebug() << "Reading Parl...";
    while( mRiffStreamReader.readNextChunkHeader() )
    {
        if( (mRiffStreamReader.chunkId() == paraId) )
        {
            const QByteArray paraData = mRiffStreamReader.chunkData();
            // 0..5: ?
        }
        else if( (mRiffStreamReader.chunkId() == bnchId) )
        {
            const QByteArray bnchData = mRiffStreamReader.chunkData();
            // 0..?: ?  sizes seen are 52, 72, 80, 4
        }
        else if( (mRiffStreamReader.chunkId() == bschId) )
        {
            const QByteArray bschData = mRiffStreamReader.chunkData();
            // 0..23: ?
        }
    }

    mRiffStreamReader.closeList();
}


CdrPage*
CdrParser::readPage()
{
    CdrPage* page = new CdrPage;

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
                    CdrLayer* layer = readLayer();
                    if( layer )
                        page->addLayer( layer );
                }
            }

            mRiffStreamReader.closeList();
        }
    }

    mRiffStreamReader.closeList();

    return page;
}

void
CdrParser::readPageFlags()
{
// 000C:3990 |       40 00  01 90                                 |   @...          
// 000C:3B00 |                                 40 00  00 90       |           @...  
// 000C:8FE0 |                           00 00 00 90              |         ....    

}


CdrLayer*
CdrParser::readLayer()
{
    CdrLayer* layer = new CdrLayer();

    mRiffStreamReader.openList();

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
        else if( (chunkId == lnkgId) &&
                 mRiffStreamReader.isListChunk() )
        {
            CdrLinkGroupObject* group = readLinkGroupObject();
            if( group )
                layer->addObject( group );
        }
        else if( (chunkId == grp_Id) &&
                 mRiffStreamReader.isListChunk() )
        {
            CdrGroupObject* group = readGroupObject();
            if( group )
                layer->addObject( group );
        }
        else if( (chunkId == obj_Id) &&
                 mRiffStreamReader.isListChunk() )
        {
            CdrObject* object = readObject();
            if( object )
                layer->addObject( object );
        }
    }

qDebug() << "Layer >>>";
    mRiffStreamReader.closeList();

    return layer;
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

CdrLinkGroupObject*
CdrParser::readLinkGroupObject()
{
    CdrLinkGroupObject* group = new CdrLinkGroupObject();
//     group->setZIndex(m_context.nextZIndex());
// TODO: register all created in a lookup table, or not needed?
//     shape->setName(id);
//     m_context.registerShape(id, shape);

    mRiffStreamReader.openList();
qDebug() << "LinkGroup <<<";

    while( mRiffStreamReader.readNextChunkHeader() )
    {
        const Koralle::FourCharCode chunkId = mRiffStreamReader.chunkId();

        if( mRiffStreamReader.chunkId() == spndId )
        {
            const QByteArray spndData = mRiffStreamReader.chunkData();
            // 0..1: ?
            const quint16 spnd = data<quint16>( spndData );
        }
        else if( mRiffStreamReader.chunkId() == flgsId )
        {
            const QByteArray flagsData = mRiffStreamReader.chunkData();
            // 0..3: ?
            const quint32 flags = data<quint32>( flagsData );
        }
        else if( (chunkId == obj_Id) &&
                 mRiffStreamReader.isListChunk() )
        {
            CdrObject* object = readObject();
            if( object )
                group->addObject( object );
        }
    }

qDebug() << "LinkGroup >>>...";
    mRiffStreamReader.closeList();
    return group;
}


CdrGroupObject*
CdrParser::readGroupObject()
{
    CdrGroupObject* group = new CdrGroupObject();
//     group->setZIndex(m_context.nextZIndex());
// TODO: register all created in a lookup table, or not needed?
//     shape->setName(id);
//     m_context.registerShape(id, shape);

    mRiffStreamReader.openList();
qDebug() << "Group <<<";

    while( mRiffStreamReader.readNextChunkHeader() )
    {
        const Koralle::FourCharCode chunkId = mRiffStreamReader.chunkId();

        if( mRiffStreamReader.chunkId() == spndId )
        {
            const QByteArray spndData = mRiffStreamReader.chunkData();
            // 0..1: ?
            const quint16 spnd = data<quint16>( spndData );
        }
        else if( mRiffStreamReader.chunkId() == flgsId )
        {
            const QByteArray flagsData = mRiffStreamReader.chunkData();
            // 0..3: ?
            const quint32 flags = data<quint32>( flagsData );
        }
        else if( mRiffStreamReader.chunkId() == bboxId )
        {
            const QByteArray bboxData = mRiffStreamReader.chunkData();
            // 0..7: Cdr4BoundingBox
            const Cdr4BoundingBox* boundingBox = dataPtr<Cdr4BoundingBox>( bboxData );
        }
        else if( (chunkId == grp_Id) &&
                 mRiffStreamReader.isListChunk() )
        {
            CdrGroupObject* object = readGroupObject();
            if( object )
                group->addObject( object );
        }
        else if( (chunkId == obj_Id) &&
                 mRiffStreamReader.isListChunk() )
        {
            CdrObject* object = readObject();
            if( object )
                group->addObject( object );
        }
    }

qDebug() << "Group >>>...";
    mRiffStreamReader.closeList();
    return group;
}


CdrObject*
CdrParser::readObject()
{
    CdrObject* object = 0;

    mRiffStreamReader.openList();
qDebug() << "Object <<<";

    while( mRiffStreamReader.readNextChunkHeader() )
    {
        const Koralle::FourCharCode chunkId = mRiffStreamReader.chunkId();

        if( (mRiffStreamReader.chunkId() == spndId) )
        {
            const QByteArray spndData = mRiffStreamReader.chunkData();
            // 0..1: ?
            const quint16 spnd = data<quint16>( spndData );
        }
        else if( (mRiffStreamReader.chunkId() == flgsId) )
        {
            const QByteArray flagsData = mRiffStreamReader.chunkData();
            // 0..3: flags
            const quint32 flags = data<quint32>( flagsData );
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


CdrObject*
CdrParser::readObjectLGOb()
{
    CdrObject* object = 0;
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
            const CdrArgumentWithTypeData* argsData = dataPtr<CdrArgumentWithTypeData>( trfdData );
qDebug() << "Reading Trfd" << argsData->count << "args";
    for (int i=0; i < argsData->count; i++)
    {
// qDebug() << i << ": type" << argsData->argPtr<>()<TransformData>(i);
    }
        }
    }

    mRiffStreamReader.closeList();
}


CdrObject*
CdrParser::readLoda()
{
    CdrObject* object = 0;

    const QByteArray lodaData = mRiffStreamReader.chunkData();

    const CdrObjectArgumentData* argsData = dataPtr<CdrObjectArgumentData>( lodaData );

qDebug() << "Reading Loda" << argsData->count << "args, loda type" << argsData->chunkType;
    if( argsData->chunkType == CdrRectangleObjectId )
        object = readRectangleObject( argsData );
    if( argsData->chunkType == CdrEllipseObjectId )
        object = readEllipseObject( argsData );
    else if( argsData->chunkType == CdrPathObjectId )
        object = readPathObject( argsData );
    else if( argsData->chunkType == CdrTextObjectId )
        object = readTextObject( argsData );

    for (int i=0; i < argsData->count; i++)
    {
const quint16 argType = argsData->argType(i);
QString argAsString;
QString argTypeAsString;
switch(argType)
{
    case 10 :
        argAsString = QString::number( data<quint32>(lodaData, argsData->argOffsets()[i]) );
        argTypeAsString = QLatin1String("outline index");
        break;
    case 20 :
        argAsString = QString::number( data<quint32>(lodaData, argsData->argOffsets()[i]) );
        argTypeAsString = QLatin1String("fill index");
        break;
    case 30 :
        argTypeAsString = QLatin1String("object specific data");
        break;
    case 40 :
        argAsString = QString::number( data<quint32>(lodaData, argsData->argOffsets()[i]) );
        argTypeAsString = QLatin1String("some 32-bit");
        break;
    case 200 :
        argAsString = QString::number( data<quint16>(lodaData, argsData->argOffsets()[i]) );
        argTypeAsString = QLatin1String("style index");
        break;
    case 1010 :
        argAsString = QString::number( data<quint16>(lodaData, argsData->argOffsets()[i]) );
        argTypeAsString = QLatin1String("some 16-bit");
        break;
    case 1000 :
        argAsString = stringData( lodaData, argsData->argOffsets()[i] );
        argTypeAsString = QLatin1String("title");
        break;
    case 100 :
    {
        Cdr4Point point = data<Cdr4Point>( lodaData, argsData->argOffsets()[i] );
        argAsString = QString::number(point.mX)+QLatin1Char(',')+QString::number(point.mY);
        argTypeAsString = QLatin1String("point?");
        break;
    }
    case 2000:
        argTypeAsString = QLatin1String("some larger data");
        break;
    default:
        argTypeAsString = QLatin1String("UNKNOWN!");
        break;
}
qDebug() << i << ": type" << argType << argTypeAsString << argAsString;

// each page has as start types 0B, 0C, 11, 0
// first set has both arg type 1000 and 2000, with text set for 1000, other have just 2000, no text
// types 3 and 4 are on second, type 5 on last
// type 3 and 5 have args 10, 20, 30, 100, 200, 1010 (1010 sometimes missing)

// Arg types:
// 10  32bit (outline?)
// 20  32bit (fill?)
// 30  object specific data
// 40  32bit  seen only with ellipse so far
// 100 32bit (point?)
// 200 16 bit  (style index?)
// 1000: text/title
// 1010 16 bit
// 2000: data 01 00 64 00 64 00 00 00 00 00 00 00

// type 2: ellipse
//  200:    5    (05 00)
//  100: 197636  (04 04 03 00)
//   40:         (28 FD 14 FE)
//   30:         (...)
//   20:   15    (0F 00 00 00)
//   10:    3    (03 00 00 00)

// type 5: bitmap?
// 1010:  320    (40 01)
//  200:    5    (05 00)
//  100: 2756    (C4 0A 00 00)
//   30:         (...) 44 bytes
//   20:    2    (02 00 00 00)
//   10:    2    (02 00 00 00)

// type 4: text
//  200:   29    (1D 00)
//  100:         (94 06 EA 01)
//   30:         (...)
//   20:    1    (01 00 00 00)
//   10:    2    (01 00 00 00)

// type 4: text
//  200:   32    (20 00)
//  100:         (64 07 FE 01)
//   30:         (...)
//   20:    1    (01 00 00 00)
//   10:    2    (01 00 00 00)

// type 3: line or curve
//  200:    5    (05 00)
//  100: 1972    (B4 07 00 00)
//   30:         (...) 64 bytes
//   20:    1    (01 00 00 00)
//   10:    2    (02 00 00 00)

// type 3: line or curve
// 1010: 32768   (01 80)
//  200:    5    (05 00)
//  100: 1940    (94 07 00 00)
//   30:         (...) 872 bytes
//   20:  256    (00 01 00 00)
//   10: 1024    (00 04 00 00)
    }

    return object;
}


CdrRectangleObject*
CdrParser::readRectangleObject( const CdrArgumentWithTypeData* argsData )
{
    CdrRectangleObject* rectangleObject = new CdrRectangleObject;

    for (int i=0; i < argsData->count; i++)
    {
        if( argsData->argType(i) == 30 )
        {
            const Cdr4RectangleData* rectangleData = argsData->argPtr<Cdr4RectangleData>( i );
            rectangleObject->setSize( rectangleData->mWidth, rectangleData->mHeight );
qDebug() << "rectangle: width" << rectangleObject->width()<<"height"<<rectangleObject->height()
                 << "unknown" << rectangleData->_unknown;
        }
    }

    return rectangleObject;
}

CdrEllipseObject*
CdrParser::readEllipseObject( const CdrArgumentWithTypeData* argsData )
{
    CdrEllipseObject* ellipseObject = new CdrEllipseObject;

    for (int i=0; i < argsData->count; i++)
    {
        if( argsData->argType(i) == 30 )
        {
            const Cdr4EllipseData* ellipseData = argsData->argPtr<Cdr4EllipseData>( i );
            ellipseObject->setCenterPoint(ellipseData->mCenterPoint);
            ellipseObject->setXRadius(ellipseData->mXRadius);
            ellipseObject->setYRadius(ellipseData->mYRadius);
qDebug() << "ellipse: center"<<ellipseData->mCenterPoint.mX<<","<<ellipseData->mCenterPoint.mY
                     <<"xradius"<<ellipseData->mXRadius<<"yradius"<<ellipseData->mYRadius
                     <<"unknown"<<ellipseData->_unknown;
        }
    }

    return ellipseObject;
}

CdrPathObject*
CdrParser::readPathObject( const CdrArgumentWithTypeData* argsData )
{
    CdrPathObject* pathObject = new CdrPathObject();

    for (int i=0; i < argsData->count; i++)
    {
        if( argsData->argType(i) == 30 )
        {
            const Cdr4PointList* points = argsData->argPtr<Cdr4PointList>( i );
qDebug() << "path points:" << points->count;
            for (unsigned int j=0; j<points->count; j++)
            {
                pathObject->addPathPoint( Cdr4PathPoint(points->point(j), points->pointType(j)) );
            }
        }
    }

    return pathObject;
}

CdrTextObject*
CdrParser::readTextObject( const CdrArgumentWithTypeData* argsData )
{
    CdrTextObject* textObject = new CdrTextObject;

    for (int i=0; i < argsData->count; i++)
    {
        // 10, 20, 30, 100, 200
        if( argsData->argType(i) == 30 )
        {
            const Cdr4TextData* textData = argsData->argPtr<Cdr4TextData>( i );
            QString text;
            for (unsigned int j=0; j<textData->mLength; j++)
                text.append( QLatin1Char(textData->charData(j).mChar) );
qDebug() << "text:" << text;
        }
    }

    return textObject;
}
