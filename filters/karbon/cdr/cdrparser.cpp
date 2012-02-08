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
#include <QtGui/QColor>
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


static QColor
color( const CdrColor& cdrColor, quint32 colorModel )
{
    if( colorModel == CdrPantoneId )
        // pantone needs user to supply data it seems, as wikipedia claims the pantone definers are nasty
        // whatever, this formula results in pretty similar grey tones for my check picture
        return QColor::fromCmyk(0, 0, 0, cdrColor.m1*255/100);
//     { return QColor::fromCmyk(mColor.mC, mColor.mM, mColor.mY, mColor.mK); }
//     { return QColor::fromCmyk(0, 0, 0, 1); }]]></code>
    return QColor();
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
            (4 == mCdrVersion)/*(4 <= mCdrVersion) && (mCdrVersion <= 5)*/ )
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
    const QByteArray versionChunk = mRiffStreamReader.chunkData();
    const CdrVersionChunkData* versionData = dataPtr<CdrVersionChunkData>( versionChunk );

    mDocument->setFullVersion( versionData->mVersion );

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
    const QByteArray mcfgChunk = mRiffStreamReader.chunkData();
    const MCfgChunkData* mcfgData = dataPtr<MCfgChunkData>( mcfgChunk );

    // set the page size
    mDocument->setSize( mcfgData->mWidth, mcfgData->mHeight );
}

void
CdrParser::readDocGuid()
{
    const QByteArray guidData = mRiffStreamReader.chunkData();
}

void
CdrParser::readDocStsh()
{
    const QByteArray styleSheetChunk = mRiffStreamReader.chunkData();
    const CdrStyleSheetChunkData* styleSheetData = dataPtr<CdrStyleSheetChunkData>( styleSheetChunk );

    mDocument->setStyleSheetFileName( QLatin1String(styleSheetData->fileName()) );
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
            CdrFont* font = new CdrFont;

            const QByteArray fontChunk = mRiffStreamReader.chunkData();
            const CdrFontChunkData* fontData = dataPtr<CdrFontChunkData>( fontChunk );

            font->setName( QLatin1String(fontData->fontName()) );

qDebug() << fontData->mFontIndex << font->name();
            mDocument->insertFont( fontData->mFontIndex, font );
        }
    }

    mRiffStreamReader.closeList();
}

void
CdrParser::readDocLnkTable()
{
    const QByteArray lnkTableChunk = mRiffStreamReader.chunkData();
    const CdrLnkTableChunkData* lnkTableData = dataPtr<CdrLnkTableChunkData>( lnkTableChunk );

qDebug() << "Reading LnkTable" << lnkTableData->mArguments.count << "args";
    for (int i=0; i < lnkTableData->mArguments.count; i++)
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

            const QByteArray fillChunk = mRiffStreamReader.chunkData();
            const CdrFillChunkData* fillData = dataPtr<CdrFillChunkData>( fillChunk );

            QString fillDataString;
            if( fillData->mFillType == CdrTransparent )
            {
                fill = new CdrTransparentFill;
                // transparent has no other data stored
            }
            else if( fillData->mFillType == CdrSolid )
            {
                CdrSolidFill* solidFill = new CdrSolidFill;
                const CdrSolidFillData* solidFillData = fillData->solidFillData();
                solidFill->setColor( color(solidFillData->mColor, solidFillData->mColorModel) );

                fill = solidFill;

                fillDataString = solidFill->color().name();
            }

            const QString fillTypeName =
                QLatin1String(fillData->mFillType == CdrTransparent ? "Transparent" :
                              fillData->mFillType == CdrSolid ?       "Solid" :
                              /*other*/                               "UNKNOWN!");
qDebug() << fillData->mFillIndex << fillTypeName << fillDataString;

            if( fill )
                mDocument->insertFill( fillData->mFillIndex, fill );
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

            const QByteArray outlineChunk = mRiffStreamReader.chunkData();
            const CdrOutlineChunkData* outlineData = dataPtr<CdrOutlineChunkData>( outlineChunk );

            outline->setType( outlineData->mType );
            outline->setLineWidth( outlineData->mLineWidth );
            outline->setColor( color(outlineData->mFillData.mColor, outlineData->mFillData.mColorModel) );

qDebug() << outlineData->mIndex << outline->type() << outline->color().name();

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

            const QByteArray styleChunk = mRiffStreamReader.chunkData();
            const CdrStyleChunkData* styleData = dataPtr<CdrStyleChunkData>( styleChunk );

            const CdrStyleArgumentData* styleArgs = &styleData->mArguments;
qDebug()<<"Style id:"<<styleData->mStyleIndex<<"args:"<<styleArgs->count<<"d:"<<styleArgs->_unknown0<<styleArgs->_unknown1<<styleArgs->_unknown2
                                              <<styleArgs->_unknown3<<styleArgs->_unknown4;

            for (int i=0; i < styleArgs->count; i++)
            {
                const quint16 argType = styleArgs->argType(i);
QString argAsString;
QString argTypeAsString;
switch(argType)
{
    case CdrStyle205ArgumentId :
    case CdrStyle210ArgumentId :
    case CdrStyle250ArgumentId :
        argTypeAsString = QLatin1String("some 32-bit");
        argAsString = QString::number( styleArgs->arg<quint32>(i) );
        break;
    case CdrStyle225ArgumentId :
        argTypeAsString = QLatin1String("some 16-bit");
        argAsString = QString::number( styleArgs->arg<quint16>(i) );
        break;
    case CdrStyleTitleArgumentId :
    {
        const QString title = QLatin1String( styleArgs->argPtr<char>(i) );
        style->setTitle( title );

        argTypeAsString = QLatin1String("title");
        argAsString = title;
        break;
    }
    case CdrStyleFontArgumentId:
    {
        const CdrStyleFontArgumentData* fontData = styleArgs->argPtr<CdrStyleFontArgumentData>( i );
        style->setFontId( fontData->mFontIndex );
        style->setFontSize( fontData->mFontSize );

        argTypeAsString = QLatin1String("font");
        argAsString = QString::number( fontData->mFontIndex ) + QLatin1Char(' ') +
                      QString::number( fontData->mFontSize) + QLatin1Char(' ') +
                      QString::number( fontData->_unknown1) + QLatin1Char(' ') +
                      QString::number( fontData->_unknown2) + QLatin1Char(' ') +
                      QString::number( fontData->_unknown3);
        break;
    }
    case CdrStyle230ArgumentId:
    case CdrStyle235ArgumentId:
    case CdrStyle240ArgumentId:
    case CdrStyle245ArgumentId:
        argTypeAsString = QLatin1String("larger data");
        break;
    default:
        argTypeAsString = QLatin1String("UNKNOWN!");
        break;
}
qDebug() << i << ": type" << argType << argTypeAsString << argAsString;
            }
            mDocument->insertStyle( styleData->mStyleIndex, style );
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
            const QByteArray bboxChunk = mRiffStreamReader.chunkData();
            const Cdr4BoundingBoxChunkData* boundingBoxData = dataPtr<Cdr4BoundingBoxChunkData>( bboxChunk );
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
            const QByteArray trfdChunk = mRiffStreamReader.chunkData();
            const CdrTrflChunkData* trfdData = dataPtr<CdrTrflChunkData>( trfdChunk );

qDebug() << "Reading Trfd" << trfdData->mArguments.count << "args" << trfdData->_unknown0 <<trfdData->_unknown1;
    for (int i=0; i < trfdData->mArguments.count; i++)
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

    const QByteArray lodaChunk = mRiffStreamReader.chunkData();

    const CdrObjectArgumentData* argsData = dataPtr<CdrObjectArgumentData>( lodaChunk );

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
    case CdrObjectOutlineIndexArgumentId :
        argAsString = QString::number( argsData->arg<quint32>(i) );
        argTypeAsString = QLatin1String("outline index");
        break;
    case CdrObjectFillIndexArgumentId :
        argAsString = QString::number( argsData->arg<quint32>(i) );
        argTypeAsString = QLatin1String("fill index");
        break;
    case CdrObjectSpecificDataArgumentId :
        argTypeAsString = QLatin1String("object specific data");
        break;
    case CdrObject40ArgumentId :
        argAsString = QString::number( argsData->arg<quint32>(i) );
        argTypeAsString = QLatin1String("some 32-bit");
        break;
    case CdrObjectStyleIndexArgumentId :
        argAsString = QString::number( argsData->arg<quint16>(i) );
        argTypeAsString = QLatin1String("style index");
        break;
    case CdrObject1010ArgumentId :
        argAsString = QString::number( argsData->arg<quint16>(i) );
        argTypeAsString = QLatin1String("some 16-bit");
        break;
    case CdrObjectTitleArgumentId :
        argAsString = QLatin1String( argsData->argPtr<char>(i) ); // TODO: all argPtr<char> need \0 term check
        argTypeAsString = QLatin1String("title");
        break;
    case CdrObject100ArgumentId :
    {
        const Cdr4Point point = argsData->arg<Cdr4Point>( i );
        argAsString = QString::number(point.mX)+QLatin1Char(',')+QString::number(point.mY);
        argTypeAsString = QLatin1String("point?");
        break;
    }
    case CdrObject2000ArgumentId :
        argTypeAsString = QLatin1String("some larger data");
        break;
    default:
        argTypeAsString = QLatin1String("UNKNOWN!");
        break;
}
qDebug() << i << ": type" << argType << argTypeAsString << argAsString;

    }

    return object;
}


CdrRectangleObject*
CdrParser::readRectangleObject( const CdrArgumentWithTypeData* argsData )
{
    CdrRectangleObject* rectangleObject = new CdrRectangleObject;

    for (int i=0; i < argsData->count; i++)
    {
        const quint16 argType = argsData->argType(i);
        const quint16 argOffset = argsData->argOffsets()[i];

        switch( argType )
        {
        case CdrObjectOutlineIndexArgumentId :
            rectangleObject->setOutlineId( argsData->arg<quint32>(i) );
            break;
        case CdrObjectFillIndexArgumentId :
            rectangleObject->setFillId( argsData->arg<quint32>(i) );
            break;
        case CdrObjectSpecificDataArgumentId :
        {
            const Cdr4RectangleData* rectangleData = argsData->argPtr<Cdr4RectangleData>( i );
            rectangleObject->setSize( rectangleData->mWidth, rectangleData->mHeight );
qDebug() << "rectangle: width" << rectangleObject->width()<<"height"<<rectangleObject->height()
                 << "unknown" << rectangleData->_unknown;
        }
        case CdrObjectStyleIndexArgumentId :
            rectangleObject->setStyleId( argsData->arg<quint16>(i) );
            break;
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
        const quint16 argType = argsData->argType(i);
        const quint16 argOffset = argsData->argOffsets()[i];

        switch( argType )
        {
        case CdrObjectOutlineIndexArgumentId :
            ellipseObject->setOutlineId( argsData->arg<quint32>(i) );
            break;
        case CdrObjectFillIndexArgumentId :
            ellipseObject->setFillId( argsData->arg<quint32>(i) );
            break;
        case CdrObjectSpecificDataArgumentId :
        {
            const Cdr4EllipseData* ellipseData = argsData->argPtr<Cdr4EllipseData>( i );
            ellipseObject->setCenterPoint(ellipseData->mCenterPoint);
            ellipseObject->setXRadius(ellipseData->mXRadius);
            ellipseObject->setYRadius(ellipseData->mYRadius);
qDebug() << "ellipse: center"<<ellipseData->mCenterPoint.mX<<","<<ellipseData->mCenterPoint.mY
                     <<"xradius"<<ellipseData->mXRadius<<"yradius"<<ellipseData->mYRadius
                     <<"unknown"<<ellipseData->_unknown;
        }
        case CdrObjectStyleIndexArgumentId :
            ellipseObject->setStyleId( argsData->arg<quint16>(i) );
            break;
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
        const quint16 argType = argsData->argType(i);
        const quint16 argOffset = argsData->argOffsets()[i];

        switch( argType )
        {
        case CdrObjectOutlineIndexArgumentId :
            pathObject->setOutlineId( argsData->arg<quint32>(i) );
            break;
        case CdrObjectFillIndexArgumentId :
            pathObject->setFillId( argsData->arg<quint32>(i) );
            break;
        case CdrObjectSpecificDataArgumentId :
        {
            const Cdr4PointList* points = argsData->argPtr<Cdr4PointList>( i );
qDebug() << "path points:" << points->count;
            for (unsigned int j=0; j<points->count; j++)
            {
                pathObject->addPathPoint( Cdr4PathPoint(points->point(j), points->pointType(j)) );
            }
        }
        case CdrObjectStyleIndexArgumentId :
            pathObject->setStyleId( argsData->arg<quint16>(i) );
            break;
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
        const quint16 argType = argsData->argType(i);
        const quint16 argOffset = argsData->argOffsets()[i];

        switch( argType )
        {
        case CdrObjectOutlineIndexArgumentId :
            textObject->setOutlineId( argsData->arg<quint32>(i) );
            break;
        case CdrObjectFillIndexArgumentId :
            textObject->setFillId( argsData->arg<quint32>(i) );
            break;
        case CdrObjectSpecificDataArgumentId :
        {
            const Cdr4TextData* textData = argsData->argPtr<Cdr4TextData>( i );
            QString text;
            for (unsigned int j=0; j<textData->mLength; j++)
                text.append( QLatin1Char(textData->charData(j).mChar) );
            textObject->setText( text );
qDebug() << "text:" << text;
        }
        case CdrObjectStyleIndexArgumentId :
            textObject->setStyleId( argsData->arg<quint16>(i) );
            break;
        }
    }

    return textObject;
}
