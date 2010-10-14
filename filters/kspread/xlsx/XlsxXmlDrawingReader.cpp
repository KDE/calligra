/*
 * This file is part of Office 2007 Filters for KOffice
 *
 * Copyright (C) 2010 Sebastian Sauer <sebsauer@kdab.com>
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include "XlsxXmlDrawingReader.h"
#include "XlsxXmlWorksheetReader.h"
#include "XlsxXmlWorksheetReader_p.h"
#include "XlsxXmlChartReader.h"
#include "XlsxImport.h"
#include "Charting.h"
#include "ChartExport.h"

#include <MsooXmlSchemas.h>
#include <MsooXmlUtils.h>
#include <MsooXmlRelationships.h>
#include <MsooXmlUnits.h>
#include <MsooXmlDiagramReader.h>

#include <KoXmlWriter.h>
#include <KoGenStyles.h>
#include <KoOdfGraphicStyles.h>
#include <KoUnit.h>
#include <QFontMetricsF>

#include <kspread/Util.h>

#define MSOOXML_CURRENT_NS "xdr"
#define MSOOXML_CURRENT_CLASS XlsxXmlDrawingReader
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS

#include <MsooXmlReader_p.h>
#include <MsooXmlUtils.h>
#include <MsooXmlContentTypes.h>
#include <MsooXmlRelationships.h>
#include <KoXmlWriter.h>

// calculates the column width in pixels
int columnWidth2(unsigned long col, unsigned long dx = 0, qreal defaultColumnWidth = 8.43) {
    QFont font("Arial", 10);
    QFontMetricsF fm(font);
    const qreal characterWidth = fm.width("h");
    defaultColumnWidth *= characterWidth;
    return (defaultColumnWidth * col) + (dx / 1024.0 * defaultColumnWidth);
}

// calculates the row height in pixels
int rowHeight2(unsigned long row, unsigned long dy = 0, qreal defaultRowHeight = 12.75)
{
    return defaultRowHeight * row + dy;
}

// Returns A for 1, B for 2, C for 3, etc.
QString columnName2(uint column)
{
    QString s;
    unsigned digits = 1;
    unsigned offset = 0;
    for (unsigned limit = 26; column >= limit + offset; limit *= 26, digits++)
        offset += limit;
    for (unsigned col = column - offset; digits; --digits, col /= 26)
        s.prepend(QChar('A' + (col % 26)));
    return s;
}

KoXmlWriter* XlsxDrawingObject::setShape(XlsxShape* shape)
{
    m_type = Shape;
    m_shape = shape;
    
    delete m_shapeBody;
    m_shapeBody = new KoXmlWriter(new QBuffer);
    return m_shapeBody;
}

void XlsxDrawingObject::save(KoXmlWriter* xmlWriter)
{
    switch(m_type) {
        case Unknown: {
            // nothing to do for us
        } break;
        case Chart: {
            m_chart->m_chartExport->saveIndex(xmlWriter);
        } break;
        case Diagram: {
            xmlWriter->startElement("draw:g");
            xmlWriter->addAttribute("draw:name", "SmartArt Shapes Group");
            xmlWriter->addAttribute("draw:z-index", "0");
            xmlWriter->addAttribute("table:end-cell-address", fromCellAddress());
            //xmlWriter->addAttribute("table:end-x", "0.6016in");
            //xmlWriter->addAttribute("table:end-y", "0.1339in");
            m_diagram->saveIndex(xmlWriter, positionRect());
            xmlWriter->endElement(); // draw:g
        } break;
        case Picture: {
            m_picture->saveXml(xmlWriter);
        } break;
        case Shape: {
            Q_ASSERT(m_shapeBody);
            QByteArray data = static_cast<QBuffer*>(m_shapeBody->device())->buffer().constData();
            xmlWriter->addCompleteElement(data);
            delete m_shapeBody;
            m_shapeBody = 0;
        } break;
    }
}

QRect XlsxDrawingObject::positionRect() const
{
    QRect rect(QPoint(0,0),QSize(0,0));
    if(m_positions.contains(FromAnchor)) {
        qreal defaultColumnWidth = 8.43;
        qreal defaultRowHeight = 12.75;

        Position f1 = m_positions[FromAnchor];
        rect.setX( columnWidth2(f1.m_col-1, 0 /*f.m_colOff*/, defaultColumnWidth) );
        rect.setY( rowHeight2(f1.m_row-1, 0 /*f.m_rowOff*/, defaultRowHeight) );
        if(m_positions.contains(ToAnchor)) {
            Position f2 = m_positions[ToAnchor];
            if(f2.m_col > 0 && f2.m_row > 0) {
                rect.setWidth( columnWidth2( f2.m_col - f1.m_col - 1, 0 /*t.m_colOff*/, defaultColumnWidth) );
                rect.setHeight( rowHeight2( f2.m_row - f1.m_row - 1, 0 /*t.m_rowOff*/, defaultRowHeight) );
            }
        }
    }
    return rect;
}

QString XlsxDrawingObject::cellAddress(const QString &sheetname, int row, int column) const
{
    QString result;
    if(!sheetname.isEmpty())
        result += sheetname + '.';
    result += columnName2(column) + QString::number(row);
    return result;
}

QString XlsxDrawingObject::fromCellAddress() const
{
    if(!m_positions.contains(FromAnchor)) return QString();
    Position f = m_positions[FromAnchor];
    return cellAddress(m_sheet->m_name, f.m_row, f.m_col);
}

QString XlsxDrawingObject::toCellAddress() const
{
    if(!m_positions.contains(ToAnchor)) return QString();
    Position f = m_positions[ToAnchor];
    return cellAddress(m_sheet->m_name, f.m_row, f.m_col);
}

XlsxXmlDrawingReaderContext::XlsxXmlDrawingReaderContext(XlsxXmlWorksheetReaderContext* _worksheetReaderContext, Sheet* _sheet, const QString& _path, const QString& _file)
    : MSOOXML::MsooXmlReaderContext(_worksheetReaderContext->relationships)
    , import(_worksheetReaderContext->import)
    , path(_path)
    , file(_file)
    , themes((_worksheetReaderContext->themes))
    , worksheetReaderContext(_worksheetReaderContext)
    , sheet(_sheet)
{
}

XlsxXmlDrawingReaderContext::~XlsxXmlDrawingReaderContext()
{
}

XlsxXmlDrawingReader::XlsxXmlDrawingReader(KoOdfWriters *writers)
    : MSOOXML::MsooXmlCommonReader(writers)
    , m_context(0)
    , m_currentDrawingObject(0)
    , m_anchorType(XlsxDrawingObject::NoAnchor)
    , m_chartNumber(0)
{
}

XlsxXmlDrawingReader::~XlsxXmlDrawingReader()
{
    Q_ASSERT(!m_currentDrawingObject);
}

KoFilter::ConversionStatus XlsxXmlDrawingReader::read(MSOOXML::MsooXmlReaderContext* context)
{
    m_context = dynamic_cast<XlsxXmlDrawingReaderContext*>(context);
    Q_ASSERT(m_context);

    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }

    readNext();
    if (!expectEl("xdr:wsDr")) {
        return KoFilter::WrongFormat;
    }

    while (!atEnd()) {
        QXmlStreamReader::TokenType tokenType = readNext();
        if(tokenType == QXmlStreamReader::Invalid || tokenType == QXmlStreamReader::EndDocument) break;
        if (isStartElement()) {
            const QStringRef s = qualifiedName();
            if ( s == "xdr:oneCellAnchor" || s == "xdr:twoCellAnchor" || s == "xdr:absoluteAnchor" || s == "xdr:grpSp" ) {
                read_anchor(s);
            }
        }
    }
#if 0
    m_context->saveCurrentCellData();
#endif
    m_context = 0;
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL twoCellAnchor
KoFilter::ConversionStatus XlsxXmlDrawingReader::read_anchor(const QStringRef&)
{
    READ_PROLOGUE

    class DrawingObjectGuard { // like QScopedPointer but sets the pointer to NULL afterwards
        public:
            DrawingObjectGuard(XlsxDrawingObject** obj) : m_obj(obj) {}
            ~DrawingObjectGuard() { delete *m_obj; *m_obj = 0; }
        private:
            XlsxDrawingObject** m_obj;
    };

    Q_ASSERT(!m_currentDrawingObject);
    m_currentDrawingObject = new XlsxDrawingObject(m_context->sheet);
    DrawingObjectGuard _guard(&m_currentDrawingObject);

    while (!atEnd()) {
        QXmlStreamReader::TokenType tokenType = readNext();
        if(tokenType == QXmlStreamReader::Invalid || tokenType == QXmlStreamReader::EndDocument) break;
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            // twoCellAnchor does define the 'from' and 'to' elements which do define the anchor-points
            TRY_READ_IF(from)
            ELSE_TRY_READ_IF(to)
            // a shape
            ELSE_TRY_READ_IF(sp)
            // the reference to a picture
            ELSE_TRY_READ_IF(pic)
            // a graphic-frame
            ELSE_TRY_READ_IF(graphicFrame)
        }
    }

    if (m_currentDrawingObject->m_type != XlsxDrawingObject::Unknown) {
        if (m_currentDrawingObject->m_positions.contains(XlsxDrawingObject::FromAnchor)) {
            XlsxDrawingObject::Position pos = m_currentDrawingObject->m_positions[XlsxDrawingObject::FromAnchor];
            Cell* cell = m_context->sheet->cell(pos.m_col, pos.m_row, true);
            cell->drawings.append(m_currentDrawingObject);
            m_currentDrawingObject = 0;
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL from
KoFilter::ConversionStatus XlsxXmlDrawingReader::read_from()
{
    READ_PROLOGUE
#if 0
    m_context->saveCurrentCellData();
#endif
    m_anchorType = XlsxDrawingObject::FromAnchor;
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(col)
            ELSE_TRY_READ_IF(row)
            ELSE_TRY_READ_IF(colOff)
            ELSE_TRY_READ_IF(rowOff)
        }
    }
    m_anchorType = XlsxDrawingObject::NoAnchor;
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL to
KoFilter::ConversionStatus XlsxXmlDrawingReader::read_to()
{
    READ_PROLOGUE
    m_anchorType = XlsxDrawingObject::ToAnchor;
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(col)
            ELSE_TRY_READ_IF(row)
            ELSE_TRY_READ_IF(colOff)
            ELSE_TRY_READ_IF(rowOff)
        }
    }
    m_anchorType = XlsxDrawingObject::NoAnchor;
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL col
KoFilter::ConversionStatus XlsxXmlDrawingReader::read_col()
{
    m_currentDrawingObject->m_positions[m_anchorType].m_col = readElementText().toInt(); // default value is zero
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL row
KoFilter::ConversionStatus XlsxXmlDrawingReader::read_row()
{
    m_currentDrawingObject->m_positions[m_anchorType].m_row = readElementText().toInt(); // default value is zero
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL colOff
KoFilter::ConversionStatus XlsxXmlDrawingReader::read_colOff()
{
    m_currentDrawingObject->m_positions[m_anchorType].m_colOff = readElementText().toInt(); // default value is zero
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL rowOff
KoFilter::ConversionStatus XlsxXmlDrawingReader::read_rowOff()
{
    m_currentDrawingObject->m_positions[m_anchorType].m_rowOff = readElementText().toInt(); // default value is zero
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL graphicFrame
//! graphicFrame
/*!
  This element specifies the existence of a graphics frame. This frame contains a graphic that was generated
  by an external source and needs a container in which to be displayed on the slide surface.

  Parent Elements:
    - grpSp (§4.4.1.19); spTree (§4.4.1.42)
  Child Elements:
    - extLst (Extension List with Modification Flag) (§4.2.4)
    - graphic (Graphic Object) (§5.1.2.1.16)
    - nvGraphicFramePr (Non-Visual Properties for a Graphic Frame) (§4.4.1.27)
    - xfrm (2D Transform for Graphic Frame)
*/
KoFilter::ConversionStatus XlsxXmlDrawingReader::read_graphicFrame()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            if (qualifiedName() == "a:graphic") {
                read_graphic2();
            }
        }
    }
    READ_EPILOGUE
}

#define blipFill_NS "a"
#undef MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_NS "a"

#undef CURRENT_EL
#define CURRENT_EL graphic
//! graphic handler (Graphic Object)
KoFilter::ConversionStatus XlsxXmlDrawingReader::read_graphic2()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            if (qualifiedName() == "a:graphicData") {
                read_graphicData2();
            }
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL graphicData
//! graphicData handler (Graphic Object Data)
KoFilter::ConversionStatus XlsxXmlDrawingReader::read_graphicData2()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            //TRY_READ_IF_NS(pic, pic)
            TRY_READ_IF_NS(pic, pic)
            if (qualifiedName() == "c:chart") {
                read_chart();
            }
            else if (qualifiedName() == QLatin1String("dgm:relIds")) {
                read_diagram(); // DrawingML diagram
            }
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL diagram
//! 5.9 DrawingML - Diagrams
/*!
A DrawingML diagram allows the definition of diagrams using DrawingML objects and constructs. This
namespace defines the contents of a DrawingML diagram.
*/
KoFilter::ConversionStatus XlsxXmlDrawingReader::read_diagram()
{
#if 0
    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR_WITH_NS(r, cs) // colors
    TRY_READ_ATTR_WITH_NS(r, dm) // data
    TRY_READ_ATTR_WITH_NS(r, lo) // layout
    TRY_READ_ATTR_WITH_NS(r, qs) // quickStyle

    //const QString colorsfile     = r_cs.isEmpty() ? QString() : m_context->relationships->target(m_context->path, m_context->file, r_cs);
    const QString datafile       = r_dm.isEmpty() ? QString() : m_context->relationships->target(m_context->path, m_context->file, r_dm);
    const QString layoutfile     = r_lo.isEmpty() ? QString() : m_context->relationships->target(m_context->path, m_context->file, r_lo);
    //const QString quickstylefile = r_qs.isEmpty() ? QString() : m_context->relationships->target(m_context->path, m_context->file, r_qs);

    //kDebug()<<"colorsfile="<<colorsfile<<"datafile="<<datafile<<"layoutfile="<<layoutfile<<"quickstylefile="<<quickstylefile;

    //KoStore* storeout = m_context->import->outputStore();
    MSOOXML::MsooXmlDiagramReaderContext* context = new MSOOXML::MsooXmlDiagramReaderContext(mainStyles);

    // first read the data-model
    MSOOXML::MsooXmlDiagramReader dataReader(this);
    const KoFilter::ConversionStatus dataReaderResult = m_context->import->loadAndParseDocument(&dataReader, datafile, context);
    if (dataReaderResult != KoFilter::OK) {
       raiseError(dataReader.errorString());
       delete context;
       return dataReaderResult;
    }
    
    // then read the layout definition
    MSOOXML::MsooXmlDiagramReader layoutReader(this);
    const KoFilter::ConversionStatus layoutReaderResult = m_context->import->loadAndParseDocument(&layoutReader, layoutfile, context);
    if (layoutReaderResult != KoFilter::OK) {
       raiseError(layoutReader.errorString());
       delete context;
       return layoutReaderResult;
    }
    
    m_context->diagrams << context;
#endif
    return KoFilter::OK;
}

XlsxXmlEmbeddedPicture::XlsxXmlEmbeddedPicture()
    : m_x(0.0)
    , m_y(0.0)
    , m_width(0.0)
    , m_height(0.0)
{

}

XlsxXmlEmbeddedPicture::XlsxXmlEmbeddedPicture(const QString &filePath)
    : m_x(0.0)
    , m_y(0.0)
    , m_width(0.0)
    , m_height(0.0)
{
    m_path = filePath;
}

bool XlsxXmlEmbeddedPicture::saveXml(KoXmlWriter *xmlWriter)   // save all needed attributes to .ods
{
    xmlWriter->startElement("draw:frame");

    if (m_fromCell.m_col > 0) {
        xmlWriter->addAttributePt("svg:x", EMU_TO_POINT(m_fromCell.m_colOff));
        xmlWriter->addAttributePt("svg:y", EMU_TO_POINT(m_fromCell.m_rowOff));
    } else {
        xmlWriter->addAttributePt("svg:x", m_x);
        xmlWriter->addAttributePt("svg:y", m_y);
    }

    // use width and height only if they are non-zero
    if (m_width > 0) {
        xmlWriter->addAttributePt("svg:width", m_width);
    }
    if (m_height > 0) {
        xmlWriter->addAttributePt("svg:height", m_height);
    }

    if (m_toCell.m_col > 0) {
        xmlWriter->addAttribute("table:end-cell-address", KSpread::Util::encodeColumnLabelText(m_toCell.m_col+1) + QString::number(m_toCell.m_row+1));
        xmlWriter->addAttributePt("table:end-x", EMU_TO_POINT(m_toCell.m_colOff));
        xmlWriter->addAttributePt("table:end-y", EMU_TO_POINT(m_toCell.m_rowOff));
    }

    xmlWriter->startElement("draw:image");

    xmlWriter->addAttribute("xlink:href", m_path);
    xmlWriter->addAttribute("xlink:type", "simple");
    xmlWriter->addAttribute("xlink:show", "embed");
    xmlWriter->addAttribute("xlink:actuate", "onLoad");

    xmlWriter->endElement(); // draw:object
    xmlWriter->endElement(); // draw:frame
    return true;
}

QString XlsxXmlEmbeddedPicture::path()
{
    return m_path;
}

void XlsxXmlEmbeddedPicture::setPath(QString &newPath)
{
    m_path = newPath;
}

// in PPTX we do not have pPr, so p@text:style-name should be added earlier
//#define SETUP_PARA_STYLE_IN_READ_P
#include <MsooXmlCommonReaderImpl.h> // this adds a:p, a:pPr, a:t, a:r, etc.
#define DRAWINGML_NS "a"
#define DRAWINGML_PIC_NS "xdr" // DrawingML/Picture
#define DRAWINGML_TXBODY_NS "xdr" // DrawingML/Picture
#define XLSXXMLDRAWINGREADER_CPP
#include <MsooXmlCommonReaderDrawingMLImpl.h> // this adds p:pic, etc.
//#include <MsooXmlDrawingReaderTableImpl.h> //this adds a:tbl
