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

#include "xfigparser.h"

// filter
#include "xfigdocument.h"
// Qt
#include <QTextCodec>
#include <QIODevice>
#include <QFont>

#include <KDebug>


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


enum XFig3_2ArrowHeadType
{
    XFig3_2ArrowHeadStick = 0,
    XFig3_2ArrowHeadClosedTriangle = 1,
    XFig3_2ArrowHeadClosedIndentedButt = 2,
    XFig3_2ArrowHeadClosedPointedButt = 3
};
static const
struct { XFig3_2ArrowHeadType type3_2; XFigArrowHeadType type; }
arrowHeadTypeMap[] =
{
    { XFig3_2ArrowHeadStick, XFigArrowHeadStick },
    { XFig3_2ArrowHeadClosedTriangle, XFigArrowHeadClosedTriangle },
    { XFig3_2ArrowHeadClosedIndentedButt,XFigArrowHeadClosedIndentedButt  },
    { XFig3_2ArrowHeadClosedPointedButt, XFigArrowHeadClosedPointedButt }
};
static const int arrowHeadTypeMapSize = sizeof(arrowHeadTypeMap)/sizeof(arrowHeadTypeMap[0]);

static inline
XFigArrowHeadType
arrowHeadType( int type3_2 )
{
    XFigArrowHeadType result = XFigArrowHeadStick;
    for (int i = 0; i<arrowHeadTypeMapSize; ++i) {
        if (arrowHeadTypeMap[i].type3_2 == type3_2) {
            result = arrowHeadTypeMap[i].type;
            break;
        }
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
    { "Ledger",  XFigPageSizeLedger },
    { "Tabloid", XFigPageSizeTabloid },
    { "A",  XFigPageSizeA },
    { "B",  XFigPageSizeB },
    { "C",  XFigPageSizeC },
    { "D",  XFigPageSizeD },
    { "E",  XFigPageSizeE },
    { "A4", XFigPageSizeA4 },
    { "A3", XFigPageSizeA3 },
    { "A2", XFigPageSizeA2 },
    { "A1", XFigPageSizeA1 },
    { "A0", XFigPageSizeA0 },
    { "B5", XFigPageSizeB5 }
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
  : mDocument(0)
  , mTextStream( device )
{
    if( (device == 0) || (mTextStream.status() != QTextStream::Ok) )
        return;

    QTextCodec* codec = QTextCodec::codecForName("ISO 8859-1");
    mTextDecoder = codec->makeDecoder();

    // setup
    if (! parseHeader())
        return;

    XFigPage* page = new XFigPage;

    while (! mTextStream.atEnd()) {
        int objectCode;
        mTextStream >> objectCode;

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
            if (object != 0)
                page->addObject(object);
        } else {
            // should not occur
            kDebug() << "unknown object type:" << objectCode;
        }
    }

    mDocument->addPage( page );
}

XFigParser::~XFigParser()
{
    delete mTextDecoder;
    delete mDocument;
}

bool
XFigParser::parseHeader()
{
    // start to parse
    const QString versionString = mTextStream.readLine();
    if (! versionString.startsWith(QLatin1String("#FIG 3.")) ||
        (versionString.length() < 8)) {
        kDebug() << "ERROR: no xfig file or wrong header";
        return false;
    }

    const QChar minorVersion = versionString.at(7);
    if (minorVersion == QLatin1Char('2')) {
        mXFigVersion = 320;
    } else if (minorVersion == QLatin1Char('1')) {
        mXFigVersion = 310;
    } else {
        kDebug() << "ERROR: unsupported xfig version";
        return false;
    }

    mDocument = new XFigDocument;

    // orientation
    {
        const QString orientationString = mTextStream.readLine();
        const XFigPageOrientation pageOrientation =
            (orientationString == QLatin1String("Landscape")) ? XFigPageLandscape :
            (orientationString == QLatin1String("Portrait")) ?  XFigPagePortrait :
                                                                XFigPageOrientationUnknown;
qDebug()<<"orientation:"<<orientationString<<pageOrientation;
        if (pageOrientation == XFigPageOrientationUnknown)
            kDebug() << "ERROR: invalid orientation";

        mDocument->setPageOrientation( pageOrientation );
    }

    // justification, ("Center" or "Flush Left")
    mTextStream.readLine();

    // units
    {
        const QString unitTypeString = mTextStream.readLine();
        const XFigUnitType unitType =
            (unitTypeString == QLatin1String("Metric")) ? XFigUnitMetric :
            (unitTypeString == QLatin1String("Inches")) ? XFigUnitInches :
                                                          XFigUnitTypeUnknown;
qDebug() << "unittype:"<<unitTypeString<<unitType;
        if (unitType == XFigUnitTypeUnknown)
            kDebug() << "ERROR: invalid units";

        mDocument->setUnitType( unitType );
    }

    if (mXFigVersion == 320) {
        const QString pageSizeString = mTextStream.readLine();
        const XFigPageSizeType pageSizeType = ::pageSizeType(pageSizeString);
qDebug() << "pagesize:"<<pageSizeString<<pageSizeType;
        mDocument->setPageSizeType(pageSizeType);

        // export and print magnification, %
        const QString magnificationString = mTextStream.readLine();
        const float magnification = magnificationString.toFloat();
qDebug() << "magnification:"<<magnificationString<<magnification;

        // singe or multiple page
        /*const QString singleOrMultiplePagesString =*/ mTextStream.readLine();

        // transparent color for GIF export: -3=background, -2=None, -1=Default, 0-31 for standard colors, 32- for user colors
        const QString transparentColorString = mTextStream.readLine();
        const int transparentColor = transparentColorString.toInt();
qDebug() << "transparentColor:"<<transparentColorString<<transparentColor;
    }

    // resolution and coordinate system
    {
        qint32 coordinateSystemType;
        qint32 resolution;
        mTextStream >> resolution >> coordinateSystemType;
        mTextStream.readLine();
        const XFigCoordSystemOriginType coordSystemOriginType =
            (coordinateSystemType == 1) ? XFigCoordSystemOriginLowerLeft :
            (coordinateSystemType == 2) ? XFigCoordSystemOriginUpperLeft :
                                          XFigCoordSystemOriginTypeUnknown;
        mDocument->setCoordSystemOriginType( coordSystemOriginType ); // said to be ignored and always upper-left
        mDocument->setResolution(resolution);

qDebug() << "resolution+coordinateSystemType:"<<resolution<<coordinateSystemType;
    }
    return true;
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

    mTextStream >> colorNumber;
    if ((colorNumber < 32) || (543 < colorNumber)) {
        kDebug() << "bad colorNumber:" << colorNumber;
        mTextStream.readLine();
        return;
    }

    QChar hashChar;
    mTextStream >> ws >> hashChar;
    const int red = parseTwoDigitHexValue(mTextStream);
    const int green = parseTwoDigitHexValue(mTextStream);
    const int blue = parseTwoDigitHexValue(mTextStream);
    mTextStream.readLine();

    mDocument->setUserColor(colorNumber, QColor(red, green, blue));
}

XFigAbstractObject*
XFigParser::parseArc()
{
qDebug()<<"arc";
    XFigArcObject* arcObject = new XFigArcObject;

    int sub_type, line_style, thickness, pen_color, fill_color,
    depth, pen_style, area_fill, cap_style, direction,
    forwardArrow, backwardArrow, x1, y1, x2, y2, x3, y3;
    float center_x, center_y;
    float style_val;

    // first line
    mTextStream
        >> sub_type >> line_style >> thickness >> pen_color >> fill_color
        >> depth >> pen_style >> area_fill >> style_val >> cap_style
        >> direction >> forwardArrow >> backwardArrow
        >> center_x >> center_y >> x1 >> y1 >> x2 >> y2 >> x3 >> y3;
    mTextStream.readLine();

    if (forwardArrow > 0) {
        // forward arrow line
        mTextStream.readLine();
    }

    if (backwardArrow > 0) {
        // backward arrow line
        mTextStream.readLine();
    }

// TODO
    arcObject->setDepth( depth );
    arcObject->setFill( area_fill, fill_color );
    arcObject->setLine( lineType(line_style), thickness, style_val, pen_color );

    return arcObject;
}

XFigAbstractObject*
XFigParser::parseEllipse()
{
qDebug()<<"ellipse";

    XFigEllipseObject* ellipseObject = new XFigEllipseObject;

    qint32 sub_type, line_style, thickness, pen_color, fill_color,
           depth, pen_style/*not used*/, area_fill, direction/*always 1*/,
           center_x, center_y, radius_x, radius_y, start_x, start_y, end_x, end_y;
    float style_val, angle;

    // ellipse data line
    mTextStream >> sub_type >> line_style >> thickness >> pen_color >> fill_color
    >> depth >> pen_style >> area_fill >> style_val >> direction
    >> angle >> center_x >> center_y >> radius_x >> radius_y
    >> start_x >> start_y >> end_x >> end_y;
    mTextStream.readLine();

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
    ellipseObject->setFill( area_fill, fill_color);
    ellipseObject->setLine( lineType(line_style), thickness, style_val, pen_color );

    return ellipseObject;
}

XFigAbstractObject*
XFigParser::parsePolyline()
{
qDebug()<<"polyline";

    XFigAbstractPolylineObject* abstractPolylineObject = 0;

    qint32 sub_type, line_style, thickness, pen_color, fill_color,
           depth, pen_style, area_fill, join_style, cap_style, radius,
           forward_arrow, backward_arrow, npoints;
    float style_val;

    // first line
    mTextStream >> sub_type >> line_style >> thickness >> pen_color >> fill_color
    >> depth >> pen_style >> area_fill >> style_val >> join_style
    >> cap_style >> radius >> forward_arrow >> backward_arrow
    >> npoints;
    mTextStream.readLine();
qDebug() << sub_type << line_style << thickness << pen_color << fill_color << depth << pen_style
         << area_fill << style_val << join_style << cap_style << radius << forward_arrow << backward_arrow << npoints;

    if (sub_type==XFig3_2PolylinePolylineId) {
        XFigPolylineObject* polylineObject = new XFigPolylineObject;
        polylineObject->setCapType(capType(cap_style));
        abstractPolylineObject = polylineObject;
    } else if (sub_type==XFig3_2PolylinePolygonId) {
        abstractPolylineObject = new XFigPolygonObject;
    } else if (sub_type==XFig3_2PolylineBoxId) {
        abstractPolylineObject = new XFigBoxObject;
    } else if (sub_type==XFig3_2PolylineArcBoxId) {
        XFigBoxObject* boxObject = new XFigBoxObject;
        boxObject->setRadius(radius);
        abstractPolylineObject = boxObject;
    } else if (sub_type==XFig3_2PolylinePictureBoundingBoxId) {
        XFigPictureBoxObject* pictureBoxObject = new XFigPictureBoxObject;
        int flipped;
        QString fileName;
        mTextStream >> flipped >> fileName;
        mTextStream.readLine();
        pictureBoxObject->setIsFlipped( flipped != 0 );
        pictureBoxObject->setFileName( fileName );
        abstractPolylineObject = pictureBoxObject;
    } else {
    }

    if (forward_arrow > 0) {
        int arrow_type, arrow_style;
        float arrow_thickness, arrow_width, arrow_height;

        mTextStream >> arrow_type >> arrow_style >> arrow_thickness
                    >> arrow_width >> arrow_height;
        mTextStream.readLine();

        if (abstractPolylineObject->typeId() == XFigAbstractObject::PolylineId) {
            XFigArrowHead* arrowHead = new XFigArrowHead;
            arrowHead->setType(arrowHeadType(arrow_style));
            arrowHead->setIsHollow((arrow_style == 0));
            arrowHead->setThickness(thickness);
            arrowHead->setSize(arrow_width, arrow_height);

            XFigPolylineObject* polylineObject =
                static_cast<XFigPolylineObject*>(abstractPolylineObject);
            polylineObject->setForwardArrow(arrowHead);
        }
    }

    if (backward_arrow > 0) {
        int arrow_type, arrow_style;
        float arrow_thickness, arrow_width, arrow_height;

        mTextStream >> arrow_type >> arrow_style >> arrow_thickness
                    >> arrow_width >> arrow_height;
        mTextStream.readLine();

        if (abstractPolylineObject->typeId() == XFigAbstractObject::PolylineId) {
            XFigArrowHead* arrowHead = new XFigArrowHead;
            arrowHead->setType(arrowHeadType(arrow_style));
            arrowHead->setIsHollow((arrow_style == 0));
            arrowHead->setThickness(thickness);
            arrowHead->setSize(arrow_width, arrow_height);

            XFigPolylineObject* polylineObject =
                static_cast<XFigPolylineObject*>(abstractPolylineObject);
            polylineObject->setBackwardArrow(arrowHead);
        }
    }
    // points line
    QVector<XFigPoint> points;
    for (int i = 0; i < npoints; i++) {
        qint32 x, y;
        mTextStream >> x >> y;
qDebug() << "point:" << i << x << y;
        points.append(XFigPoint(x,y));
    }
    mTextStream.readLine();

    // check box:
    if ((abstractPolylineObject->typeId()==XFigAbstractObject::BoxId) &&
        (points.count()!=5)) {
        kDebug() << "box object does not have 5 points, but points:" << points.count();
        delete abstractPolylineObject;
        return 0;
    }
    abstractPolylineObject->setPoints(points);
    abstractPolylineObject->setDepth(depth);
    abstractPolylineObject->setFill(area_fill, fill_color);
    abstractPolylineObject->setLine(lineType(line_style), thickness, style_val, pen_color);
    abstractPolylineObject->setJoinType(joinType(join_style));

    return abstractPolylineObject;
}

XFigAbstractObject*
XFigParser::parseSpline()
{
qDebug()<<"spline";

    qint32 sub_type, line_style, thickness, pen_color, fill_color, depth,
           pen_style, area_fill, cap_style, forward_arrow, backward_arrow, npoints;
    float style_val;

    // this should be a spline
    mTextStream >> sub_type >> line_style >> thickness >> pen_color >> fill_color
    >>  depth >> pen_style >> area_fill >> style_val >> cap_style
    >> forward_arrow >> backward_arrow >> npoints;
    mTextStream.readLine();
qDebug() << sub_type << line_style << thickness << pen_color << fill_color << depth << pen_style
         << area_fill << style_val << cap_style << forward_arrow << backward_arrow << npoints;

    // TODO: no idea yet how to translate the xfig splines to odf ones
    // thus simply creating polygones/polylines for now :/
    XFigAbstractPolylineObject* abstractPolylineObject = 0;
    if ((sub_type==XFig3_2SplineOpenApproximatedId) ||
        (sub_type==XFig3_2SplineOpenInterpolatedId) ||
        (sub_type==XFig3_2SplineOpenXId)) {
        XFigPolylineObject* polylineObject = new XFigPolylineObject;
        polylineObject->setCapType(capType(cap_style));
        abstractPolylineObject = polylineObject;
    } else {
        abstractPolylineObject = new XFigPolygonObject;
    }

    if (forward_arrow > 0) {
        int arrow_type, arrow_style;
        float arrow_thickness, arrow_width, arrow_height;

        mTextStream >> arrow_type >> arrow_style >> arrow_thickness
                    >> arrow_width >> arrow_height;
        mTextStream.readLine();

        if (abstractPolylineObject->typeId() == XFigAbstractObject::PolylineId) {
            XFigArrowHead* arrowHead = new XFigArrowHead;
            arrowHead->setType(arrowHeadType(arrow_style));
            arrowHead->setIsHollow((arrow_style == 0));
            arrowHead->setThickness(thickness);
            arrowHead->setSize(arrow_width, arrow_height);

            XFigPolylineObject* polylineObject =
                static_cast<XFigPolylineObject*>(abstractPolylineObject);
            polylineObject->setForwardArrow(arrowHead);
        }
    }

    if (backward_arrow > 0) {
        int arrow_type, arrow_style;
        float arrow_thickness, arrow_width, arrow_height;

        mTextStream >> arrow_type >> arrow_style >> arrow_thickness
                    >> arrow_width >> arrow_height;
        mTextStream.readLine();

        if (abstractPolylineObject->typeId() == XFigAbstractObject::PolylineId) {
            XFigArrowHead* arrowHead = new XFigArrowHead;
            arrowHead->setType(arrowHeadType(arrow_style));
            arrowHead->setIsHollow((arrow_style == 0));
            arrowHead->setThickness(thickness);
            arrowHead->setSize(arrow_width, arrow_height);

            XFigPolylineObject* polylineObject =
                static_cast<XFigPolylineObject*>(abstractPolylineObject);
            polylineObject->setBackwardArrow(arrowHead);
        }
    }

    // points line
    QVector<XFigPoint> points;
    for (int i = 0; i < npoints; i++) {
        qint32 x, y;
        mTextStream >> x >> y;
qDebug() << "point:" << i << x << y;
        points.append(XFigPoint(x,y));
    }
    mTextStream.readLine();

    // control points line
    for (int i = 0; i < npoints; i++) {
        float fac;
        mTextStream >> fac;
        // read and ignored for now
    }
    mTextStream.readLine();

    abstractPolylineObject->setPoints(points);
    abstractPolylineObject->setDepth(depth);
    abstractPolylineObject->setFill(area_fill, fill_color);
    abstractPolylineObject->setLine(lineType(line_style), thickness, style_val, pen_color);
    abstractPolylineObject->setJoinType(XFigJoinRound);

    return abstractPolylineObject;
}

XFigAbstractObject*
XFigParser::parseText()
{
qDebug()<<"text";

    XFigTextObject* textObject = new XFigTextObject;

    qint32 sub_type, color, depth, pen_style, font, font_flags, x, y;
    float font_size, angle, height, length;

    mTextStream >> sub_type >> color >> depth >> pen_style >> font >> font_size
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

    bool finished = false;
    QString text;
    while (! finished) {
        char c;
        mTextStream >> c;
        if (c == '\\') {
            const QString ocode = mTextStream.read(3);
            unsigned char charValue =
                (ocode.at(0).toLatin1() - '0') * 64 +
                (ocode.at(1).toLatin1() - '0') * 8 +
                (ocode.at(2).toLatin1() - '0');
            if (charValue == 1) {
                break;
            }
            const char textChar = static_cast<char>(charValue);
            text.append( mTextDecoder->toUnicode(&textChar,1) );
        } else
            text.append(QLatin1Char(c));
    }
    textObject->setText(text);

    return textObject;
}

XFigAbstractObject*
XFigParser::parseCompoundObject()
{
qDebug()<<"compound";
    XFigCompoundObject* compoundObject = new XFigCompoundObject;
    {
        qint32 upperLeftX, upperLeftY, lowerRightX, lowerRightY;

        mTextStream >> upperLeftX >> upperLeftY >> lowerRightX >> lowerRightY;
        mTextStream.readLine();

        const XFigBoundingBox boundingBox( XFigPoint(upperLeftX, upperLeftY),
                                    XFigPoint(lowerRightX, lowerRightY) );
        compoundObject->setBoundingBox( boundingBox );
    }

    while (! mTextStream.atEnd()) {
        int objectCode;
        mTextStream >> objectCode;

        // end reached?
        if (objectCode == XFig3_2CompoundObjectEndId) {
            mTextStream.readLine();
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
            if (object != 0)
                compoundObject->addObject(object);
        } else {
            // should not occur
            kDebug() << "unknown object type:" << objectCode;
        }
    }
qDebug()<<"compound end";

    return compoundObject;
}
