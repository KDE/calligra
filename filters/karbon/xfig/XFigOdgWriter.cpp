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
   Boston, MA 02110-1301, USA.
*/

#include "XFigOdgWriter.h"

// filter
#include "XFigDocument.h"
// Calligra
#include <KoXmlWriter.h>
#include <KoXmlNS.h>
#include <KoOdf.h>
#include <KoStore.h>
#include <KoStoreDevice.h>
#include <KoDocumentInfo.h>

// keep in sync with XFigPageSizeType
static const
struct PageSize
{
    const char* width;
    const char* height;
} pageSizeTable[29] =
{
    {  "8.5in",  "11in"}, // letter
    {  "8.5in",  "14in"}, // legal
    { "11in",    "17in"}, // tabloid

    {  "8.5in",  "11in"}, // A
    { "11in",    "17in"}, // B
    { "17in",    "22in"}, // C
    { "22in",    "34in"}, // D
    { "34in",    "44in"}, // E

    { "37mm",    "52mm"}, // A9
    { "52mm",    "74mm"}, // A8
    { "74mm",   "105mm"}, // A7
    {"105mm",   "148mm"}, // A6
    {"148mm",   "297mm"}, // A5
    {"210mm",   "297mm"}, // A4
    {"297mm",   "420mm"}, // A3
    {"420mm",   "594mm"}, // A2
    {"594mm",   "841mm"}, // A1
    {"841mm",  "1189mm"}, // A0

    {  "32mm",    "45mm"}, // B10
    {  "45mm",    "64mm"}, // B9
    {  "64mm",    "91mm"}, // B8
    {  "91mm",   "128mm"}, // B7
    { "128mm",   "182mm"}, // B6
    { "182mm",   "257mm"}, // B5
    { "257mm",   "364mm"}, // B4
    { "364mm",   "515mm"}, // B3
    { "515mm",   "728mm"}, // B2
    { "728mm",  "1030mm"}, // B1
    {"1030mm",  "1456mm"}  // B0
};


// Metric conversion
// 1 point = 1/72 inches
static const double inchtoPtFactor = 72;
// TODO: check that support for both inch and metric system works

static inline
double
ptUnit( double x )
{
    return x * inchtoPtFactor;
}

static inline
double
odfCornerRadius( qint32 xfigRadius )
{
    return ptUnit( static_cast<double>(xfigRadius) / 80);
}

static inline
double
odfLineThickness( qint32 xfigLineThickness )
{
    return ptUnit( static_cast<double>(xfigLineThickness) / 80);
}

double
XFigOdgWriter::odfLength( qint32 length ) const
{
    return ptUnit( static_cast<double>(length) / m_Document->resolution() );
}

double
XFigOdgWriter::odfXCoord( qint32 x ) const
{
    return ptUnit( static_cast<double>(x) / m_Document->resolution() );
}

double
XFigOdgWriter::odfYCoord( qint32 y ) const
{
    return ptUnit( static_cast<double>(y) / m_Document->resolution() );
}



XFigOdgWriter::XFigOdgWriter( KoStore* outputStore )
  : m_CLocale(QLocale::c())
  , m_OdfWriteStore( outputStore )
  , m_OutputStore( outputStore )
  , m_PageCount( 0 )
{
    m_CLocale.setNumberOptions(QLocale::OmitGroupSeparator);
    m_ManifestWriter = m_OdfWriteStore.manifestWriter( KoOdf::mimeType(KoOdf::Graphics) );
}

XFigOdgWriter::~XFigOdgWriter()
{
    m_OdfWriteStore.closeManifestWriter();

    delete m_OutputStore;
}

bool
XFigOdgWriter::write( XFigDocument* document )
{
    m_Document = document;

    storePixelImageFiles();

    // Create content.xml
    storeContentXml();

    // Create the styles.xml file
    m_StyleCollector.saveOdfStylesDotXml( m_OutputStore, m_ManifestWriter );

    // Create meta.xml
    storeMetaXml();

    return true;
}

void
XFigOdgWriter::storePixelImageFiles()
{
    // TODO: store pixel files linked from XFigPictureBoxObject
#if 0
    // TODO: as mManifestWriter needs full rel path, perhaps drop enterDirectory/leaveDirectory
    mOutputStore->enterDirectory( QLatin1String("Pictures") );

    foreach( const XFigPixelImage* image, mDocument->pixelImages() )
    {
        mOutputStore->open( fileName );
        mOutputStore->write( *image );
        mOutputStore->close();

        mManifestWriter->addManifestEntry( filePath, mediaType );
    }

    mOutputStore->leaveDirectory();
#endif
}


void
XFigOdgWriter::storeMetaXml()
{
    KoDocumentInfo documentInfo;
    documentInfo.setOriginalGenerator(QLatin1String("Calligra XFig filter"));
    documentInfo.setAboutInfo(QLatin1String("comments"), m_Document->comment());

    const QString documentInfoFilePath = QLatin1String( "meta.xml" );

    m_OutputStore->open( documentInfoFilePath );
    documentInfo.saveOasis( m_OutputStore );
    m_OutputStore->close();

    // TODO: "text/xml" could be a static string
    m_ManifestWriter->addManifestEntry( documentInfoFilePath, QLatin1String("text/xml") );
}

void
XFigOdgWriter::storeContentXml()
{
    KoXmlWriter* contentWriter = m_OdfWriteStore.contentWriter();
    m_BodyWriter = m_OdfWriteStore.bodyWriter();

    m_BodyWriter->startElement( "office:body" );
    m_BodyWriter->startElement( KoOdf::bodyContentElement(KoOdf::Graphics, true));

    writeMasterPage();

    foreach( const XFigPage* page, m_Document->pages() )
        writePage( page );

    m_BodyWriter->endElement(); //office:drawing
    m_BodyWriter->endElement(); //office:body
    m_BodyWriter->endDocument();

    m_StyleCollector.saveOdfStyles( KoGenStyles::DocumentAutomaticStyles, contentWriter );

    m_OdfWriteStore.closeContentWriter();

    // TODO: mOdfWriteStore.closeContentWriter() should do that, or? also "text/xml" could be a static string
    m_ManifestWriter->addManifestEntry( QLatin1String("content.xml"), QLatin1String("text/xml") );
}


void
XFigOdgWriter::writeMasterPage()
{
    KoGenStyle masterPageStyle( KoGenStyle::MasterPageStyle );

    KoGenStyle masterPageLayoutStyle( KoGenStyle::PageLayoutStyle );
    masterPageLayoutStyle.setAutoStyleInStylesDotXml( true );

    if (m_Document->pageSizeType() != XFigPageSizeUnknown) {
        const PageSize& pageSize = pageSizeTable[m_Document->pageSizeType()-1];

        // defaults to portrait in case orientation is unknown
        const bool isLandscape = (m_Document->pageOrientation() == XFigPageLandscape);
        masterPageLayoutStyle.addProperty( QLatin1String("fo:page-width"),
                                           isLandscape ? pageSize.height : pageSize.width );
        masterPageLayoutStyle.addProperty( QLatin1String("fo:page-height"),
                                           isLandscape ? pageSize.width : pageSize.height );
    }

    const QString masterPageLayoutStyleName =
        m_StyleCollector.insert( masterPageLayoutStyle, QLatin1String("masterPageLayoutStyle") );

    masterPageStyle.addAttribute( QLatin1String("style:page-layout-name"), masterPageLayoutStyleName );

    KoGenStyle drawingMasterPageStyle( KoGenStyle::DrawingPageStyle, "drawing-page" );
    drawingMasterPageStyle.setAutoStyleInStylesDotXml( true );

    drawingMasterPageStyle.addProperty( QLatin1String("draw:fill"), "none" );

    const QString drawingMasterPageStyleName =
        m_StyleCollector.insert( drawingMasterPageStyle, QLatin1String("drawingMasterPageStyle") );

    masterPageStyle.addAttribute( QLatin1String("draw:style-name"), drawingMasterPageStyleName );

    m_MasterPageStyleName =
        m_StyleCollector.insert( masterPageStyle, QLatin1String("masterPageStyle") );
}

void
XFigOdgWriter::writePage( const XFigPage* page )
{
    m_BodyWriter->startElement( "draw:page" );

    m_BodyWriter->addAttribute( "xml:id", QLatin1String("page")+QString::number(m_PageCount++) );
    m_BodyWriter->addAttribute( "draw:master-page-name", m_MasterPageStyleName );

    // objects
    foreach( const XFigAbstractObject* object, page->objects() ) {
        writeObject( object );
    }

    m_BodyWriter->endElement(); //draw:page
}

void
XFigOdgWriter::writeObject( const XFigAbstractObject* object )
{
    const XFigAbstractObject::TypeId typeId = object->typeId();

    if (typeId == XFigAbstractObject::EllipseId)
        writeEllipseObject( static_cast<const XFigEllipseObject*>(object) );
    else if (typeId == XFigAbstractObject::PolylineId)
        writePolylineObject( static_cast<const XFigPolylineObject*>(object) );
    else if (typeId == XFigAbstractObject::PolygonId)
        writePolygonObject( static_cast<const XFigPolygonObject*>(object) );
    else if (typeId == XFigAbstractObject::BoxId)
        writeBoxObject( static_cast<const XFigBoxObject*>(object) );
    else if (typeId == XFigAbstractObject::PictureBoxId)
        writePictureBoxObject( static_cast<const XFigPictureBoxObject*>(object) );
    else if (typeId == XFigAbstractObject::SplineId)
        writeSplineObject( static_cast<const XFigSplineObject*>(object) );
    else if (typeId == XFigAbstractObject::ArcId)
        writeArcObject( static_cast<const XFigArcObject*>(object) );
    else if (typeId == XFigAbstractObject::TextId)
        writeTextObject( static_cast<const XFigTextObject*>(object) );
    else if( typeId == XFigAbstractObject::CompoundId )
        writeCompoundObject( static_cast<const XFigCompoundObject*>(object) );
}

void
XFigOdgWriter::writeCompoundObject( const XFigCompoundObject* groupObject )
{
// for now compounds are not written as groups, as in OpenDocument groups
// are kind of sublayers with an automatically assigned z-index
// while in XFig objects from different compounds can be intertwined in the z-order
// losing the grouping seems the less tragic solution over wrong order of objects
// TODO: find if a compound really needs to be dissolved, or if it does not overlap with others
//     mBodyWriter->startElement("draw:g");

//     writeComment(groupObject);

    foreach( const XFigAbstractObject* object, groupObject->objects() ) {
        writeObject( object );
    }

//     mBodyWriter->endElement(); //draw:g
}

void
XFigOdgWriter::writeEllipseObject(const XFigEllipseObject* ellipseObject)
{
    m_BodyWriter->startElement("draw:ellipse");

    writeZIndex(ellipseObject);

    const XFigPoint centerPoint = ellipseObject->centerPoint();
    m_BodyWriter->addAttribute("svg:cx", "0pt");
    m_BodyWriter->addAttribute("svg:cy", "0pt");
    m_BodyWriter->addAttributePt("svg:rx", odfLength(ellipseObject->xRadius()));
    m_BodyWriter->addAttributePt("svg:ry", odfLength(ellipseObject->yRadius()));

    const QString transformationString =
        QLatin1String("rotate(") + m_CLocale.toString(ellipseObject->xAxisAngle()) +
        QLatin1String(")translate(") +
        m_CLocale.toString(odfXCoord(centerPoint.x())) + QLatin1String("pt ") +
        m_CLocale.toString(odfYCoord(centerPoint.y())) + QLatin1String("pt)");
    m_BodyWriter->addAttribute("draw:transform", transformationString);

    KoGenStyle ellipseStyle(KoGenStyle::GraphicAutoStyle, "graphic");
    writeStroke(ellipseStyle, ellipseObject );
    writeFill(ellipseStyle, ellipseObject );
    const QString ellipseStyleName = m_StyleCollector.insert(ellipseStyle, QLatin1String("ellipseStyle"));
    m_BodyWriter->addAttribute("draw:style-name", ellipseStyleName);

    writeComment(ellipseObject);

    m_BodyWriter->endElement(); // draw:ellipse
}

void
XFigOdgWriter::writePolylineObject(const XFigPolylineObject* polylineObject)
{
    m_BodyWriter->startElement("draw:polyline");

    writeZIndex(polylineObject);

    writePoints(polylineObject->points());

    KoGenStyle polylineStyle(KoGenStyle::GraphicAutoStyle, "graphic");
    writeStroke(polylineStyle, polylineObject);
    writeFill(polylineStyle, polylineObject);
    writeJoinType(polylineStyle, polylineObject->joinType());
    writeCapType(polylineStyle, polylineObject->capType());
    writeArrow(polylineStyle, polylineObject->backwardArrow(), LineStart);
    writeArrow(polylineStyle, polylineObject->forwardArrow(), LineEnd);
    const QString polylineStyleName =
        m_StyleCollector.insert(polylineStyle, QLatin1String("polylineStyle"));
    m_BodyWriter->addAttribute("draw:style-name", polylineStyleName);

    writeComment(polylineObject);

    m_BodyWriter->endElement(); // draw:polyline
}

void
XFigOdgWriter::writePolygonObject( const XFigPolygonObject* polygonObject )
{
    m_BodyWriter->startElement("draw:polygon");

    writeZIndex(polygonObject);

    writePoints(polygonObject->points());

    KoGenStyle polygonStyle(KoGenStyle::GraphicAutoStyle, "graphic");
    writeStroke(polygonStyle, polygonObject);
    writeFill(polygonStyle, polygonObject);
    writeJoinType(polygonStyle, polygonObject->joinType());
    const QString polygonStyleName =
        m_StyleCollector.insert(polygonStyle, QLatin1String("polygonStyle"));
    m_BodyWriter->addAttribute("draw:style-name", polygonStyleName);

    writeComment(polygonObject);

    m_BodyWriter->endElement(); // draw:polygon
}

void
XFigOdgWriter::writeBoxObject( const XFigBoxObject* boxObject )
{
    m_BodyWriter->startElement("draw:rect");

    writeZIndex( boxObject );

    const XFigPoint upperleft = boxObject->upperLeft();
    m_BodyWriter->addAttributePt("svg:x", odfXCoord(upperleft.x()));
    m_BodyWriter->addAttributePt("svg:y", odfYCoord(upperleft.y()));
    m_BodyWriter->addAttributePt("svg:width", odfLength(boxObject->width()));
    m_BodyWriter->addAttributePt("svg:height", odfLength(boxObject->height()));

    const qint32 radius = boxObject->radius();
    if (radius != 0) {
        const double odfRadius = odfCornerRadius(radius);
        m_BodyWriter->addAttributePt("svg:rx", odfRadius);
        m_BodyWriter->addAttributePt("svg:ry", odfRadius);
    }

    {
        KoGenStyle boxStyle(KoGenStyle::GraphicAutoStyle, "graphic");
        writeStroke(boxStyle, boxObject);
        writeFill(boxStyle, boxObject);
        writeJoinType(boxStyle, boxObject->joinType());
        const QString boxStyleName = m_StyleCollector.insert(boxStyle, QLatin1String("boxStyle"));
        m_BodyWriter->addAttribute("draw:style-name", boxStyleName);
    }

    writeComment(boxObject);

    m_BodyWriter->endElement(); // draw:rect
}

void
XFigOdgWriter::writePictureBoxObject( const XFigPictureBoxObject* /*pictureBoxObject*/ )
{
    // TODO
}

void
XFigOdgWriter::writeSplineObject( const XFigSplineObject* /*object*/ )
{
}

void
XFigOdgWriter::writeArcObject( const XFigArcObject* /*object*/ )
{
    // TODO
}


void
XFigOdgWriter::writeTextObject( const XFigTextObject* textObject )
{
    m_BodyWriter->startElement("draw:frame");

    writeZIndex(textObject);

    const double length = odfLength(textObject->length()) * 1.3; // 1.3 to adapt to wider fonts being used
    const double height = odfLength(textObject->height())*1.3;

    const XFigTextAlignment alignment = textObject->textAlignment();
    const XFigPoint point = textObject->baselineStartPoint();
    double xCoord = odfXCoord(point.x());
    if (alignment == XFigTextCenterAligned) {
        xCoord -= length * 0.5;
    } else if (alignment == XFigTextRightAligned) {
        xCoord -= length;
    }
    // given point is at baseline, with height the ascend of the font
    double yCoord = odfYCoord(point.y() - textObject->height());

    m_BodyWriter->addAttribute("svg:x", "0pt");
    m_BodyWriter->addAttribute("svg:y", "0pt");
    m_BodyWriter->addAttributePt("svg:width", length);
    m_BodyWriter->addAttributePt("svg:height", height);
    const QString transformationString =
        QLatin1String("rotate(") + m_CLocale.toString(textObject->xAxisAngle()) +
        QLatin1String(")translate(") +
        m_CLocale.toString(xCoord) + QLatin1String("pt ") +
        m_CLocale.toString(yCoord) + QLatin1String("pt)");
    m_BodyWriter->addAttribute("draw:transform", transformationString);

    KoGenStyle frameStyle( KoGenStyle::GraphicAutoStyle, "graphic" );
    frameStyle.addProperty( QLatin1String("style:overflow-behavior"), "clip" );
    const QString frameStyleName =
        m_StyleCollector.insert( frameStyle, QLatin1String("frameStyle") );
    m_BodyWriter->addAttribute( "draw:style-name", frameStyleName );

    m_BodyWriter->startElement("draw:text-box");

    m_BodyWriter->startElement( "text:p", false );  //false: we should not indent the inner tags

    KoGenStyle paragraphStyle( KoGenStyle::ParagraphAutoStyle, "paragraph" );
    writeParagraphStyle( paragraphStyle, textObject );

    const QString paragraphStyleName =
        m_StyleCollector.insert( paragraphStyle, QLatin1String("paragraphStyle") );
    m_BodyWriter->addAttribute( "text:style-name", paragraphStyleName );

    m_BodyWriter->startElement( "text:span" );

    KoGenStyle textSpanStyle( KoGenStyle::TextAutoStyle, "text" );
    writeFont( textSpanStyle, textObject );

    const QString textSpanStyleName =
        m_StyleCollector.insert( textSpanStyle, QLatin1String("textSpanStyle") );
    m_BodyWriter->addAttribute( "text:style-name", textSpanStyleName );

    m_BodyWriter->addTextNode( textObject->text() );

    m_BodyWriter->endElement(); //text:span
    m_BodyWriter->endElement(); //text:p

    m_BodyWriter->endElement();//draw:text-box

    writeComment(textObject);

    m_BodyWriter->endElement();//draw:frame
}

void
XFigOdgWriter::writeZIndex( const XFigAbstractGraphObject* graphObject )
{
    m_BodyWriter->addAttribute( "draw:z-index", (1000-graphObject->depth()) );
}

void
XFigOdgWriter::writePoints( const QVector<XFigPoint>& points )
{
    const XFigPoint& firstPoint = points.at(0);

    XFigCoord minX = firstPoint.x();
    XFigCoord minY = firstPoint.y();
    XFigCoord maxX = firstPoint.x();
    XFigCoord maxY = firstPoint.y();

    QString pointsString;
    int i = 0;
    while (true) {
        const XFigPoint& point = points.at(i);
        const XFigCoord x = point.x();
        const XFigCoord y = point.y();

        if( x < minX ) {
            minX = x;
        } else if( maxX < x ) {
            maxX = x;
        }
        if( y < minY ) {
            minY = y;
        } else if( maxY < y ) {
            maxY = y;
        }

        pointsString +=  m_CLocale.toString(x)+QLatin1Char(',')+m_CLocale.toString(y);
        ++i;
        if (i >= points.count()) {
            break;
        }
        pointsString += QLatin1Char(' ');
    }
    const XFigCoord width =  maxX - minX + 1;
    const XFigCoord height = maxY - minY + 1;
    const QString viewBoxString =
        QString::number(minX) + QLatin1Char(' ') + QString::number(minY) + QLatin1Char(' ') +
        QString::number(width) + QLatin1Char(' ') + QString::number(height);

    m_BodyWriter->addAttributePt("svg:x", odfXCoord(minX));
    m_BodyWriter->addAttributePt("svg:y", odfYCoord(minY));
    m_BodyWriter->addAttributePt("svg:width", odfLength(width) );
    m_BodyWriter->addAttributePt("svg:height", odfLength(height) );
    m_BodyWriter->addAttribute("svg:viewBox", viewBoxString);
    m_BodyWriter->addAttribute("draw:points", pointsString );
}

void XFigOdgWriter::writeComment(const XFigAbstractObject* object)
{
    const QString& comment = object->comment();
    if (comment.isEmpty()) {
        return;
    }

    m_BodyWriter->startElement("svg:desc");
    m_BodyWriter->addTextNode(comment);
    m_BodyWriter->endElement(); // svg:desc
}

void
XFigOdgWriter::writeFill( KoGenStyle& odfStyle, const XFigFillable* fillable )
{
    // TODO: support for fill patterns not yet done, mapping to solid fill for now
    const qint32 fillStyleId = (fillable->fillStyleId() > 20) ? 20 : fillable->fillStyleId();

    const bool isNotFilled = ( fillStyleId == -1 );

    odfStyle.addProperty( QLatin1String("draw:fill"), isNotFilled ? "none" : "solid" );

    if (! isNotFilled) {
        const qint32 fillColorId = fillable->fillColorId();

        QString colorString;
        // BLACK or DEFAULT color?
        if (fillColorId < 1) {
            // 0: white, 20: black, 1..19 shades of grey, from lighter to darker
            const int value = qRound((20 - fillStyleId) * 255.0 / 20.0);
            colorString = QColor(value, value, value).name();
        // WHITE color ?
        } else if (fillColorId == 7) {
            // 0: black, 20: white, 1..19 shades of grey, from darker to lighter
            const int value = qRound(fillStyleId * 255.0 / 20.0);
            colorString = QColor(value, value, value).name();
        } else {
            //TODO: tint blackness/whiteness of color
            const QColor* color = m_Document->color(fillColorId);
            if (color != 0) {
                colorString = color->name();
            }
        }

        odfStyle.addProperty( QLatin1String("draw:fill-color"), colorString );
    }
}

void
XFigOdgWriter::writeDotDash( KoGenStyle& odfStyle, int lineType, double distance )
{
    const double odfDistance = odfLineThickness(distance);
    odfStyle.addAttribute(QLatin1String("draw:style"), "rect");
    odfStyle.addAttribute(QLatin1String("draw:distance"),
                          m_CLocale.toString(odfDistance)+QLatin1String("pt"));

    const char* displayName = 0;
    bool isFirstDot = false;
    const char* secondDotsNumber = 0;

    switch (lineType) {
    case XFigLineDashed:
        displayName = "Dashed";
        break;
    case XFigLineDotted:
        displayName = "Dotted";
        isFirstDot = true;
        break;
    case XFigLineDashDotted:
        displayName = "1 Dot 1 Dash";
        secondDotsNumber = "1";
        break;
    case XFigLineDashDoubleDotted:
        displayName = "1 Dash 2 Dots";
        secondDotsNumber = "2";
        break;
    case XFigLineDashTrippleDotted:
        displayName = "1 Dash 3 Dots";
        secondDotsNumber = "3";
        break;
    }

    odfStyle.addAttribute(QLatin1String("draw:display-name"), displayName);
    odfStyle.addAttribute(QLatin1String("draw:dots1"), "1");
    odfStyle.addAttribute(QLatin1String("draw:dots1-length"),
                          isFirstDot ? QString::fromLatin1("100%") :
                                       m_CLocale.toString(odfDistance)+QLatin1String("pt"));
    if (secondDotsNumber!=0) {
        odfStyle.addAttribute(QLatin1String("draw:dots2"), QLatin1String(secondDotsNumber));
        odfStyle.addAttribute(QLatin1String("draw:dots2-length"), "100%");
    }
}

void
XFigOdgWriter::writeStroke( KoGenStyle& odfStyle, const XFigLineable* lineable )
{
    const qint32 colorId = lineable->lineColorId();
    if (colorId >= 0) {
        const QColor* color = m_Document->color(colorId);
        if (color != 0) {
            odfStyle.addProperty( QLatin1String("svg:stroke-color"), color->name() );
        }
    }

    odfStyle.addPropertyPt( QLatin1String("svg:stroke-width"), odfLineThickness(lineable->lineThickness()) );

    const XFigLineType lineType = lineable->lineType();
    const bool isDashed = (lineType != XFigLineSolid) && (lineType != XFigLineDefault);

    odfStyle.addProperty( QLatin1String("draw:stroke"), (isDashed) ? "dash" : "solid" );

    if (isDashed) {
        KoGenStyle dashStyle(KoGenStyle::StrokeDashStyle);
        writeDotDash(dashStyle, lineType, lineable->lineStyleValue());
        const QString dashStyleName = m_StyleCollector.insert( dashStyle, QLatin1String("dashStyle") );

        odfStyle.addProperty(QLatin1String("draw:stroke-dash"), dashStyleName);
    }
}

void
XFigOdgWriter::writeJoinType(KoGenStyle& odfStyle, int joinType)
{
    const char* const linejoin =
        (joinType == XFigJoinRound) ?  "round" :
        (joinType == XFigJoinBevel) ?  "bevel" :
                                       "miter";
    odfStyle.addProperty(QLatin1String("draw:stroke-linejoin"), linejoin);
}

void
XFigOdgWriter::writeCapType(KoGenStyle& odfStyle, int capType)
{
    const char* const linecap =
        (capType == XFigCapRound) ?      "round" :
        (capType == XFigCapProjecting) ? "square" :
                                         "butt";
    odfStyle.addProperty(QLatin1String("svg:stroke-linecap"), linecap);
}

void
XFigOdgWriter::writeArrow(KoGenStyle& odfStyle, const XFigArrowHead* arrow, LineEndType lineEndType)
{
    if (arrow != 0) {
        KoGenStyle arrowStyle(KoGenStyle::MarkerStyle);
        // TODO: support all arrow types. this is just a tmp. substitute copied from kpr2odf
        const char* const displayName = "Line Arrow";
        const char* const viewBox = "0 0 1122 2243";
        const char* const d = "m0 2108v17 17l12 42 30 34 38 21 43 4 29-8 30-21 25-26 13-34 343-1532 339 1520 13 42 29 34 39 21 42 4 42-12 34-30 21-42v-39-12l-4 4-440-1998-9-42-25-39-38-25-43-8-42 8-38 25-26 39-8 42z";

        arrowStyle.addAttribute(QLatin1String("draw:display-name"), displayName);
        arrowStyle.addAttribute(QLatin1String("svg:viewBox"), viewBox);
        arrowStyle.addAttribute(QLatin1String("svg:d"), d);
        const QString arrowStyleName =
            m_StyleCollector.insert(arrowStyle, QLatin1String("arrowStyle"));

        const char* const markerStart =
            (lineEndType==LineStart) ? "draw:marker-start" : "draw:marker-end";
        const char* const markerStartWidth =
            (lineEndType==LineStart) ? "draw:marker-start-width" : "draw:marker-end-width";
        odfStyle.addProperty(QLatin1String(markerStart), arrowStyleName);
        odfStyle.addPropertyPt(QLatin1String(markerStartWidth), odfLineThickness(arrow->thickness()));
    }
}

void
XFigOdgWriter::writeFont( KoGenStyle& odfStyle, const XFigTextObject* textObject )
{
    const XFigFontData& fontData = textObject->fontData();

    odfStyle.addPropertyPt(QLatin1String("fo:font-size"), fontData.mSize);
    const char* const weight =
        (fontData.mWeight == QFont::Bold) ?     "bold" :
        (fontData.mWeight == QFont::DemiBold) ? "600" :
                                                "normal";
    odfStyle.addProperty(QLatin1String("fo:font-weight"), weight);
    const char* const style =
        (fontData.mStyle == QFont::StyleItalic) ?  "italic" :
        (fontData.mStyle == QFont::StyleOblique) ? "oblique" :
                                                   "normal";
    odfStyle.addProperty(QLatin1String("fo:font-style"), style);
    if (! fontData.mFamily.isEmpty())
        odfStyle.addProperty( QLatin1String("fo:font-family"), fontData.mFamily );
}


void
XFigOdgWriter::writeParagraphStyle( KoGenStyle& odfStyle, const XFigTextObject* textObject )
{
    const XFigTextAlignment textAlignment = textObject->textAlignment();
    const char* const alignmentName =
        (textAlignment == XFigTextCenterAligned) ? "center" :
        (textAlignment == XFigTextRightAligned) ?  "right" :
        /* XFigTextLeftAligned */                  "left";
    odfStyle.addProperty( QLatin1String("fo:text-align"), QLatin1String(alignmentName) );

    odfStyle.addProperty(QLatin1String("fo:margin"), "0pt");
    odfStyle.addProperty(QLatin1String("fo:padding"), "0pt");
}
