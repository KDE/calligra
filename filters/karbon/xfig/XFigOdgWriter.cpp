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
XFigOdgWriter::odfLength( double length ) const
{
    return ptUnit( length / m_Document->resolution() );
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

    m_OutputStore->open(documentInfoFilePath);
    documentInfo.saveOasis(m_OutputStore);
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
    writeFill(ellipseStyle, ellipseObject, ellipseObject->lineColorId());
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
    writeFill(polylineStyle, polylineObject, polylineObject->lineColorId());
    writeJoinType(polylineStyle, polylineObject->joinType());
    writeCapType(polylineStyle, polylineObject);
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
    writeFill(polygonStyle, polygonObject, polygonObject->lineColorId());
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
        writeFill(boxStyle, boxObject, boxObject->lineColorId());
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

void XFigOdgWriter::writeArcObject( const XFigArcObject* arcObject )
{
    const XFigPoint centerPoint = arcObject->centerPoint();
    const XFigPoint point1 = arcObject->point1();
    const XFigPoint point3 = arcObject->point3();

    const XFigCoord diffX1 = point1.x() - centerPoint.x();
    const XFigCoord diffY1 = point1.y() - centerPoint.y();
    const XFigCoord diffX3 = point3.x() - centerPoint.x();
    const XFigCoord diffY3 = point3.y() - centerPoint.y();

    double startAngle = -atan2( (qreal)diffY1, diffX1 ) * 180.0/M_PI;
    double endAngle   = -atan2( (qreal)diffY3, diffX3 ) * 180.0/M_PI;
    if (arcObject->direction() == XFigArcObject::Clockwise) {
        const double helper = startAngle;
        startAngle = endAngle;
        endAngle = helper;
    }
    const double radius = qSqrt((diffX1*diffX1) + (diffY1*diffY1));

    m_BodyWriter->startElement("draw:circle");

    writeZIndex( arcObject );

    m_BodyWriter->addAttributePt("svg:cx", odfXCoord(centerPoint.x()));
    m_BodyWriter->addAttributePt("svg:cy", odfXCoord(centerPoint.y()));
    m_BodyWriter->addAttributePt("svg:r", odfLength(radius));
    m_BodyWriter->addAttribute("draw:start-angle", startAngle);
    m_BodyWriter->addAttribute("draw:end-angle", endAngle);

    // TODO: cut in XFig has no line on the cut side, only on the curve
    const char* kind =
        (arcObject->subtype() == XFigArcObject::PieWedgeClosed) ? "section" :
        (arcObject->fillType() != XFigFillNone ) ?                "cut" :
                                                                  "arc";
    m_BodyWriter->addAttribute("draw:kind", kind);

    KoGenStyle arcStyle(KoGenStyle::GraphicAutoStyle, "graphic");
    writeStroke(arcStyle, arcObject);
    writeFill(arcStyle, arcObject, arcObject->lineColorId());
    writeCapType(arcStyle, arcObject);
    writeArrow(arcStyle, arcObject->backwardArrow(),
               (arcObject->direction() == XFigArcObject::Clockwise)?LineEnd:LineStart);
    writeArrow(arcStyle, arcObject->forwardArrow(),
               (arcObject->direction() == XFigArcObject::Clockwise)?LineStart:LineEnd);
    const QString arcStyleName = m_StyleCollector.insert(arcStyle, QLatin1String("arcStyle"));
    m_BodyWriter->addAttribute("draw:style-name", arcStyleName);

    writeComment(arcObject);

    m_BodyWriter->endElement(); // draw:circle
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
XFigOdgWriter::writeFill(KoGenStyle& odfStyle, const XFigFillable* fillable, qint32 penColorId)
{
    const XFigFillType fillType = fillable->fillType();

    const char* const fillString =
        (fillType == XFigFillSolid) ?   "solid" :
        (fillType == XFigFillPattern) ? "hatch" :
        /*(fillType == XFigFillNone)*/  "none";
    odfStyle.addProperty(QLatin1String("draw:fill"), fillString);

    if (fillType != XFigFillNone) {
        const qint32 fillColorId = fillable->fillColorId();

        QString colorString;
        if (fillType == XFigFillSolid) {
            // BLACK or DEFAULT color?
            if (fillColorId < 1) {
                // 0: white, 20: black, 1..19 shades of grey, from lighter to darker
                const int value = qRound((20 - fillable->fillTinting()) * 255.0 / 20.0);
                colorString = QColor(value, value, value).name();
            // WHITE color ?
            } else if (fillColorId == 7) {
                // 0: black, 20: white, 1..19 shades of grey, from darker to lighter
                const int value = qRound(fillable->fillTinting() * 255.0 / 20.0);
                colorString = QColor(value, value, value).name();
            } else {
                //TODO: tint blackness/whiteness of color
                const QColor* const color = m_Document->color(fillColorId);
                if (color != 0) {
                    colorString = color->name();
                }
            }

            odfStyle.addProperty(QLatin1String("draw:fill-color"), colorString);
        } else {
            // ODF 1.2 does not support a hatch pattern with a background
            // Options are to
            // * just do the hatch (not all XFig ones can be mapped)
            // * just do the background
            // * use pixmaps (needs pixmaps created with given colors)
            // Decision for now: just do the hatch
            const QColor* const color = m_Document->color(penColorId);
            if (color != 0) {
                colorString = color->name();
            }

            writeHatch(odfStyle, fillable->fillPatternType(), colorString);
        }
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
XFigOdgWriter::writeCapType(KoGenStyle& odfStyle, const XFigLineEndable* lineEndable)
{
    const XFigCapType capType = lineEndable->capType();

    const char* const linecap =
        (capType == XFigCapRound) ?      "round" :
        (capType == XFigCapProjecting) ? "square" :
                                         "butt";
    odfStyle.addProperty(QLatin1String("svg:stroke-linecap"), linecap);
}

static const
struct ArrowData
{
    const char* displayName;
    const char* viewBox;
    const char* d;
} arrowDataList[13] =
{
    {   // 0
        "Arrowheads 7",//"Stick Arrow",
        "0 0 1122 2243",//"0 0 1 1",
        "m0 2108v17 17l12 42 30 34 38 21 43 4 29-8 30-21 25-26 13-34 343-1532 339 1520 13 42 29 34 39 21 42 4 42-12 34-30 21-42v-39-12l-4 4-440-1998-9-42-25-39-38-25-43-8-42 8-38 25-26 39-8 42z"//"m0,1 l0.5,-1 0.5,1"
    },
    {   // 1
        "Arrowheads 6",//"Triangle Arrow",
        "0 0 1131 902",//"0 0 1 1",
        "m564 0-564 902h1131z"//"m0,1 l0.5,-1 0.5,1 -1,0z"
    },
    {   // 2
        "Concave Spear Arrow",
        "0 0 1131 1580",//"0 0 1 1.25",
        "m1013 1491 118 89-567-1580-564 1580 114-85 136-68 148-46 161-17 161 13 153 46z"//"m0,1.25 l0.5,-1.25 0.5,1.25 -0.5,-0.25 -0.5,0.25z"
    },
    {   // 3
        "Convex Spear Arrow",
        "0 0 1 1",
        "m0,0.75 l0.5,-0.75 0.5,0.75 -0.5,0.25 -0.5,-0.25z"
    },
    {   // 4
        "Arrowheads 10",//"Diamond Arrow",
        "0 0 1131 1131",//"0 0 1 1",
        "m0 564 564 567 567-567-567-564z",//"m0,0.5 l0.5,-0.5 0.5,0.5 -0.5,0.5 -0.5,-0.5z"
    },
    {   // 5
        "Arrowheads 9",//"Circle Arrow",
        "0 0 1131 1131",//"0 0 1 1",
        "m462 1118-102-29-102-51-93-72-72-93-51-102-29-102-13-105 13-102 29-106 51-102 72-89 93-72 102-50 102-34 106-9 101 9 106 34 98 50 93 72 72 89 51 102 29 106 13 102-13 105-29 102-51 102-72 93-93 72-98 51-106 29-101 13z",//"m0,0.5 a0.5,0.5 0 1,1 0,0.5z"
    },
    {   // 6
        "Half Circle Arrow",
        "0 0 1 1",
        "a0.5,0.5 0 1,0 1,0z"
    },
    {   // 7
        "Arrowheads 1",//"Square Arrow",
        "0 0 10 10",
        "m0 0h10v10h-10z",
    },
    {   // 8
        "Reverse Triangle Arrow",
        "0 0 1 1",
        "l0.5,1 0.5,-1 -1,0z"
    },
    {   // 9
        "Wye Arrow",
        "0 0 1 1",
        "l0.5,1 0.5,-1" // TODO
    },
    {   // 10
        "Arrowheads 3",//"Bar Arrow",
        "0 0 836 110",
        "m0 0h278 278 280v36 36 38h-278-278-280v-36-36z",
    },
    {   // 11
        "Two Prong Fork Arrow",
        "0 0 1 1",
        "l0,1 1,0 0,-1" // TODO
    },
    {   // 12
        "Reverse Two Prong Fork Arrow",
        "0 0 1 1",
        "m0,1 l0,-1 1,0 0,1" // TODO
    }
};

// ODF seems to miss support for:
// * non-centric arrows
// * stroke-based arrows
// * scaling by length, not just width
// Non-centric arrows are for now simply mapped to the similar centric ones.
// TODO: calculate hollow/stroke arrow data; scale all with arrow->length()
static const int arrowDataMap[XFigArrowHeadTypeCount] =
{
    0, // XFigArrowHeadStick
    1, //XFigArrowHeadHollowTriangle
    1, //XFigArrowHeadFilledTriangle
    2, //XFigArrowHeadHollowConcaveSpear
    2, //XFigArrowHeadFilledConcaveSpear
    3, //XFigArrowHeadHollowConvexSpear
    3, //XFigArrowHeadFilledConvexSpear
    4, //XFigArrowHeadHollowDiamond
    4, //XFigArrowHeadFilledDiamond
    5, //XFigArrowHeadHollowCircle
    5, //XFigArrowHeadFilledCircle
    6, //XFigArrowHeadHollowHalfCircle
    6, //XFigArrowHeadFilledHalfCircle
    7, //XFigArrowHeadHollowSquare
    7, //XFigArrowHeadFilledSquare
    8, //XFigArrowHeadHollowReverseTriangle
    8, //XFigArrowHeadFilledReverseTriangle
    2, //XFigArrowHeadTopHalfFilledConcaveSpear
    2, //XFigArrowHeadBottomHalfFilledConcaveSpear
    1, //XFigArrowHeadHollowTopHalfTriangle
    1, //XFigArrowHeadFilledTopHalfTriangle
    2, //XFigArrowHeadHollowTopHalfConcaveSpear
    2, //XFigArrowHeadFilledTopHalfConcaveSpear
    3, //XFigArrowHeadHollowTopHalfConvexSpear
    3, //XFigArrowHeadFilledTopHalfConvexSpear
    8, //XFigArrowHeadWye
    10, //XFigArrowHeadBar
    1, //XFigArrowHeadTwoProngFork
    1 //XFigArrowHeadReverseTwoProngFork
};

void XFigOdgWriter::writeArrow(KoGenStyle& odfStyle, const XFigArrowHead* arrow, LineEndType lineEndType)
{
    if (arrow == 0) {
        return;
    }

    KoGenStyle arrowStyle(KoGenStyle::MarkerStyle);
    const ArrowData& arrowData = arrowDataList[arrowDataMap[arrow->type()]];
    arrowStyle.addAttribute(QLatin1String("draw:display-name"), arrowData.displayName);
    arrowStyle.addAttribute(QLatin1String("svg:viewBox"), arrowData.viewBox);
    arrowStyle.addAttribute(QLatin1String("svg:d"), arrowData.d);
    const QString arrowStyleName =
        m_StyleCollector.insert(arrowStyle, QLatin1String("arrowStyle"));

    const char* const marker =
        (lineEndType==LineStart) ? "draw:marker-start" : "draw:marker-end";
    const char* const markerWidth =
        (lineEndType==LineStart) ? "draw:marker-start-width" : "draw:marker-end-width";
    const char* const markerCenter =
        (lineEndType==LineStart) ? "draw:marker-start-center" : "draw:marker-end-center";
    odfStyle.addProperty(QLatin1String(marker), arrowStyleName);
    odfStyle.addPropertyPt(QLatin1String(markerWidth), odfLength(arrow->width()));
    odfStyle.addProperty(QLatin1String(markerCenter), "1.0");
}

void XFigOdgWriter::writeHatch(KoGenStyle& odfStyle, int patternType, const QString& colorString)
{
    KoGenStyle hatchStyle(KoGenStyle::HatchStyle);

    const char* displayName = 0;
    const char* style = 0;
    const char* distance = 0;
    const char* rotation = 0;

    // Shingles, bricks, tire treads cannot be done, are mapped to horizontal/vertical lines
    // Fish, circles, hexagons, octagons cannot be done, are mapped to cross hatchess
    // 30 degree crosshatch cannot be done, mapped to 45 degree one
    switch (patternType) {
    default:
    case XFigFillVerticalShinglesSkewedDown:
    case XFigFillVerticalShinglesSkewedUp:
    case XFigFillVerticalTireTreads:
    case XFigFillVerticalBricks:
    case XFigFillVerticalLines:
        displayName = " Vertical";
        style = "single";
        distance = "0.102cm";
        rotation = "900";
        break;
    case XFigFillHorizontalShinglesSkewedRight:
    case XFigFillHorizontalShinglesSkewedLeft:
    case XFigFillHorizontalTireTreads:
    case XFigFillHorizontalBricks:
    case XFigFillHorizontalLines:
        displayName = " Horizontal";
        style = "single";
        distance = "0.102cm";
        rotation = "0";
        break;
    case XFigFillRightDiagonal30Degree:
        displayName = " 30 Degrees";
        style = "single";
        distance = "0.102cm";
        rotation = "300";
        break;
    case XFigFillLeftDiagonal30Degree:
        displayName = " -30 Degrees";
        style = "single";
        distance = "0.102cm";
        rotation = "3300";
        break;
    case XFigFillRightDiagonal45Degree:
        displayName =  " 45 Degrees";
        style = "single";
        distance = "0.102cm";
        rotation = "450";
        break;
    case XFigFillLeftDiagonal45Degree:
        displayName = " -45 Degrees";
        style = "single";
        distance = "0.102cm";
        rotation = "3150";
        break;
    case XFigFillFishScales:
    case XFigFillSmallFishScales:
    case XFigFillCircles:
    case XFigFillHexagons:
    case XFigFillOctagons:
    case XFigFillCrossHatch:
        displayName = " Crossed 0 Degrees";
        style = "double";
        distance = "0.102cm";
        rotation = "900";
        break;
    case XFigFillCrossHatch30Degree:
    case XFigFillCrossHatch45Degree:
        displayName = " Crossed 45 Degrees";
        style = "double";
        distance = "0.102cm";
        rotation = "450";
        break;
    }

    hatchStyle.addAttribute("draw:display-name", colorString+QLatin1String(displayName));
    hatchStyle.addAttribute("draw:style", style);
    hatchStyle.addAttribute("draw:color", colorString);
    hatchStyle.addAttribute("draw:distance", distance);
    hatchStyle.addAttribute("draw:rotation", rotation);
    const QString hatchStyleName =
        m_StyleCollector.insert(hatchStyle, QLatin1String("hatchStyle"));

    odfStyle.addProperty("draw:fill-hatch-name", hatchStyleName);
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
