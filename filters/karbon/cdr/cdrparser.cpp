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
static const Koralle::FourCharCode spndId('s','p','n','d');
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
const T&
dataRef( const QByteArray& data, int offset = 0 )
{
    return reinterpret_cast<const T&>(data.constData()[offset]);
}


static QColor
color( const Cdr4Color& cdrColor, quint32 colorModel )
{
    if( colorModel == Cdr4ColorPantoneId )
        // pantone needs user to supply data it seems, as wikipedia claims the pantone definers are nasty
        // whatever, this formula results in pretty similar grey tones for my check picture
        return QColor::fromCmyk(0, 0, 0, cdrColor.d1()*255/100);
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

    bool isFirstPage = true;
    while( mRiffStreamReader.readNextChunkHeader() )
    {
        const Koralle::FourCharCode chunkId = mRiffStreamReader.chunkId();

        // TODO: where to check fixed sizes for correctness?
        if( chunkId == vrsnId )
            readVersion();
        else if( chunkId == dispId )
            readDisp();
        else if( chunkId == infoId )
            readInfo();
        else if( chunkId == doc_Id )
            readDoc();
        else if( chunkId == pageId )
        {
            CdrPage* const page = readPage();
            if( page )
            {
                if( isFirstPage )
                {
                    mDocument->setMasterPage( page );
                    isFirstPage = false;
                }
                else
                    mDocument->addPage( page );
            }
        }
    }

    mRiffStreamReader.closeList();
}

void
CdrParser::readVersion()
{
    const QByteArray versionChunk = mRiffStreamReader.chunkData();
    const Cdr4VersionChunkData& versionData = dataRef<Cdr4VersionChunkData>( versionChunk );

    mDocument->setFullVersion( versionData.version() );

qDebug() << "Version:" << static_cast<qreal>(mDocument->fullVersion())/100;
}

void
CdrParser::readDoc()
{
    mRiffStreamReader.openList();

    while( mRiffStreamReader.readNextChunkHeader() )
    {
        const Koralle::FourCharCode chunkId = mRiffStreamReader.chunkId();

        if( chunkId == stshId )
            readDocStsh();
        else if( chunkId == mcfgId )
            readDocMCfg();
        else if( chunkId == guidId )
            readDocGuid();
        else if( chunkId == fnttId )
            readDocFontTable();
        else if( chunkId == bmptId )
            readDocBitmapTable();
        else if( chunkId == lnktId )
            readDocLinkTable();
        else if( chunkId == vectId )
            readDocVecTable();
        else if( chunkId == filtId )
            readDocFillTable();
        else if( chunkId == otltId )
            readDocOutlineTable();
        else if( chunkId == stltId )
            readDocStyleTable();
        else if( chunkId == btxtId )
            readDocBlockTextTable();
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
    const Cdr4MCfgChunkData& mcfgData = dataRef<Cdr4MCfgChunkData>( mcfgChunk );

    // set the page size
    mDocument->setSize( mcfgData.width(), mcfgData.height() );
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
    const Cdr4StyleSheetChunkData& styleSheetData = dataRef<Cdr4StyleSheetChunkData>( styleSheetChunk );

    mDocument->setStyleSheetFileName( QLatin1String(styleSheetData.fileName()) );
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
            const Cdr4FontChunkData& fontData = dataRef<Cdr4FontChunkData>( fontChunk );

            font->setName( QLatin1String(fontData.fontName()) );

qDebug() << fontData.fontIndex() << font->name();
            mDocument->insertFont( fontData.fontIndex(), font );
        }
    }

    mRiffStreamReader.closeList();
}

void
CdrParser::readDocLinkTable()
{
    CdrBlockTextLinkTable blockTextLinkTable;

    const QByteArray linkTableChunk = mRiffStreamReader.chunkData();
    const Cdr4LinkTableChunkData& linkTableData = dataRef<Cdr4LinkTableChunkData>( linkTableChunk );
    const Cdr4ArgumentListData& arguments = linkTableData.arguments();

qDebug() << "Reading LnkTable" << arguments.count() << "args";
    for (int i=0; i < arguments.count(); i++)
    {
        const Cdr4LinkData& linkData = arguments.argRef<Cdr4LinkData>(i);

qDebug() << i << ": type:" << linkData.type();
        switch( linkData.type() )
        {
        case Cdr4LinkBlockTextId:
        {
            const Cdr4LinkBlockTextData& blockTextData = linkData.blockTextData();

            QString objectIndizes;
            for( int s = 0; s<blockTextData.objectCount(); ++s )
            {
                blockTextLinkTable.insert( blockTextData.objectIndex(s),
                                           CdrBlockTextPartIndex(blockTextData.textIndex(),s) );
                objectIndizes += QString::number(blockTextData.objectIndex(s)) + QLatin1Char(' ');
            }
qDebug() << "block text:" << blockTextData.textIndex() << "in" << objectIndizes <<"other:"
                    << blockTextData._unknown1() << blockTextData._unknown2() << blockTextData._unknown3()
                    << blockTextData._unknown4() << blockTextData._unknown5() << blockTextData._unknown6();
            break;
        }
        default:
        {
            const char* const argumentData = &arguments.argRef<char>(i);
            const int dataSize = &arguments.argRef<char>(i+1) - argumentData;
qDebug() << QByteArray::fromRawData(argumentData, dataSize).toHex();
        }
        }
    }

    mDocument->setBlockTextLinkTable( blockTextLinkTable );
}

void
CdrParser::readDocVecTable()
{
qDebug() << "Reading Vec Table";
    mRiffStreamReader.openList();

    while( mRiffStreamReader.readNextChunkHeader() )
    {
        if( mRiffStreamReader.chunkId() == grp_Id )
        {
            /*CdrGroupObject* group = */readGroupObject();
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
            const Cdr4FillChunkData& fillData = dataRef<Cdr4FillChunkData>( fillChunk );

            QString fillDataString;
            switch( fillData.fillType() )
            {
            case Cdr4FillTransparentId:
                fill = new CdrTransparentFill;
                // transparent has no other data stored
                break;
            case Cdr4FillSolidId:
            {
                CdrSolidFill* solidFill = new CdrSolidFill;
                const Cdr4FillSolidData& solidFillData = fillData.fillSolidData();
                solidFill->setColor( color(solidFillData.color(), solidFillData.colorModel()) );

                fill = solidFill;

                fillDataString = solidFill->color().name();
                break;
            }
            case Cdr4Fill11Id:
            {
                const Cdr4Fill11Data& unknown11FillData = fillData.fill11Data();

                fillDataString = QLatin1String(unknown11FillData.originalName())+QLatin1Char('|')+
                    QLatin1String(unknown11FillData.localizedName());
                break;
            }
            }

            const QString fillTypeName =
                QLatin1String(fillData.fillType() == Cdr4FillTransparentId ? "Transparent" :
                              fillData.fillType() == Cdr4FillSolidId ?       "Solid" :
                              fillData.fillType() == Cdr4FillGradientId ?    "Gradient" :
                              fillData.fillType() == Cdr4Fill7Id ?           "Unknown type 7" :
                              fillData.fillType() == Cdr4Fill11Id ?          "Unknown type 11" :
                              /*other*/                                      "UNKNOWN!");
qDebug() << fillData.fillIndex() << fillData.fillType() << fillTypeName << fillDataString;

            if( fill )
                mDocument->insertFill( fillData.fillIndex(), fill );
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
            const Cdr4OutlineChunkData& outlineData = dataRef<Cdr4OutlineChunkData>( outlineChunk );

            const CdrStrokeCapType capType =
                (outlineData.strokeCapType() == Cdr4StrokeCapRoundId) ? CdrStrokeRoundCap : CdrStrokeButtCap;
            const CdrStrokeJoinType joinType =
                (outlineData.strokeJoinType() == Cdr4StrokeJoinRoundId) ? CdrStrokeRoundJoin : CdrStrokeMiterJoin;
            outline->setStrokeType( outlineData.strokeType() );
            outline->setStrokeCapType( capType );
            outline->setStrokeJoinType( joinType );
            outline->setStrokeWidth( outlineData.strokeWidth() );
            outline->setColor( color(outlineData.fillData().color(), outlineData.fillData().colorModel()) );

            const QByteArray endData = QByteArray::fromRawData(outlineData._unknown11Ptr(), outlineData._unknown11Count());
qDebug() << "id:" << outlineData.index()
         << "stroke type:" << outline->strokeType()
         << "caps type:" << outline->strokeCapType()
         << "join type:" << outline->strokeJoinType()
         << "color:" << outline->color().name()
         << "stroke width:" << outline->strokeWidth()
         << "pen thickness (%):" << outlineData.strokeThicknessPercentage()
         << "pen angle:" << outlineData.penAngle()
         << endData.toHex();

            mDocument->insertOutline( outlineData.index(), outline );
        }
    }

    mRiffStreamReader.closeList();
}

// used to collect base style links, because styles can be referenced before declared
struct StyleLink
{
    StyleLink()  {}
    StyleLink(CdrStyle* style, quint16 baseStyleIndex)
    : mStyle(style), mBaseStyleIndex(baseStyleIndex) {}
    CdrStyle* mStyle; quint16 mBaseStyleIndex;
};

void
CdrParser::readDocStyleTable()
{
    mRiffStreamReader.openList();
qDebug() << "Reading Styles...";

    QVector<StyleLink> styleLinkList;
    QHash<quint16,CdrStyle*> styleTable;

    while( mRiffStreamReader.readNextChunkHeader() )
    {
        if( mRiffStreamReader.chunkId() == stylId )
        {
            mRiffStreamReader.openList();

            while( mRiffStreamReader.readNextChunkHeader() )
            {
                if( mRiffStreamReader.chunkId() == stydId )
                {
                    CdrStyle* style = new CdrStyle;

                    const QByteArray styleChunk = mRiffStreamReader.chunkData();
                    const Cdr4StyleChunkData& styleData = dataRef<Cdr4StyleChunkData>( styleChunk );
                    styleTable.insert( styleData.styleIndex(), style );
                    if( styleData.baseStyleIndex() > 0 )
                        styleLinkList.append( StyleLink(style,styleData.baseStyleIndex()) );

                    const Cdr4ArgumentWithTypeListData& styleArgs = styleData.arguments();
        qDebug()<<"Style id:"<<styleData.styleIndex()<<"Base id:"<<styleData.baseStyleIndex()
                <<"d:"<<styleData._unknown1()<<styleData._unknown2()
                    <<styleData._unknown3()<<styleData._unknown4()
                <<"args:"<<styleArgs.count();

                    for (int i=0; i < styleArgs.count(); i++)
                    {
                        const quint16 argType = styleArgs.argType(i);
        QString argAsString;
        QString argTypeAsString;
        switch(argType)
        {
            case Cdr4StyleArgument205Id :
            case Cdr4StyleArgument210Id :
                argTypeAsString = QLatin1String("some 32-bit");
                argAsString = QString::number( styleArgs.arg<quint32>(i) );
                break;
            case Cdr4StyleArgumentTextAlignmentId :
            {
                static const struct { const char* name; CdrTextAlignment id;} alignData[5] =
                {
                    {" left?",   CdrTextAlignmentUnknown},
                    {" other left?",   CdrTextAlignmentUnknown},
                    {" center", CdrTextAlignCenter},
                    {" right",  CdrTextAlignRight},
                    {" block?",  CdrTextAlignmentUnknown}
                };

                const Cdr4StyleArgumentTextAlignmentData& data = styleArgs.argRef<Cdr4StyleArgumentTextAlignmentData>( i );
                const bool isAlignmentTypeKnown = ( /*(Cdr4StyleTextAlignment0Id <= data.type()) &&*/
                                                    (data.type() <= Cdr4TextAlignment4Id) );
                if( isAlignmentTypeKnown )
                    style->setTextAlignment( alignData[data.type()].id );

                argTypeAsString = QLatin1String("text alignment");
                argAsString = QString::number( data.type() );
                if( isAlignmentTypeKnown )
                    argAsString.append( QLatin1String(alignData[data.type()].name) );
                else
                    argAsString.append( QLatin1String(" UNKNOWN") );
                break;
            }
            case Cdr4StyleArgumentTitleId :
            {
                const QString title = QLatin1String( styleArgs.argRef<Cdr4StyleArgumentTitleData>(i).title() );
                style->setTitle( title );

                argTypeAsString = QLatin1String("title");
                argAsString = title;
                break;
            }
            case Cdr4StyleArgumentFontId:
            {
                const Cdr4StyleArgumentFontData& fontDataArgument =
                    styleArgs.argRef<Cdr4StyleArgumentFontData>( i );

                CdrFontData* fontData = new CdrFontData;
                fontData->setFontId( fontDataArgument.font().index() );
                fontData->setFontSize( fontDataArgument.font().size() );
                const CdrFontWeight fontWeight =
                    (fontDataArgument.font().style()==Cdr4StyleFontBoldId) ? CdrFontBold : CdrFontNormal;
                fontData->setFontWeight( fontWeight );
                style->setFontData( fontData );

                argTypeAsString = QLatin1String("font");
                argAsString = QLatin1String("id:") + QString::number( fontData->fontId() ) +
                              QLatin1String(" size:") + QString::number( fontData->fontSize()) +
                              QLatin1String(" style:") + QString::number( fontDataArgument.font().style());
                break;
            }
            case Cdr4StyleArgumentBulletSymbolId:
            {
                const Cdr4StyleArgumentBulletSymbolData& data =
                    styleArgs.argRef<Cdr4StyleArgumentBulletSymbolData>( i );
                const QByteArray hex = QByteArray::fromRawData(data._unknownPtr(), data._unknownCount()).toHex();
                argTypeAsString = QLatin1String("bullet symbol");
                argAsString = QString::number( data.length() ) +
                              QLatin1String(" font id:") + QString::number( data.font().index() ) +
                              QLatin1String(" size:") + QString::number( data.font().size()) +
                              QLatin1String(" style:") + QString::number( data.font().style()) + QLatin1Char(' ') +
                              QString::number( data._unknown4() ) + QLatin1Char(' ') +
                              QString::fromLatin1(hex.constData(), hex.count());
                break;
            }
            case Cdr4StyleArgumentTabStopsId:
            {
                const Cdr4StyleArgumentTabStopsData& data = styleArgs.argRef<Cdr4StyleArgumentTabStopsData>( i );
                argTypeAsString = QLatin1String("some max 258 bytes");
                argAsString = QLatin1String("count:") + QString::number(data.count());
                if( data.count() > 0 )
                    argAsString = argAsString + QLatin1String(", from ") +
                                  QString::number(data.tabStop(0).offset()) +
                                  QLatin1String(" to ") +
                                  QString::number(data.tabStop(data.count()-1).offset());
                break;
            }
            case Cdr4StyleArgument235Id:
            {
                const Cdr4StyleArgument235Data& data = styleArgs.argRef<Cdr4StyleArgument235Data>( i );

                argTypeAsString = QLatin1String("some 12 bytes");
                argAsString = QString::number( data._unknown0() ) + QLatin1Char(' ') +
                            QString::number( data._unknown1() ) + QLatin1Char(' ') +
                            QString::number( data._unknown2() ) + QLatin1Char(' ') +
                            QString::number( data._unknown3() ) + QLatin1Char(' ') +
                            QString::number( data._unknown4() ) + QLatin1Char(' ') +
                            QString::number( data._unknown5() );
                break;
            }
            case Cdr4StyleArgumentTextMarginsId:
            {
                const Cdr4StyleArgumentTextMarginsData& data =
                    styleArgs.argRef<Cdr4StyleArgumentTextMarginsData>( i );

                CdrTextMargins* textMargins = new CdrTextMargins;
                textMargins->setLeftMargin( data.leftMargin() );
                textMargins->setTopMargin( data.topMargin() );
                textMargins->setBottomMargin( data.bottomMargin() );
                textMargins->setRightMargin( data.rightMargin() );

                argTypeAsString = QLatin1String("margins?");
                argAsString = QLatin1String("left:") + QString::number( data.leftMargin() ) +
                              QLatin1String(" top:") + QString::number( data.topMargin() ) +
                              QLatin1String(" bottom:") + QString::number( data.bottomMargin() ) +
                              QLatin1String(" right:") + QString::number( data.rightMargin() );
                break;
            }
            case Cdr4StyleArgument250Id :
            {
                const Cdr4StyleArgument250Data& data = styleArgs.argRef<Cdr4StyleArgument250Data>( i );

                argTypeAsString = QLatin1String("two 16-bit");
                argAsString = QString::number( data._unknown0() ) + QLatin1Char(' ') +
                            QString::number( data._unknown1() );
                break;
            }
            default:
                argTypeAsString = QLatin1String("UNKNOWN!");
                break;
        }
        qDebug() << i << ": type" << argType << argTypeAsString << argAsString;
                    }
                    mDocument->insertStyle( styleData.styleIndex(), style );
                }
            }

            mRiffStreamReader.closeList();
        }
    }

    // set base style links
    foreach( StyleLink link, styleLinkList )
    {
        CdrStyle* baseStyle = styleTable.value(link.mBaseStyleIndex);
        if( baseStyle == 0 )
        {
qDebug() << "base style NOT FOUND:" << link.mBaseStyleIndex;
            continue;
        }
        link.mStyle->setBaseStyle(baseStyle);
    }

    mRiffStreamReader.closeList();
}


void
CdrParser::readDocBlockTextTable()
{
    mRiffStreamReader.openList();
qDebug() << "Reading Btx Table...";
    while( mRiffStreamReader.readNextChunkHeader() )
    {
        if( mRiffStreamReader.chunkId() == strlId )
        {
            readStrList();
        }
    }

    mRiffStreamReader.closeList();
}


void
CdrParser::readStrList()
{
    CdrBlockText* blockText = new CdrBlockText;
    quint16 blockTextId;

    mRiffStreamReader.openList();
qDebug() << "Reading Strl...";
    while( mRiffStreamReader.readNextChunkHeader() )
    {
        const Koralle::FourCharCode chunkId = mRiffStreamReader.chunkId();

        if( chunkId == btidId )
        {
            const QByteArray btidChunk = mRiffStreamReader.chunkData();
            const Cdr4BlockTextIdChunkData btidData = data<Cdr4BlockTextIdChunkData>( btidChunk );
            blockTextId = btidData.id();
qDebug()<<"BlockText id:"<<blockTextId;
        }
        else if( chunkId == parlId )
        {
            CdrParagraph* paragraph = readParagraphList();
            if( paragraph )
                blockText->addParagraph( paragraph );
        }
    }

    mRiffStreamReader.closeList();

    mDocument->insertBlockText( blockTextId, blockText );
}

CdrParagraph*
CdrParser::readParagraphList()
{
    CdrParagraph* paragraph = new CdrParagraph;

    CdrParagraphLine* currentParagraphLine = new CdrParagraphLine;

    mRiffStreamReader.openList();
qDebug() << "Reading Parl...";

    while( mRiffStreamReader.readNextChunkHeader() )
    {
        const Koralle::FourCharCode chunkId = mRiffStreamReader.chunkId();

        if( chunkId == paraId )
        {
            const QByteArray paraChunk = mRiffStreamReader.chunkData();
            const Cdr4BlockTextParagraphChunkData& paraData = dataRef<Cdr4BlockTextParagraphChunkData>( paraChunk );

            paragraph->setStyleId( paraData.styleIndex() );
            qDebug() << "...para: style:"<<paraData.styleIndex() <<"length:"<<paraData.length()<<paraData._unknown2();
        }
        else if( chunkId == bnchId )
        {
            const QByteArray bnchChunk = mRiffStreamReader.chunkData();
            const Cdr4BlockTextNormalCharChunkData& bnchData =
                dataRef<Cdr4BlockTextNormalCharChunkData>( bnchChunk );

            CdrNormalTextSpan* textSpan = new CdrNormalTextSpan;
            QString text;
            const int count = bnchChunk.count() / sizeof(Cdr4BlockTextChar);
            for( int i = 0; i<count; ++i )
            {
                const unsigned char textChar = bnchData.textChar(i).character();
                if( (textChar >= ' ') || (textChar==0x09)  )
                    text.append( QChar(textChar) );
                else
                    qDebug() << "CONTROL CHAR found:" << textChar;
            }
            textSpan->setText( text );
            currentParagraphLine->addTextSpan( textSpan );

            qDebug() << "...bnch:"<<text;
        }
        else if( chunkId == bschId )
        {
            const QByteArray bschChunk = mRiffStreamReader.chunkData();
            const Cdr4BlockTextSpecialCharChunkData& bschData =
                dataRef<Cdr4BlockTextSpecialCharChunkData>( bschChunk );

            const unsigned char textChar = bschData.character().character();

            // is linebreak?
            if( textChar == 0x0D )
            {
                currentParagraphLine->setNextLineOffset( CdrPoint(bschData.nextLineOffset().x(),
                                                                  bschData.nextLineOffset().y()) );
                paragraph->addParagraphLine( currentParagraphLine );
                currentParagraphLine = new CdrParagraphLine;
            }
            else
            {
                CdrStyledTextSpan* textSpan = new CdrStyledTextSpan;
                textSpan->setText( QChar(textChar) );
                textSpan->setFontId( bschData.font().index() );
                textSpan->setFontSize( bschData.font().size() );
                const CdrFontWeight fontWeight =
                    (bschData.font().style()==Cdr4StyleFontBoldId) ? CdrFontBold : CdrFontNormal;
                textSpan->setFontWeight( fontWeight );
                currentParagraphLine->addTextSpan( textSpan );
            }

            qDebug() << "...bsch:"<<((textChar >= ' ')?QString(QChar(textChar)):QString::number(textChar,16))
                     << "font index:" << bschData.font().index()
                     << "font size:" << bschData.font().size()
                     << "font style:" << bschData.font().style()
                     << "next line:"<< bschData.nextLineOffset().x() << bschData.nextLineOffset().y()
                     << "d:" << bschData._unknown6() << bschData._unknown7() << bschData._unknown8();
        }
    }
    mRiffStreamReader.closeList();

    if( currentParagraphLine->textSpans().isEmpty() )
        delete currentParagraphLine;
    else
        paragraph->addParagraphLine( currentParagraphLine );

    return paragraph;
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

        if( chunkId == flgsId )
        {
            const QByteArray flagsChunk = mRiffStreamReader.chunkData();
//             const CdrPageFlagChunkData flags = data<CdrPageFlagChunkData>( flagsChunk );
qDebug()<< "...with flags"<<flagsChunk.toHex();
        }
        else if( chunkId == gobjId )
        {
            mRiffStreamReader.openList();

            while( mRiffStreamReader.readNextChunkHeader() )
            {
                if( mRiffStreamReader.chunkId() == layrId )
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


CdrLayer*
CdrParser::readLayer()
{
    CdrLayer* layer = new CdrLayer();

    mRiffStreamReader.openList();

qDebug() << "Layer <<<";
    while( mRiffStreamReader.readNextChunkHeader() )
    {
        const Koralle::FourCharCode chunkId = mRiffStreamReader.chunkId();

        if( chunkId == flgsId )
        {
            const QByteArray flagsChunk = mRiffStreamReader.chunkData();
//             const CdrLayerFlagChunkData flags = data<CdrLayerFlagChunkData>( flagsChunk );
qDebug()<< "...with flags"<<flagsChunk.toHex();
        }
        else if( chunkId == lgobId )
        {
            readLayerLGOb();
        }
        else if( chunkId == lnkgId )
        {
            CdrLinkGroupObject* group = readLinkGroupObject();
            if( group )
                layer->addObject( group );
        }
        else if( chunkId == grp_Id )
        {
            CdrGroupObject* group = readGroupObject();
            if( group )
                layer->addObject( group );
        }
        else if( chunkId == obj_Id )
        {
            CdrAbstractObject* object = readObject();
            if( object )
                layer->addObject( object );
        }
    }

qDebug() << "Layer >>>";
    mRiffStreamReader.closeList();

    return layer;
}

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

        if( chunkId == spndId )
        {
            const QByteArray spndChunk = mRiffStreamReader.chunkData();
            const Cdr4LinkGroupSpndChunkData spndData = data<Cdr4LinkGroupSpndChunkData>( spndChunk );
            group->setObjectId( spndData.id() );

qDebug()<< "...with spnd"<<group->objectId();
        }
        else if( chunkId == flgsId )
        {
            const QByteArray flagsChunk = mRiffStreamReader.chunkData();
//             const CdrLinkGroupFlagChunkData flagsData = data<CdrLinkGroupFlagChunkData>( flagsChunk );
qDebug()<< "...with flags"<<flagsChunk.toHex();
        }
        else if( chunkId == obj_Id )
        {
            CdrAbstractObject* object = readObject();
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

        if( chunkId == spndId )
        {
            const QByteArray spndChunk = mRiffStreamReader.chunkData();
            const Cdr4GroupSpndChunkData spndData = data<Cdr4GroupSpndChunkData>( spndChunk );
            group->setObjectId( spndData.id() );

qDebug()<< "...with spnd"<<group->objectId();
        }
        else if( chunkId == flgsId )
        {
            const QByteArray flagsChunk = mRiffStreamReader.chunkData();
//             const CdrGroupFlagChunkData flagsData = data<CdrGroupFlagChunkData>( flagsChunk );
qDebug()<< "...with flags"<<flagsChunk.toHex();
        }
        else if( chunkId == lgobId )
        {
            readGroupObjectLGOb( group );
        }
        else if( chunkId == bboxId )
        {
//             const QByteArray bboxChunk = mRiffStreamReader.chunkData();
//             const Cdr4BoundingBoxChunkData& boundingBoxData = dataRef<Cdr4BoundingBoxChunkData>( bboxChunk );
        }
        else if( chunkId == grp_Id )
        {
            CdrGroupObject* object = readGroupObject();
            if( object )
                group->addObject( object );
        }
        else if( chunkId == obj_Id )
        {
            CdrAbstractObject* object = readObject();
            if( object )
                group->addObject( object );
        }
    }

qDebug() << "Group >>>...";
    mRiffStreamReader.closeList();
    return group;
}

void
CdrParser::readGroupObjectLGOb( CdrGroupObject* group )
{
    QVector<CdrAbstractTransformation*> transformations;
    mRiffStreamReader.openList();
qDebug() << "LGOb <<<";

    while( mRiffStreamReader.readNextChunkHeader() )
    {
        const Koralle::FourCharCode chunkId = mRiffStreamReader.chunkId();

        if( chunkId == lodaId )
        {
            readLoda();
        }
        else if( chunkId == trflId )
        {
            transformations = readTrfl();
        }
    }
qDebug() << "LGOb >>>";
    mRiffStreamReader.closeList();
    if( group )
        group->setTransformations( transformations );
}

CdrAbstractObject*
CdrParser::readObject()
{
    CdrAbstractObject* object = 0;
    CdrObjectId objectId = cdrObjectInvalidId;

    mRiffStreamReader.openList();
qDebug() << "Object <<<";

    while( mRiffStreamReader.readNextChunkHeader() )
    {
        const Koralle::FourCharCode chunkId = mRiffStreamReader.chunkId();

        if( chunkId == spndId )
        {
            const QByteArray spndChunk = mRiffStreamReader.chunkData();
            const Cdr4ObjectSpndChunkData spndData = data<Cdr4ObjectSpndChunkData>( spndChunk );
            objectId = spndData.id();
qDebug()<< "...with spnd"<<objectId;
        }
        else if( chunkId == flgsId )
        {
            const QByteArray flagsChunk = mRiffStreamReader.chunkData();
//             const CdrObjectFlagChunkData flagsData = data<CdrObjectFlagChunkData>( flagsChunk );
qDebug()<< "...with flags"<<flagsChunk.toHex();
        }
        else if( chunkId == lgobId )
        {
            object = readObjectLGOb();
        }
    }

qDebug() << "Object >>>...";
    mRiffStreamReader.closeList();

    if( object )
    {
        object->setObjectId( objectId );
    }

    return object;
}


CdrAbstractObject*
CdrParser::readObjectLGOb()
{
    CdrAbstractObject* object = 0;
    QVector<CdrAbstractTransformation*> transformations;
    mRiffStreamReader.openList();
qDebug() << "LGOb <<<";

    while( mRiffStreamReader.readNextChunkHeader() )
    {
        const Koralle::FourCharCode chunkId = mRiffStreamReader.chunkId();

        if( chunkId == lodaId )
        {
            object = readLoda();
        }
        else if( chunkId == trflId )
        {
            transformations = readTrfl();
        }
    }
qDebug() << "LGOb >>>";
    mRiffStreamReader.closeList();
    if( object )
        object->setTransformations( transformations );
    return object;
}

static inline
double
cdrTrafoFactor( Cdr4SomeStrangeFloat strangeFloat )
{
    return static_cast<double>(strangeFloat.integralPart()) + static_cast<double>(strangeFloat.fractionPart()) / 65535;
}

QVector<CdrAbstractTransformation*>
CdrParser::readTrfl()
{
    QVector<CdrAbstractTransformation*> result;

    mRiffStreamReader.openList();

    while( mRiffStreamReader.readNextChunkHeader() )
    {
        if( (mRiffStreamReader.chunkId() == trfdId) )
        {
            const QByteArray trfdChunk = mRiffStreamReader.chunkData();
            const Cdr4TransformDataChunkData& trfdData = dataRef<Cdr4TransformDataChunkData>( trfdChunk );

qDebug() << "Reading Trfd" << trfdData.arguments().count() << "args" << trfdData._unknown0() <<trfdData._unknown1();
            for (int i=0; i < trfdData.arguments().count(); i++)
            {
                const Cdr4TransformData& transformData = trfdData.arguments().argRef<Cdr4TransformData>(i);
                const int dataSize = ((const char*)&trfdData.arguments().argRef<Cdr4TransformData>(i+1)-(const char*)(&transformData));
//         qDebug() << i << ": type" << transformData.index()<< dataSize;
                if( transformData.index() == Cdr4TransformMatrixId )
                {
                    const Cdr4TransformMatrixData& data8 = transformData.matrixData();
                    CdrNormalTransformation* transformation = new CdrNormalTransformation;
                    transformation->setData(
                        cdrTrafoFactor(data8.a()),
                        cdrTrafoFactor(data8.c()),
                        data8.e(),
                        cdrTrafoFactor(data8.b()),
                        cdrTrafoFactor(data8.d()),
                        data8.f() );
                    result.append( transformation );

//         qDebug() << "e:" << transformation->e() << "f:" << transformation->f()
//                  << "a" << transformation->a() << "c" << transformation->c()
//                  << "b" << transformation->b() << "d" << transformation->d();
                }
                else
// qDebug() << QByteArray::fromRawData((const char*)(&transformData), dataSize).toHex();
        ;
            }
        }
    }

    mRiffStreamReader.closeList();

    return result;
}


CdrAbstractObject*
CdrParser::readLoda()
{
    CdrAbstractObject* object = 0;

    const QByteArray lodaChunk = mRiffStreamReader.chunkData();

    const Cdr4ObjectArgumentsData& argsData = dataRef<Cdr4ObjectArgumentsData>( lodaChunk );
    const Cdr4ArgumentWithTypeListData& arguments = argsData.arguments();

    const QString lodaTypeName = QLatin1String(
        (argsData.chunkType() == Cdr4ObjectLayerId) ?      "Layer" :
        (argsData.chunkType() == Cdr4ObjectPixelImageId) ? "PixelImage" :
        (argsData.chunkType() == Cdr4ObjectGridId) ?       "Grid" :
        (argsData.chunkType() == Cdr4ObjectGuidesId) ?     "Guides" :
        (argsData.chunkType() == Cdr4ObjectDesktopId) ?    "Desktop" :
        (argsData.chunkType() == Cdr4ObjectRectangleId) ?  "Rectangle" :
        (argsData.chunkType() == Cdr4ObjectEllipseId) ?    "Ellipse" :
        (argsData.chunkType() == Cdr4ObjectPathId) ?       "Path" :
        (argsData.chunkType() == Cdr4ObjectGraphicTextId) ?"Graphic Text" :
        (argsData.chunkType() == Cdr4ObjectBlockTextId) ?  "Block Text" :
                                                           "" );
qDebug() << "Reading Loda" << arguments.count()
         << "args, loda type" << argsData.chunkType() << lodaTypeName;
    object =
        (argsData.chunkType() == Cdr4ObjectRectangleId) ?   (CdrAbstractObject*)readRectangleObject( arguments ) :
        (argsData.chunkType() == Cdr4ObjectEllipseId) ?     (CdrAbstractObject*)readEllipseObject( arguments ) :
        (argsData.chunkType() == Cdr4ObjectPathId) ?        (CdrAbstractObject*)readPathObject( arguments ) :
        (argsData.chunkType() == Cdr4ObjectGraphicTextId) ? (CdrAbstractObject*)readGraphicTextObject( arguments ) :
        (argsData.chunkType() == Cdr4ObjectBlockTextId) ?   (CdrAbstractObject*)readBlockTextObject( arguments ) :
                                                            (CdrAbstractObject*)0;

    for (int i=0; i < arguments.count(); i++)
    {
const quint16 argType = arguments.argType(i);
QString argAsString;
QString argTypeAsString;
switch(argType)
{
    case Cdr4ObjectArgumentOutlineIndexId :
        argAsString = QString::number( arguments.arg<Cdr4OutlineIndex>(i) );
        argTypeAsString = QLatin1String("outline index");
        break;
    case Cdr4ObjectArgumentFillIndexId :
        argAsString = QString::number( arguments.arg<Cdr4FillIndex>(i) );
        argTypeAsString = QLatin1String("fill index");
        break;
    case Cdr4ObjectArgumentSpecificDataId :
        argTypeAsString = QLatin1String("object specific data");
        break;
    case Cdr4ObjectArgument40Id :
    {
        const Cdr4ObjectArgument40Data data = arguments.arg<Cdr4ObjectArgument40Data>( i );
        argAsString = QString::number( data._unknown0() )+QLatin1Char(',')+QString::number(data._unknown1());
        argTypeAsString = QLatin1String("some 32-bit");
        break;
    }
    case Cdr4ObjectArgument100Id :
    {
        const Cdr4ObjectArgument100Data data = arguments.arg<Cdr4ObjectArgument100Data>( i );
        argAsString = QString::number(data._unknown0())+QLatin1Char(',')+QString::number(data._unknown1());
        argTypeAsString = QLatin1String("z-index?");
        break;
    }
    case Cdr4ObjectArgumentStyleIndexId :
        argAsString = QString::number( arguments.arg<Cdr4StyleIndex>(i) );
        argTypeAsString = QLatin1String("style index");
        break;
    case Cdr4ObjectArgumentTitleId :
        argAsString = QLatin1String( arguments.argRef<Cdr4ObjectArgumentTitleData>(i).title() );
        argTypeAsString = QLatin1String("title");
        break;
    case Cdr4ObjectArgument1010Id :
        argAsString = QString::number( arguments.arg<Cdr4ObjectArgument1010Data>(i)._unknown0(), 16 );
        argTypeAsString = QLatin1String("some 16-bit");
        break;
    case Cdr4ObjectArgument2000Id :
    {
        const Cdr4ObjectArgument2000Data& data = arguments.argRef<Cdr4ObjectArgument2000Data>( i );
        argAsString =
            QString::number(data._unknown0())+QLatin1Char(',')+QString::number(data._unknown1())+QLatin1Char(',') +
            QString::number(data._unknown2())+QLatin1Char(',')+QString::number(data._unknown3())+QLatin1Char(',') +
            QString::number(data._unknown4())+QLatin1Char(',')+QString::number(data._unknown5());
        argTypeAsString = QLatin1String("some 12 bytes");
        break;
    }
    case Cdr4ObjectArgument3000Id :
    {
        const Cdr4ObjectArgument3000Data& data = arguments.argRef<Cdr4ObjectArgument3000Data>( i );
        for(unsigned int i=0; i<sizeof(Cdr4ObjectArgument3000Data); ++i )
            argAsString = argAsString + QLatin1Char(' ')+QString::number(data._unknown0(i));
        argTypeAsString = QLatin1String("some 24 bytes");
        break;
    }
    case Cdr4ObjectArgument3010Id :
    {
        argTypeAsString = QLatin1String("text aligned on path data");
        break;
    }
    case Cdr4ObjectArgument6000Id :
    {
        const Cdr4ObjectArgument6000Data& data = arguments.argRef<Cdr4ObjectArgument6000Data>( i );
        argAsString =
            QString::number(data._unknown0())+QLatin1Char(',')+QString::number(data._unknown1())+QLatin1Char(',') +
            QString::number(data._unknown2())+QLatin1Char(',')+QString::number(data._unknown3());
        argTypeAsString = QLatin1String("some 8 bytes");
        break;
    }
    default:
        argTypeAsString = QLatin1String("UNKNOWN!");
        break;
}
qDebug() << i << ": type" << argType << argTypeAsString << argAsString;

    }

    return object;
}


CdrRectangleObject*
CdrParser::readRectangleObject( const Cdr4ArgumentWithTypeListData& argsData )
{
    CdrRectangleObject* rectangleObject = new CdrRectangleObject;

    for (int i=0; i < argsData.count(); i++)
    {
        const quint16 argType = argsData.argType(i);

        switch( argType )
        {
        case Cdr4ObjectArgumentOutlineIndexId :
            rectangleObject->setOutlineId( argsData.arg<Cdr4OutlineIndex>(i) );
            break;
        case Cdr4ObjectArgumentFillIndexId :
            rectangleObject->setFillId( argsData.arg<Cdr4FillIndex>(i) );
            break;
        case Cdr4ObjectArgumentSpecificDataId :
        {
            const Cdr4RectangleData& rectangleData = argsData.argRef<Cdr4RectangleData>( i );
            rectangleObject->setCornerPoint( CdrPoint(rectangleData.cornerPoint().x(),rectangleData.cornerPoint().y()) );
qDebug() << "rectangle: corner point" << rectangleObject->cornerPoint().x()<<","<<rectangleObject->cornerPoint().y()
                 << "unknown" << rectangleData._unknown();
        }
        case Cdr4ObjectArgumentStyleIndexId :
            rectangleObject->setStyleId( argsData.arg<Cdr4StyleIndex>(i) );
            break;
        }
    }

    return rectangleObject;
}

CdrEllipseObject*
CdrParser::readEllipseObject( const Cdr4ArgumentWithTypeListData& argsData )
{
    CdrEllipseObject* ellipseObject = new CdrEllipseObject;

    for (int i=0; i < argsData.count(); i++)
    {
        const quint16 argType = argsData.argType(i);

        switch( argType )
        {
        case Cdr4ObjectArgumentOutlineIndexId :
            ellipseObject->setOutlineId( argsData.arg<Cdr4OutlineIndex>(i) );
            break;
        case Cdr4ObjectArgumentFillIndexId :
            ellipseObject->setFillId( argsData.arg<Cdr4FillIndex>(i) );
            break;
        case Cdr4ObjectArgumentSpecificDataId :
        {
            const Cdr4EllipseData& ellipseData = argsData.argRef<Cdr4EllipseData>( i );
            ellipseObject->setCornerPoint( CdrPoint(ellipseData.cornerPoint().x(),ellipseData.cornerPoint().y()) );
            ellipseObject->setStartAngle(ellipseData.startAngle());
            ellipseObject->setEndAngle(ellipseData.endAngle());
qDebug() << "ellipse: corner"<<ellipseObject->cornerPoint().x()<<","<<ellipseObject->cornerPoint().y()
                     <<"start angle"<<ellipseObject->startAngle()<<"end angle"<<ellipseObject->endAngle()
                     <<"unknown"<<ellipseData._unknown();
        }
            break;
        case Cdr4ObjectArgumentStyleIndexId :
            ellipseObject->setStyleId( argsData.arg<Cdr4StyleIndex>(i) );
            break;
        }
    }

    return ellipseObject;
}

CdrPathObject*
CdrParser::readPathObject( const Cdr4ArgumentWithTypeListData& argsData )
{
    CdrPathObject* pathObject = new CdrPathObject();

    for (int i=0; i < argsData.count(); i++)
    {
        const quint16 argType = argsData.argType(i);

        switch( argType )
        {
        case Cdr4ObjectArgumentOutlineIndexId :
            pathObject->setOutlineId( argsData.arg<Cdr4OutlineIndex>(i) );
            break;
        case Cdr4ObjectArgumentFillIndexId :
            pathObject->setFillId( argsData.arg<Cdr4FillIndex>(i) );
            break;
        case Cdr4ObjectArgumentSpecificDataId :
        {
            const Cdr4PointList& points = argsData.argRef<Cdr4PointList>( i );
            int curveControlPointCount = 0;
qDebug() << "path points:" << points.count();
            for (unsigned int j=0; j<points.count(); j++)
            {
                const CdrPointType pointType = points.pointType(j);
                // check validness
                bool isBadPoint = false;
                if( pointType == 0xC0 )
                {
                    ++curveControlPointCount;
                    isBadPoint = ( curveControlPointCount > 2 );
                }
                else
                {
                    isBadPoint = ( curveControlPointCount == 1 );
                    curveControlPointCount = 0;
                }
                const Cdr4Point point = points.point(j);
// qDebug() <<"    "<< j<<":"<<point.x()<<","<<point.y()<< QString::number(pointType,16);
                pathObject->addPathPoint( CdrPathPoint(CdrPoint(point.x(),point.y()), pointType) );
                if( isBadPoint )
                {
qDebug() << "Assumption about path data not met, not creating path object.";
                    delete pathObject;
                    return 0;
                }
            }
        }
            break;
        case Cdr4ObjectArgumentStyleIndexId :
            pathObject->setStyleId( argsData.arg<Cdr4StyleIndex>(i) );
            break;
        }
    }

    return pathObject;
}

CdrGraphicTextObject*
CdrParser::readGraphicTextObject( const Cdr4ArgumentWithTypeListData& argsData )
{
    CdrGraphicTextObject* textObject = new CdrGraphicTextObject;

    for (int i=0; i < argsData.count(); i++)
    {
        const quint16 argType = argsData.argType(i);

        switch( argType )
        {
        case Cdr4ObjectArgumentOutlineIndexId :
            textObject->setOutlineId( argsData.arg<Cdr4OutlineIndex>(i) );
            break;
        case Cdr4ObjectArgumentFillIndexId :
            textObject->setFillId( argsData.arg<Cdr4FillIndex>(i) );
            break;
        case Cdr4ObjectArgumentSpecificDataId :
        {
            const Cdr4GraphicTextData& textData = argsData.argRef<Cdr4GraphicTextData>( i );
            QString text;
            const char* rawCharData = textData.charData();
            for (unsigned int j=0; j<textData.length(); j++)
            {
                const Cdr4CharData* charData = reinterpret_cast<const Cdr4CharData*>(rawCharData);
                text.append( QLatin1Char(charData->character()) );
                switch( charData->type() )
                {
                default:
                    qDebug() << "UNKNOWN CHARDATA type" << charData->type();
                    break;
                case 0: rawCharData += sizeof(Cdr4CharData); break;
                case 1: rawCharData += sizeof(Cdr4CharData)+sizeof(Cdr4CharAdditionalData); break;
                }
            }
            textObject->setText( text );
qDebug() << "text:" << text << "unknown:"<<textData._unknown();
        }
            break;
        case Cdr4ObjectArgumentStyleIndexId :
            textObject->setStyleId( argsData.arg<Cdr4StyleIndex>(i) );
            break;
        }
    }

    return textObject;
}

CdrBlockTextObject*
CdrParser::readBlockTextObject( const Cdr4ArgumentWithTypeListData& argsData )
{
    CdrBlockTextObject* blockTextObject = new CdrBlockTextObject;

    for (int i=0; i < argsData.count(); i++)
    {
        const quint16 argType = argsData.argType(i);

        switch( argType )
        {
        case Cdr4ObjectArgumentOutlineIndexId :
            blockTextObject->setOutlineId( argsData.arg<Cdr4OutlineIndex>(i) );
            break;
        case Cdr4ObjectArgumentFillIndexId :
            blockTextObject->setFillId( argsData.arg<Cdr4FillIndex>(i) );
            break;
        case Cdr4ObjectArgumentSpecificDataId :
        {
            const Cdr4BlockTextData& blockTextData = argsData.argRef<Cdr4BlockTextData>( i );
            blockTextObject->setWidth( blockTextData.width() );
            blockTextObject->setHeight( blockTextData.height() );
qDebug() << "blockTextData:" << blockTextData._unknown0() << blockTextData.width()
                             << blockTextData.height() << blockTextData.index()
                             << blockTextData._unknown4() << blockTextData._unknown5();
        }
            break;
        case Cdr4ObjectArgumentStyleIndexId :
            blockTextObject->setStyleId( argsData.arg<Cdr4StyleIndex>(i) );
            break;
        }
    }

    return blockTextObject;
}
