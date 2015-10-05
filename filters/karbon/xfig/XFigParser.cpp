/*
    This file is part of the Calligra project, made within the KDE community.

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
    Boston, MA 02110-1301, USA.
*/

#include "XFigParser.h"

// filter
#include "XFigDocument.h"
// Qt
#include <QTextStream>
#include <QTextCodec>
#include <QIODevice>
#include <QFont>
#include <QScopedPointer>

#include <QDebug>


enum XFig3_2TextFlag {
    XFig3_2TextNotScaled =  1 << 0,
    XFig3_2TextForLaTeX =   1 << 1,
    XFig3_2TextPostScriptFont = 1 << 2,
    XFig3_2TextHidden =     1 << 3
};

enum XFig3_2ObjectCode
{
    XFig3_2CompoundObjectEndId = -6,
    XFig3_2ColorObjectId = 0,
    XFig3_2EllipseObjectId = 1,
    XFig3_2PolylineObjectId = 2,
    XFig3_2SplineObjectId = 3,
    XFig3_2TextObjectId = 4,
    XFig3_2ArcObjectId = 5,
    XFig3_2CompoundObjectId = 6
};

enum XFig3_2ArcSubTypeCode
{
    XFig3_2ArcOpenEndedId = 1,
    XFig3_2ArcPieWedgeId= 2
};

enum XFig3_2PolylineSubTypeCode
{
    XFig3_2PolylinePolylineId = 1,
    XFig3_2PolylineBoxId = 2,
    XFig3_2PolylinePolygonId = 3,
    XFig3_2PolylineArcBoxId = 4,
    XFig3_2PolylinePictureBoundingBoxId = 5
};

enum XFig3_2SplineSubtypeCode
{
    XFig3_2SplineOpenApproximatedId  = 0,
    XFig3_2SplineClosedApproximatedId  = 1,
    XFig3_2SplineOpenInterpolatedId  = 2,
    XFig3_2SplineClosedInterpolatedId  = 3,
    XFig3_2SplineOpenXId  = 4,
    XFig3_2SplineClosedXId  = 5
};

enum XFig3_2TextAlignment {
    XFig3_2TextLeftAligned = 0,
    XFig3_2TextCenterAligned = 1,
    XFig3_2TextRightAligned = 2
};

static const int arrowHeadTypeMapSize = 15;
static const
struct { XFigArrowHeadType style[2]; }
arrowHeadTypeMap[arrowHeadTypeMapSize] =
{
    { {XFigArrowHeadStick,                     XFigArrowHeadStick} },
    { {XFigArrowHeadHollowTriangle,            XFigArrowHeadFilledTriangle} },
    { {XFigArrowHeadHollowConcaveSpear,        XFigArrowHeadFilledConcaveSpear} },
    { {XFigArrowHeadHollowConvexSpear,         XFigArrowHeadFilledConvexSpear} },
    { {XFigArrowHeadHollowDiamond,             XFigArrowHeadFilledDiamond} },
    { {XFigArrowHeadHollowCircle,              XFigArrowHeadFilledCircle} },
    { {XFigArrowHeadHollowHalfCircle,          XFigArrowHeadFilledHalfCircle} },
    { {XFigArrowHeadHollowSquare,              XFigArrowHeadFilledSquare} },
    { {XFigArrowHeadHollowReverseTriangle,     XFigArrowHeadFilledReverseTriangle} },
    { {XFigArrowHeadTopHalfFilledConcaveSpear, XFigArrowHeadBottomHalfFilledConcaveSpear} },
    { {XFigArrowHeadHollowTopHalfTriangle,     XFigArrowHeadFilledTopHalfTriangle} },
    { {XFigArrowHeadHollowTopHalfConcaveSpear, XFigArrowHeadFilledTopHalfConcaveSpear} },
    { {XFigArrowHeadHollowTopHalfConvexSpear,  XFigArrowHeadFilledTopHalfConvexSpear} },
    { {XFigArrowHeadWye,                       XFigArrowHeadBar} },
    { {XFigArrowHeadTwoProngFork,              XFigArrowHeadReverseTwoProngFork} }
};

static inline
XFigArrowHeadType
arrowHeadType( int type3_2, int style3_2 )
{
    XFigArrowHeadType result = XFigArrowHeadStick;
    if ((0<=type3_2) && (type3_2<arrowHeadTypeMapSize) &&
        ((style3_2 == 0) || (style3_2 == 1))) {
        result = arrowHeadTypeMap[type3_2].style[style3_2];
    }
    return result;
}

enum XFig3_2LineType
{
    XFig3_2LineDefault = -1,
    XFig3_2LineSolid = 0,
    XFig3_2LineDashed = 1,
    XFig3_2LineDotted = 2,
    XFig3_2LineDashDotted = 3,
    XFig3_2LineDashDoubleDotted = 4,
    XFig3_2LineDashTrippleDotted = 5
};

static const
struct { int type3_2; XFigLineType type; }
lineTypeMap[] =
{
    { XFig3_2LineDefault, XFigLineDefault },
    { XFig3_2LineSolid, XFigLineSolid },
    { XFig3_2LineDashed,XFigLineDashed  },
    { XFig3_2LineDotted, XFigLineDotted },
    { XFig3_2LineDashDotted, XFigLineDashDotted },
    { XFig3_2LineDashDoubleDotted,XFigLineDashDoubleDotted  },
    { XFig3_2LineDashTrippleDotted, XFigLineDashTrippleDotted }
};
static const int lineTypeMapSize = sizeof(lineTypeMap)/sizeof(lineTypeMap[0]);

static inline
XFigLineType
lineType( int type3_2 )
{
    XFigLineType result = XFigLineDefault;
    for (int i = 0; i<lineTypeMapSize; ++i) {
        if (lineTypeMap[i].type3_2 == type3_2) {
            result = lineTypeMap[i].type;
            break;
        }
    }
    return result;
}

enum XFig3_2CapType
{
    XFig3_2CapButt = 0,
    XFig3_2CapRound = 1,
    XFig3_2CapProjecting = 2,
};

static const
struct { int type3_2; XFigCapType type; }
capTypeMap[] =
{
    {XFig3_2CapButt, XFigCapButt },
    {XFig3_2CapRound, XFigCapRound},
    {XFig3_2CapProjecting,XFigCapProjecting}
};
static const int capTypeMapSize = sizeof(capTypeMap)/sizeof(capTypeMap[0]);

static inline
XFigCapType
capType( int type3_2 )
{
    XFigCapType result = XFigCapButt;
    for (int i = 0; i<capTypeMapSize; ++i) {
        if (capTypeMap[i].type3_2 == type3_2) {
            result = capTypeMap[i].type;
            break;
        }
    }
    return result;
}

enum XFig3_2JoinType
{
    XFig3_2JoinMiter = 0,
    XFig3_2JoinRound = 1,
    XFig3_2JoinBevel = 2,
};

static const
struct { int type3_2; XFigJoinType type; }
joinTypeMap[] =
{
    {XFig3_2JoinMiter, XFigJoinMiter},
    {XFig3_2JoinRound, XFigJoinRound},
    {XFig3_2JoinBevel, XFigJoinBevel}
};
static const int joinTypeMapSize = sizeof(joinTypeMap)/sizeof(joinTypeMap[0]);

static inline
XFigJoinType
joinType( int type3_2 )
{
    XFigJoinType result = XFigJoinMiter;
    for (int i = 0; i<joinTypeMapSize; ++i) {
        if (joinTypeMap[i].type3_2 == type3_2) {
            result = joinTypeMap[i].type;
            break;
        }
    }
    return result;
}

static inline
XFigFillType fillType(int type3_2)
{
    return
        ((0 <= type3_2) && (type3_2 <= 40)) ?  XFigFillSolid :
        ((41 <= type3_2) && (type3_2 <= 62)) ? XFigFillPattern :
        /*(type3_2 == -1) ?*/                  XFigFillNone;
}

enum XFig3_2FillPatternType
{
    XFig3_2FillLeftDiagonal30Degree = 41,
    XFig3_2FillRightDiagonal30Degree = 42,
    XFig3_2FillCrossHatch30Degree = 43,
    XFig3_2FillLeftDiagonal45Degree = 44,
    XFig3_2FillRightDiagonal45Degree = 45,
    XFig3_2FillCrossHatch45Degree = 46,
    XFig3_2FillHorizontalBricks = 47,
    XFig3_2FillVerticalBricks = 48,
    XFig3_2FillHorizontalLines = 49,
    XFig3_2FillVerticalLines = 50,
    XFig3_2FillCrossHatch = 51,
    XFig3_2FillHorizontalShinglesSkewedRight = 52,
    XFig3_2FillHorizontalShinglesSkewedLeft = 53,
    XFig3_2FillVerticalShinglesSkewedDown = 54,
    XFig3_2FillVerticalShinglesSkewedUp = 55,
    XFig3_2FillFishScales = 56,
    XFig3_2FillSmallFishScales = 57,
    XFig3_2FillCircles = 58,
    XFig3_2FillHexagons = 59,
    XFig3_2FillOctagons = 60,
    XFig3_2FillHorizontalTireTreads = 61,
    XFig3_2FillVerticalTireTreads = 62
};
static const
struct { XFig3_2FillPatternType type3_2; XFigFillPatternType type; }
fillPatternTypeMap[] =
{
    {XFig3_2FillLeftDiagonal30Degree, XFigFillLeftDiagonal30Degree},
    {XFig3_2FillRightDiagonal30Degree, XFigFillRightDiagonal30Degree},
    {XFig3_2FillCrossHatch30Degree, XFigFillCrossHatch30Degree},
    {XFig3_2FillLeftDiagonal45Degree, XFigFillLeftDiagonal45Degree},
    {XFig3_2FillRightDiagonal45Degree, XFigFillRightDiagonal45Degree},
    {XFig3_2FillCrossHatch45Degree, XFigFillCrossHatch45Degree},
    {XFig3_2FillHorizontalBricks, XFigFillHorizontalBricks},
    {XFig3_2FillVerticalBricks, XFigFillVerticalBricks},
    {XFig3_2FillHorizontalLines, XFigFillHorizontalLines},
    {XFig3_2FillVerticalLines, XFigFillVerticalLines},
    {XFig3_2FillCrossHatch, XFigFillCrossHatch},
    {XFig3_2FillHorizontalShinglesSkewedRight, XFigFillHorizontalShinglesSkewedRight},
    {XFig3_2FillHorizontalShinglesSkewedLeft, XFigFillHorizontalShinglesSkewedLeft},
    {XFig3_2FillVerticalShinglesSkewedDown, XFigFillVerticalShinglesSkewedDown},
    {XFig3_2FillVerticalShinglesSkewedUp, XFigFillVerticalShinglesSkewedUp},
    {XFig3_2FillFishScales, XFigFillFishScales},
    {XFig3_2FillSmallFishScales, XFigFillSmallFishScales},
    {XFig3_2FillCircles, XFigFillCircles},
    {XFig3_2FillHexagons, XFigFillHexagons},
    {XFig3_2FillOctagons, XFigFillOctagons},
    {XFig3_2FillHorizontalTireTreads, XFigFillHorizontalTireTreads},
    {XFig3_2FillVerticalTireTreads, XFigFillVerticalTireTreads}
};
static const int fillPatternTypeMapSize = sizeof(fillPatternTypeMap)/sizeof(fillPatternTypeMap[0]);

static inline
XFigFillPatternType fillPatternType(int type3_2)
{
    XFigFillPatternType result = XFigFillLeftDiagonal30Degree;
    for (int i = 0; i<fillPatternTypeMapSize; ++i) {
        if (fillPatternTypeMap[i].type3_2 == type3_2) {
            result = fillPatternTypeMap[i].type;
            break;
        }
    }
    return result;
}

static const
struct XFig3_2PostScriptFontData {
    const char* family;
    QFont::Weight weight;
    QFont::Style style;
} postScriptFontDataTable[35] = {
    { "times", QFont::Normal, QFont::StyleNormal },         // Times Roman
    { "times", QFont::Normal, QFont::StyleItalic },         // Times Italic
    { "times", QFont::Bold, QFont::StyleNormal },           // Times Bold
    { "times", QFont::Bold, QFont::StyleItalic },           // Times Bold Italic
    { "avantgarde", QFont::Normal, QFont::StyleNormal },    // AvantGarde Book
    { "avantgarde", QFont::Normal, QFont::StyleOblique },   // AvantGarde Book Oblique
    { "avantgarde", QFont::DemiBold, QFont::StyleNormal },  // AvantGarde Demi
    { "avantgarde", QFont::DemiBold, QFont::StyleOblique }, // AvantGarde Demi Oblique
    { "bookman", QFont::Light, QFont::StyleNormal },        // Bookman Light
    { "bookman", QFont::Light, QFont::StyleItalic },        // Bookman Light Italic
    { "bookman", QFont::DemiBold, QFont::StyleNormal },     // Bookman Demi
    { "bookman", QFont::DemiBold, QFont::StyleItalic },     // Bookman Demi Italic
    { "courier", QFont::Normal, QFont::StyleNormal },       // Courier
    { "courier", QFont::Normal, QFont::StyleOblique },      // Courier Oblique
    { "courier", QFont::Bold, QFont::StyleNormal },         // Courier Bold
    { "courier", QFont::Bold, QFont::StyleOblique },        // Courier Bold Oblique
    { "helvetica", QFont::Normal, QFont::StyleNormal },     // Helvetica
    { "helvetica", QFont::Normal, QFont::StyleOblique },    // Helvetica Oblique
    { "helvetica", QFont::Bold, QFont::StyleNormal },       // Helvetica Bold
    { "helvetica", QFont::Bold, QFont::StyleOblique },      // Helvetica Bold Oblique
    { "helvetica", QFont::Normal, QFont::StyleNormal },     // Helvetica Narrow
    { "helvetica", QFont::Normal, QFont::StyleOblique },    // Helvetica Narrow Oblique
    { "helvetica", QFont::Bold, QFont::StyleNormal },       // Helvetica Narrow Bold
    { "helvetica", QFont::Bold, QFont::StyleOblique },      // Helvetica Narrow Bold Oblique
    { "newcenturyschoolbook", QFont::Normal, QFont::StyleNormal }, // New Century Schoolbook
    { "newcenturyschoolbook", QFont::Normal, QFont::StyleItalic }, // New Century Italic
    { "newcenturyschoolbook", QFont::Bold, QFont::StyleNormal },   // New Century Bold
    { "newcenturyschoolbook", QFont::Bold, QFont::StyleItalic },   // New Century Bold Italic
    { "palatino", QFont::Normal, QFont::StyleNormal },      // Palatino Roman
    { "palatino", QFont::Normal, QFont::StyleItalic },      // Palatino Italic
    { "palatino", QFont::Bold, QFont::StyleNormal },        // Palatino Bold
    { "palatino", QFont::Bold, QFont::StyleItalic },        // Palatino Bold Italic
    { "symbol", QFont::Normal, QFont::StyleNormal },        // Symbol
    { "zapfchancery", QFont::Normal, QFont::StyleNormal },  // Zapf Chancery Medium Italic
    { "zapfdingbats", QFont::Normal, QFont::StyleNormal },  // Zapf Dingbats
};

enum XFig3_2LatexFontType{
    XFig3_2LatexFontDefault = 0,
    XFig3_2LatexFontRoman = 1,
    XFig3_2LatexFontBold = 2,
    XFig3_2LatexFontItalic = 3,
    XFig3_2LatexFontSansSerif = 4,
    XFig3_2LatexFontTypewriter = 5
};

static const
struct PaperSizeIdMap {
    const char* stringId;
    XFigPageSizeType type;
} paperSizeTable[] = {
    { "Letter",  XFigPageSizeLetter },
    { "Legal",   XFigPageSizeLegal },
    { "Ledger",  XFigPageSizeTabloid },
    { "Tabloid", XFigPageSizeTabloid },

    { "A", XFigPageSizeA },
    { "B", XFigPageSizeB },
    { "C", XFigPageSizeC },
    { "D", XFigPageSizeD },
    { "E", XFigPageSizeE },

    { "A9", XFigPageSizeA9 },
    { "A8", XFigPageSizeA8 },
    { "A7", XFigPageSizeA7 },
    { "A6", XFigPageSizeA6 },
    { "A5", XFigPageSizeA5 },
    { "A4", XFigPageSizeA4 },
    { "A3", XFigPageSizeA3 },
    { "A2", XFigPageSizeA2 },
    { "A1", XFigPageSizeA1 },
    { "A0", XFigPageSizeA0 },

    { "B10", XFigPageSizeB10 },
    { "B9",  XFigPageSizeB9 },
    { "B8",  XFigPageSizeB8 },
    { "B7",  XFigPageSizeB7 },
    { "B6",  XFigPageSizeB6 },
    { "B5",  XFigPageSizeB5 },
    { "B4",  XFigPageSizeB4 },
    { "B3",  XFigPageSizeB3 },
    { "B2",  XFigPageSizeB2 },
    { "B1",  XFigPageSizeB1 },
    { "B0",  XFigPageSizeB0 }
};
static const int paperSizeCount = sizeof(paperSizeTable)/sizeof(paperSizeTable[0]);

static inline
XFigPageSizeType
pageSizeType( const QString& stringId )
{
    XFigPageSizeType result = XFigPageSizeUnknown;
    for( int i = 0; i<paperSizeCount; ++i) {
        const PaperSizeIdMap& idMap = paperSizeTable[i];
        if (stringId == QLatin1String(idMap.stringId)) {
            result = idMap.type;
            break;
        }
    }
    return result;
}

XFigDocument*
XFigParser::parse( QIODevice* device )
{
    XFigParser parser(device);
    XFigDocument* document = parser.takeDocument();
    return document;
}


XFigParser::XFigParser( QIODevice* device )
  : m_Document(0)
  , m_XFigStreamLineReader( device )
{
    if( (device == 0) || (m_XFigStreamLineReader.hasError()) )
        return;

    const QTextCodec* codec = QTextCodec::codecForName("ISO 8859-1");
    m_TextDecoder = codec->makeDecoder();

    // setup
    if (! parseHeader())
        return;

    XFigPage* page = new XFigPage;

    while (! m_XFigStreamLineReader.readNextObjectLine()) {
        const int objectCode = m_XFigStreamLineReader.objectCode();
        const QString objectComment = m_XFigStreamLineReader.comment();

        if (objectCode == XFig3_2ColorObjectId) {
            parseColorObject();
        } else if ((XFig3_2EllipseObjectId<=objectCode) && (objectCode<=XFig3_2CompoundObjectId)) {
            XFigAbstractObject* object =
                (objectCode == XFig3_2EllipseObjectId) ?  parseEllipse() :
                (objectCode == XFig3_2PolylineObjectId) ? parsePolyline() :
                (objectCode == XFig3_2SplineObjectId) ?   parseSpline() :
                (objectCode == XFig3_2TextObjectId) ?     parseText() :
                (objectCode == XFig3_2ArcObjectId) ?      parseArc() :
                /*else XFig3_2CompoundObjectId)*/         parseCompoundObject();
            if (object != 0) {
                object->setComment(objectComment);
                page->addObject(object);
            }
        } else {
            // should not occur
            qWarning() << "unknown object type:" << objectCode;
        }
    }

    m_Document->addPage( page );
}

XFigParser::~XFigParser()
{
    delete m_TextDecoder;
    delete m_Document;
}

bool
XFigParser::parseHeader()
{
    // start to parse
    m_XFigStreamLineReader.readNextLine(XFigStreamLineReader::TakeComment);
    const QString versionString = m_XFigStreamLineReader.line();
    if (! versionString.startsWith(QLatin1String("#FIG 3.")) ||
        (versionString.length() < 8)) {
        qWarning() << "ERROR: no xfig file or wrong header";
        return false;
    }

    const QChar minorVersion = versionString.at(7);
    if (minorVersion == QLatin1Char('2')) {
        m_XFigVersion = 320;
    } else if (minorVersion == QLatin1Char('1')) {
        m_XFigVersion = 310;
    } else {
        qWarning() << "ERROR: unsupported xfig version";
        return false;
    }

    m_Document = new XFigDocument;

    // assume header is broken by default
    bool isHeaderCorrect = false;
    do {
        // orientation
        if (m_XFigStreamLineReader.readNextLine()) {
            const QString orientationString = m_XFigStreamLineReader.line().trimmed();
            const XFigPageOrientation pageOrientation =
                (orientationString == QLatin1String("Landscape")) ? XFigPageLandscape :
                (orientationString == QLatin1String("Portrait")) ?  XFigPagePortrait :
                                                                    XFigPageOrientationUnknown;
// qDebug()<<"orientation:"<<orientationString<<pageOrientation;
            if (pageOrientation == XFigPageOrientationUnknown)
                qWarning() << "ERROR: invalid orientation";

            m_Document->setPageOrientation( pageOrientation );
        } else {
            break;
        }

        // justification, ("Center" or "Flush Left")
        if (! m_XFigStreamLineReader.readNextLine()) {
            break;
        }

        // units
        if (m_XFigStreamLineReader.readNextLine()) {
            const QString unitTypeString = m_XFigStreamLineReader.line().trimmed();
            const XFigUnitType unitType =
                (unitTypeString == QLatin1String("Metric")) ? XFigUnitMetric :
                (unitTypeString == QLatin1String("Inches")) ? XFigUnitInches :
                                                            XFigUnitTypeUnknown;
//     qDebug() << "unittype:"<<unitTypeString<<unitType;
            if (unitType == XFigUnitTypeUnknown)
                qWarning() << "ERROR: invalid units";

            m_Document->setUnitType( unitType );
        } else {
            break;
        }

        if (m_XFigVersion == 320) {
            if (m_XFigStreamLineReader.readNextLine()) {
                const QString pageSizeString = m_XFigStreamLineReader.line().trimmed();
                const XFigPageSizeType pageSizeType = ::pageSizeType(pageSizeString);
// qDebug() << "pagesize:"<<pageSizeString<<pageSizeType;
                m_Document->setPageSizeType(pageSizeType);
            } else {
                break;
            }

            // export and print magnification, %
            if (m_XFigStreamLineReader.readNextLine()) {
                const QString magnificationString = m_XFigStreamLineReader.line();
                const float magnification = magnificationString.toFloat();
                // TODO
                Q_UNUSED(magnification);
// qDebug() << "magnification:"<<magnificationString<<magnification;
            } else {
                break;
            }

            // singe or multiple page
            if (m_XFigStreamLineReader.readNextLine()) {
                const QString singleOrMultiplePagesString = m_XFigStreamLineReader.line().trimmed();
                // multiple pages not yet supported
                if (singleOrMultiplePagesString != QLatin1String("Single")) {
                    break;
                }
            } else {
                break;
            }

            // transparent color for GIF export: -3=background, -2=None, -1=Default, 0-31 for standard colors, 32- for user colors
            if (m_XFigStreamLineReader.readNextLine()) {
                const QString transparentColorString = m_XFigStreamLineReader.line();
                const int transparentColor = transparentColorString.toInt();
                // TODO
                Q_UNUSED(transparentColor);
// qDebug() << "transparentColor:"<<transparentColorString<<transparentColor;
            } else {
                break;
            }
        }

        // resolution and coordinate system
        if (m_XFigStreamLineReader.readNextLine(XFigStreamLineReader::CollectComments)) {
            qint32 coordinateSystemType;
            qint32 resolution;
            QString line = m_XFigStreamLineReader.line();
            QTextStream textStream(&line, QIODevice::ReadOnly);
            textStream >> resolution >> coordinateSystemType;

            const XFigCoordSystemOriginType coordSystemOriginType =
                (coordinateSystemType == 1) ? XFigCoordSystemOriginLowerLeft :
                (coordinateSystemType == 2) ? XFigCoordSystemOriginUpperLeft :
                                            XFigCoordSystemOriginTypeUnknown;
            m_Document->setCoordSystemOriginType( coordSystemOriginType ); // said to be ignored and always upper-left
            m_Document->setResolution(resolution);
            m_Document->setComment(m_XFigStreamLineReader.comment());

//     qDebug() << "resolution+coordinateSystemType:"<<resolution<<coordinateSystemType;
        } else {
            break;
        }
        isHeaderCorrect = true;
    } while (false);

    if (! isHeaderCorrect) {
        delete m_Document;
        m_Document = 0;
    }

    return isHeaderCorrect;
}


static inline
int
parseTwoDigitHexValue(QTextStream& textStream)
{
    int result = 0;

    char digit[2];
    textStream >> digit[1] >> digit[0];

    int faktor = 1;
    for (int i = 0; i < 2; i++) {
        const int value =
            ('0' <= digit[i]  && digit[i] <= '9') ? digit[i] - '0' :
            ('A' <= digit[i]  && digit[i] <= 'F') ? digit[i] - 'A' + 10 :
            ('a' <= digit[i]  && digit[i] <= 'f') ? digit[i] - 'a' + 10 :
            /* bogus data */                        0;
        result += value * faktor;
        faktor = 16;
    }

    return result;
};

void XFigParser::parseColorObject()
{
    int colorNumber;

    QString line = m_XFigStreamLineReader.line();
    QTextStream textStream(&line, QIODevice::ReadOnly);
    textStream >> colorNumber;
    if ((colorNumber < 32) || (543 < colorNumber)) {
        qWarning() << "bad colorNumber:" << colorNumber;
        return;
    }

    QChar hashChar;
    textStream >> ws >> hashChar;
    const int red = parseTwoDigitHexValue(textStream);
    const int green = parseTwoDigitHexValue(textStream);
    const int blue = parseTwoDigitHexValue(textStream);

    m_Document->setUserColor(colorNumber, QColor(red, green, blue));
}

XFigAbstractObject*
XFigParser::parseArc()
{
// qDebug()<<"arc";

    QScopedPointer<XFigArcObject> arcObject(new XFigArcObject);

    int sub_type, line_style, thickness, pen_color, fill_color,
    depth, pen_style, area_fill, cap_style, direction,
    forwardArrow, backwardArrow, x1, y1, x2, y2, x3, y3;
    float center_x, center_y;
    float style_val;

    // first line
    QString line = m_XFigStreamLineReader.line();
    QTextStream textStream(&line, QIODevice::ReadOnly);
    textStream
        >> sub_type >> line_style >> thickness >> pen_color >> fill_color
        >> depth >> pen_style >> area_fill >> style_val >> cap_style
        >> direction >> forwardArrow >> backwardArrow
        >> center_x >> center_y >> x1 >> y1 >> x2 >> y2 >> x3 >> y3;

    if (forwardArrow > 0) {
        QScopedPointer<XFigArrowHead> arrowHead(parseArrowHead());
        if (arrowHead.isNull()) {
            return 0;
        }
        arcObject->setForwardArrow(arrowHead.take());
    }

    if (backwardArrow > 0) {
        QScopedPointer<XFigArrowHead> arrowHead(parseArrowHead());
        if (arrowHead.isNull()) {
            return 0;
        }
        arcObject->setBackwardArrow(arrowHead.take());
    }

    const XFigArcObject::Subtype subtype =
        (sub_type==1) ?   XFigArcObject::OpenEnded :
        /*(sub_type==2)*/ XFigArcObject::PieWedgeClosed;
    arcObject->setSubtype(subtype);
    const XFigArcObject::Direction arcDirection =
        (direction==1) ?   XFigArcObject::CounterClockwise :
        /*(direction==0)*/ XFigArcObject::Clockwise;
    arcObject->setDirection(arcDirection);
    arcObject->setCenterPoint(XFigPoint(center_x, center_y));
    arcObject->setPoints(XFigPoint(x1, y1), XFigPoint(x2, y2), XFigPoint(x3, y3));
    arcObject->setCapType(capType(cap_style));
    arcObject->setDepth( depth );
    const XFigFillType fillType = ::fillType(area_fill);
    if (fillType == XFigFillSolid) {
        arcObject->setFillTinting(area_fill);
    } else if (fillType == XFigFillPattern) {
        arcObject->setFillPatternType(::fillPatternType(area_fill));
    } else {
        arcObject->setFillNone();
    }
    arcObject->setFillColorId(fill_color);
    arcObject->setLine( lineType(line_style), thickness, style_val, pen_color );

    return arcObject.take();
}

XFigAbstractObject*
XFigParser::parseEllipse()
{
// qDebug()<<"ellipse";

    QScopedPointer<XFigEllipseObject> ellipseObject(new XFigEllipseObject);

    qint32 sub_type, line_style, thickness, pen_color, fill_color,
           depth, pen_style/*not used*/, area_fill, direction/*always 1*/,
           center_x, center_y, radius_x, radius_y, start_x, start_y, end_x, end_y;
    float style_val, angle;

    // ellipse data line
    QString line = m_XFigStreamLineReader.line();
    QTextStream textStream(&line, QIODevice::ReadOnly);
    textStream
        >> sub_type >> line_style >> thickness >> pen_color >> fill_color
        >> depth >> pen_style >> area_fill >> style_val >> direction
        >> angle >> center_x >> center_y >> radius_x >> radius_y
        >> start_x >> start_y >> end_x >> end_y;

    const XFigEllipseObject::Subtype subtype =
        (sub_type==1) ?   XFigEllipseObject::EllipseByRadii :
        (sub_type==2) ?   XFigEllipseObject::EllipseByDiameter :
        (sub_type==3) ?   XFigEllipseObject::CircleByRadius :
        /*(sub_type==4)*/ XFigEllipseObject::CircleByDiameter;
    ellipseObject->setSubtype(subtype);
    ellipseObject->setCenterPoint(XFigPoint(center_x, center_y));
    ellipseObject->setStartEnd(XFigPoint(start_x,start_y), XFigPoint(end_x,end_y));
    ellipseObject->setRadii(radius_x, radius_y);
    ellipseObject->setXAxisAngle(angle);

    ellipseObject->setDepth( depth );
    const XFigFillType fillType = ::fillType(area_fill);
    if (fillType == XFigFillSolid) {
        ellipseObject->setFillTinting(area_fill);
    } else if (fillType == XFigFillPattern) {
        ellipseObject->setFillPatternType(::fillPatternType(area_fill));
    } else {
        ellipseObject->setFillNone();
    }
    ellipseObject->setFillColorId(fill_color);
    ellipseObject->setLine( lineType(line_style), thickness, style_val, pen_color );

    return ellipseObject.take();
}

XFigAbstractObject*
XFigParser::parsePolyline()
{
// qDebug()<<"polyline";

    QScopedPointer<XFigAbstractPolylineObject> abstractPolylineObject(0);

    qint32 sub_type, line_style, thickness, pen_color, fill_color,
           depth, pen_style, area_fill, join_style, cap_style, radius,
           forward_arrow, backward_arrow, npoints;
    float style_val;

    // polyline data line
    QString line = m_XFigStreamLineReader.line();
    QTextStream textStream(&line, QIODevice::ReadOnly);
    textStream
        >> sub_type >> line_style >> thickness >> pen_color >> fill_color
        >> depth >> pen_style >> area_fill >> style_val >> join_style
        >> cap_style >> radius >> forward_arrow >> backward_arrow
        >> npoints;
// qDebug() << sub_type << line_style << thickness << pen_color << fill_color << depth << pen_style
//          << area_fill << style_val << join_style << cap_style << radius << forward_arrow << backward_arrow << npoints;

    // ignore line with useless point number
    if (npoints < 1) {
        return 0;
    }

    if (sub_type==XFig3_2PolylinePolylineId) {
        XFigPolylineObject* polylineObject = new XFigPolylineObject;
        polylineObject->setCapType(capType(cap_style));
        abstractPolylineObject.reset(polylineObject);
    } else if (sub_type==XFig3_2PolylinePolygonId) {
        abstractPolylineObject.reset(new XFigPolygonObject);
    } else if (sub_type==XFig3_2PolylineBoxId) {
        abstractPolylineObject.reset(new XFigBoxObject);
    } else if (sub_type==XFig3_2PolylineArcBoxId) {
        XFigBoxObject* boxObject = new XFigBoxObject;
        boxObject->setRadius(radius);
        abstractPolylineObject.reset(boxObject);
    } else if (sub_type==XFig3_2PolylinePictureBoundingBoxId) {
        XFigPictureBoxObject* pictureBoxObject = new XFigPictureBoxObject;
        if (! m_XFigStreamLineReader.readNextLine()) {
            return 0;
        }

        QString line = m_XFigStreamLineReader.line();
        QTextStream textStream(&line, QIODevice::ReadOnly);

        int flipped;
        QString fileName;
        textStream >> flipped >> fileName;

        pictureBoxObject->setIsFlipped( flipped != 0 );
        pictureBoxObject->setFileName( fileName );

        abstractPolylineObject.reset(pictureBoxObject);
    } else {
    }

    if (forward_arrow > 0) {
        QScopedPointer<XFigArrowHead> arrowHead(parseArrowHead());
        if (arrowHead.isNull()) {
            return 0;
        }

        if (abstractPolylineObject->typeId() == XFigAbstractObject::PolylineId) {
            XFigPolylineObject* polylineObject =
                static_cast<XFigPolylineObject*>(abstractPolylineObject.data());
            polylineObject->setForwardArrow(arrowHead.take());
        }
    }

    if (backward_arrow > 0) {
        QScopedPointer<XFigArrowHead> arrowHead(parseArrowHead());
        if (arrowHead.isNull()) {
            return 0;
        }

        if (abstractPolylineObject->typeId() == XFigAbstractObject::PolylineId) {
            XFigPolylineObject* polylineObject =
                static_cast<XFigPolylineObject*>(abstractPolylineObject.data());
            polylineObject->setBackwardArrow(arrowHead.take());
        }
    }

    // points line
    const QVector<XFigPoint> points = parsePoints(npoints);
    if (points.count() != npoints) {
        return 0;
    }

    // check box:
    if ((abstractPolylineObject->typeId()==XFigAbstractObject::BoxId) &&
        (points.count()!=5)) {
        qWarning() << "box object does not have 5 points, but points:" << points.count();
        return 0;
    }
    abstractPolylineObject->setPoints(points);
    abstractPolylineObject->setDepth(depth);
    const XFigFillType fillType = ::fillType(area_fill);
    if (fillType == XFigFillSolid) {
        abstractPolylineObject->setFillTinting(area_fill);
    } else if (fillType == XFigFillPattern) {
        abstractPolylineObject->setFillPatternType(::fillPatternType(area_fill));
    } else {
        abstractPolylineObject->setFillNone();
    }
    abstractPolylineObject->setFillColorId(fill_color);
    abstractPolylineObject->setLine(lineType(line_style), thickness, style_val, pen_color);
    abstractPolylineObject->setJoinType(joinType(join_style));

    return abstractPolylineObject.take();
}

XFigAbstractObject*
XFigParser::parseSpline()
{
// qDebug()<<"spline";

    qint32 sub_type, line_style, thickness, pen_color, fill_color, depth,
           pen_style, area_fill, cap_style, forward_arrow, backward_arrow, npoints;
    float style_val;

    // this should be a spline
    QString line = m_XFigStreamLineReader.line();
    QTextStream textStream(&line, QIODevice::ReadOnly);
    textStream
        >> sub_type >> line_style >> thickness >> pen_color >> fill_color
        >>  depth >> pen_style >> area_fill >> style_val >> cap_style
        >> forward_arrow >> backward_arrow >> npoints;
// qDebug() << sub_type << line_style << thickness << pen_color << fill_color << depth << pen_style
//          << area_fill << style_val << cap_style << forward_arrow << backward_arrow << npoints;

    // ignore line with useless point number
    if (npoints < 1) {
        return 0;
    }

    // TODO: no idea yet how to translate the xfig splines to odf ones
    // thus simply creating polygones/polylines for now :/
    QScopedPointer<XFigAbstractPolylineObject> abstractPolylineObject(0);

    if ((sub_type==XFig3_2SplineOpenApproximatedId) ||
        (sub_type==XFig3_2SplineOpenInterpolatedId) ||
        (sub_type==XFig3_2SplineOpenXId)) {
        XFigPolylineObject* polylineObject = new XFigPolylineObject;
        polylineObject->setCapType(capType(cap_style));
        abstractPolylineObject.reset(polylineObject);
    } else {
        abstractPolylineObject.reset(new XFigPolygonObject);
    }

    if (forward_arrow > 0) {
        QScopedPointer<XFigArrowHead> arrowHead(parseArrowHead());
        if (arrowHead.isNull()) {
            return 0;
        }

        if (abstractPolylineObject->typeId() == XFigAbstractObject::PolylineId) {
            XFigPolylineObject* polylineObject =
                static_cast<XFigPolylineObject*>(abstractPolylineObject.data());
            polylineObject->setForwardArrow(arrowHead.take());
        }
    }

    if (backward_arrow > 0) {
        QScopedPointer<XFigArrowHead> arrowHead(parseArrowHead());
        if (arrowHead.isNull()) {
            return 0;
        }

        if (abstractPolylineObject->typeId() == XFigAbstractObject::PolylineId) {
            XFigPolylineObject* polylineObject =
                static_cast<XFigPolylineObject*>(abstractPolylineObject.data());
            polylineObject->setBackwardArrow(arrowHead.take());
        }
    }

    // points line
    const QVector<XFigPoint> points = parsePoints(npoints);
    if (points.count() != npoints) {
        return 0;
    }

    // control points line
    parseFactors(npoints);

    abstractPolylineObject->setPoints(points);
    abstractPolylineObject->setDepth(depth);
    const XFigFillType fillType = ::fillType(area_fill);
    if (fillType == XFigFillSolid) {
        abstractPolylineObject->setFillTinting(area_fill);
    } else if (fillType == XFigFillPattern) {
        abstractPolylineObject->setFillPatternType(::fillPatternType(area_fill));
    } else {
        abstractPolylineObject->setFillNone();
    }
    abstractPolylineObject->setFillColorId(fill_color);
    abstractPolylineObject->setLine(lineType(line_style), thickness, style_val, pen_color);
    abstractPolylineObject->setJoinType(XFigJoinRound);

    return abstractPolylineObject.take();
}

XFigAbstractObject*
XFigParser::parseText()
{
// qDebug()<<"text";

    QScopedPointer<XFigTextObject> textObject(new XFigTextObject);

    qint32 sub_type, color, depth, pen_style, font, font_flags, x, y;
    float font_size, angle, height, length;

    QString line = m_XFigStreamLineReader.line();
    QTextStream textStream(&line, QIODevice::ReadOnly);
    textStream
        >> sub_type >> color >> depth >> pen_style >> font >> font_size
        >> angle >> font_flags >> height >> length >> x >> y;

    const XFigTextAlignment textAlignment =
        (sub_type == XFig3_2TextCenterAligned) ? XFigTextCenterAligned :
        (sub_type == XFig3_2TextRightAligned) ?  XFigTextRightAligned :
        /*(sub_type == XFig3_2TextLeftAligned)*/ XFigTextLeftAligned;
    textObject->setTextAlignment(textAlignment);
    textObject->setBaselineStartPoint(XFigPoint(x,y));
    textObject->setSize(length, height);
    textObject->setXAxisAngle(angle);
    textObject->setColorId(color);
    textObject->setDepth(depth);
    textObject->setIsHidden(font_flags&XFig3_2TextHidden);

    XFigFontData fontData;
    if (font_flags&XFig3_2TextPostScriptFont) {
        if (font == -1) {
            /* default means */;
        } else {
            if ((0<=font) && (font<=34)) {
                const XFig3_2PostScriptFontData& postScriptFontData = postScriptFontDataTable[font];
                fontData.mFamily = QLatin1String(postScriptFontData.family);
                fontData.mWeight = postScriptFontData.weight;
                fontData.mStyle = postScriptFontData.style;
            }
        }
    } else {
        // LaTeX font
        if (font == XFig3_2LatexFontSansSerif) {
            fontData.mFamily = QLatin1String("helvetica");
        } else if (font == XFig3_2LatexFontTypewriter) {
            fontData.mFamily = QLatin1String("courier");
        } else if ((XFig3_2LatexFontDefault<=font) && (font<=XFig3_2LatexFontItalic)) {
            fontData.mFamily = QLatin1String("times");
            if (font==XFig3_2LatexFontBold) {
                fontData.mWeight = QFont::Bold;
            } else if (font==XFig3_2LatexFontItalic) {
                fontData.mStyle = QFont::StyleItalic;
            }
        }
    }
    fontData.mSize = font_size;
    textObject->setFontData(fontData);

    // read text
    // skip one space char used as separator
    const QString textData = line.mid(textStream.pos()+1);

    // TODO: improve this, not really sane yet
    QString text;
    for (int i = 0; i < textData.length(); ++i) {
        const QChar textChar = textData.at(i);
        if (textChar == QLatin1Char('\\')) {

            if ((i+3) >= textData.length()) {
                // encoded char, especially end marker not possible, is broken
                break;
            }
            // read digits and check this is really about an octal encoded char
            int digitValues[3];
            bool isOctalEncodedChar = true;
            for (int d = 0; d < 3; ++d) {
                const int digitValue = textData.at(i+1+d).digitValue();
                if ((digitValue < 0) || (7 < digitValue)) {
                    isOctalEncodedChar = false;
                    break;
                }
                digitValues[d] = digitValue;
            }
            if (isOctalEncodedChar) {
                unsigned char charValue =
                    digitValues[0] * 64 +
                    digitValues[1] * 8 +
                    digitValues[2];
                // \001 is used as end marker
                if (charValue == 1) {
                    break;
                }
                const char encodedChar = static_cast<char>(charValue);
                text.append( m_TextDecoder->toUnicode(&encodedChar,1) );

                // digits are consumed
                i += 3;
            } else if (textData.at(i+1) == QLatin1Char('\\')) {
                text.append( textChar );
                // double \ is consumed
                ++i;
            }
        } else {
            text.append( textChar );
        }
    }
    textObject->setText(text);

    return textObject.take();
}

XFigAbstractObject*
XFigParser::parseCompoundObject()
{
// qDebug()<<"compound";

    QScopedPointer<XFigCompoundObject> compoundObject(new XFigCompoundObject);

    {
        qint32 upperLeftX, upperLeftY, lowerRightX, lowerRightY;

        QString line = m_XFigStreamLineReader.line();
        QTextStream textStream(&line, QIODevice::ReadOnly);
        textStream >> upperLeftX >> upperLeftY >> lowerRightX >> lowerRightY;

        const XFigBoundingBox boundingBox( XFigPoint(upperLeftX, upperLeftY),
                                    XFigPoint(lowerRightX, lowerRightY) );
        compoundObject->setBoundingBox( boundingBox );
    }

    while (! m_XFigStreamLineReader.readNextObjectLine()) {
        const int objectCode = m_XFigStreamLineReader.objectCode();
        const QString objectComment = m_XFigStreamLineReader.comment();

        // end reached?
        if (objectCode == XFig3_2CompoundObjectEndId) {
            break;
        }

        if (objectCode == XFig3_2ColorObjectId) {
            parseColorObject();
        } else if ((XFig3_2EllipseObjectId<=objectCode) && (objectCode<=XFig3_2CompoundObjectId)) {
            XFigAbstractObject* object =
                (objectCode == XFig3_2EllipseObjectId) ?  parseEllipse() :
                (objectCode == XFig3_2PolylineObjectId) ? parsePolyline() :
                (objectCode == XFig3_2SplineObjectId) ?   parseSpline() :
                (objectCode == XFig3_2TextObjectId) ?     parseText() :
                (objectCode == XFig3_2ArcObjectId) ?      parseArc() :
                /*else XFig3_2CompoundObjectId)*/         parseCompoundObject();
            if (object != 0) {
                object->setComment(objectComment);
                compoundObject->addObject(object);
            }
        } else {
            // should not occur
            qWarning() << "unknown object type:" << objectCode;
        }
    }
// qDebug()<<"compound end";

    return compoundObject.take();
}

XFigArrowHead* XFigParser::parseArrowHead()
{
    if (! m_XFigStreamLineReader.readNextLine()) {
        return 0;
    }

    QString line = m_XFigStreamLineReader.line();
    QTextStream textStream(&line, QIODevice::ReadOnly);

    int arrow_type, arrow_style;
    float arrow_thickness, arrow_width, arrow_height;
    textStream
        >> arrow_type >> arrow_style >> arrow_thickness
        >> arrow_width >> arrow_height;

    XFigArrowHead* arrowHead = new XFigArrowHead;
    arrowHead->setType(arrowHeadType(arrow_type, arrow_style));
    arrowHead->setThickness(arrow_thickness );
    arrowHead->setSize(arrow_width, arrow_height);

    return arrowHead;
}

QVector<XFigPoint> XFigParser::parsePoints(int pointCount)
{
    QVector<XFigPoint> result;

    QString pointsText;
    QTextStream pointsTextStream(&pointsText, QIODevice::ReadOnly);
    for (int i = 0; i < pointCount; i++) {
        if (pointsTextStream.atEnd()) {
            if (! m_XFigStreamLineReader.readNextLine()) {
                return QVector<XFigPoint>();
            }

            pointsText = m_XFigStreamLineReader.line();
            pointsTextStream.setString(&pointsText, QIODevice::ReadOnly);
        }

        qint32 x;
        qint32 y;
        pointsTextStream >> x >> y;
// qDebug() << "point:" << i << x << y;

        result.append(XFigPoint(x, y));

        pointsTextStream.skipWhiteSpace();
    }

    return result;
}

QVector<double> XFigParser::parseFactors(int pointCount)
{
    QVector<double> result;

    QString factorsText;
    QTextStream factorsTextStream(&factorsText, QIODevice::ReadOnly);
    for (int i = 0; i < pointCount; i++) {
        if (factorsTextStream.atEnd()) {
            if (! m_XFigStreamLineReader.readNextLine()) {
                return QVector<double>();
            }

            factorsText = m_XFigStreamLineReader.line();
            factorsTextStream.setString(&factorsText, QIODevice::ReadOnly);
        }

        double factor;
        factorsTextStream >> factor;
// qDebug() << "factor:" << i << factor;

        result.append(factor);

        factorsTextStream.skipWhiteSpace();
    }

    return result;
}
