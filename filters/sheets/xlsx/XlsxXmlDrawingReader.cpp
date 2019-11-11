/*
 * This file is part of Office 2007 Filters for Calligra
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
#include "XlsxChartOdfWriter.h"

#include <MsooXmlSchemas.h>
#include <MsooXmlUtils.h>
#include <MsooXmlRelationships.h>
#include <MsooXmlUnits.h>
#include <MsooXmlDiagramReader.h>

#include <KoXmlWriter.h>
#include <KoGenStyles.h>
#include <KoOdfGraphicStyles.h>
#include <QFontMetricsF>

#include <sheets/Util.h>

#define MSOOXML_CURRENT_NS "xdr"
#define MSOOXML_CURRENT_CLASS XlsxXmlDrawingReader
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS

#include <MsooXmlReader_p.h>
#include <MsooXmlContentTypes.h>

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

// Returns A for 0, B for 1, C for 2, etc.
QString columnName2(uint column)
{
    QString s;
    unsigned digits = 1;
    unsigned offset = 0;
    for (unsigned limit = 26; column >= limit + offset; limit *= 26, ++digits)
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
            m_chart->m_chartWriter->saveIndex(xmlWriter);
        } break;
        case Diagram: {
            xmlWriter->startElement("draw:g");
            xmlWriter->addAttribute("draw:name", "SmartArt Shapes Group");
            xmlWriter->addAttribute("draw:z-index", "0");
            //xmlWriter->addAttribute("table:end-cell-address", fromCellAddress());
            //xmlWriter->addAttribute("table:end-x", rect.width());
            //xmlWriter->addAttribute("table:end-y", rect.height());
            m_diagram->saveIndex(xmlWriter, positionRect());
            xmlWriter->endElement(); // draw:g
        } break;
        case Picture: {
            m_picture->saveXml(xmlWriter);
            delete m_picture;
            m_type = Unknown;
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

KoXmlWriter* XlsxDrawingObject::pictureWriter()
{
    if (m_type == Unknown) {
        setPicture(new XlsxXmlEmbeddedPicture());
    }
    Q_ASSERT(m_picture);
    return m_picture->pictureWriter();
}



QRect XlsxDrawingObject::positionRect() const
{
    QRect rect(QPoint(0,0),QSize(0,0));
    if(m_positions.contains(FromAnchor)) {
        qreal defaultColumnWidth = 8.43;
        qreal defaultRowHeight = 12.75;
        Position f1 = m_positions[FromAnchor];
        rect.setX( EMU_TO_POINT(f1.m_colOff) );
        rect.setY( EMU_TO_POINT(f1.m_rowOff) );
        if(m_positions.contains(ToAnchor)) {
            Position f2 = m_positions[ToAnchor];
            if(f2.m_col > 0 && f2.m_row > 0) {
                rect.setWidth( columnWidth2( f2.m_col - f1.m_col - 1, EMU_TO_POINT(f2.m_colOff), defaultColumnWidth) );
                rect.setHeight( rowHeight2( f2.m_row - f1.m_row - 1, EMU_TO_POINT(f2.m_rowOff), defaultRowHeight) );
            }
        }
        Q_ASSERT(rect.x() >= 0);
        Q_ASSERT(rect.y() >= 0);
        Q_ASSERT(rect.width() >= 0);
        Q_ASSERT(rect.height() >= 0);
    }
    return rect;
}

QString XlsxDrawingObject::cellAddress(const QString &sheetname, int row, int column) const
{
    QString result;
    if(!sheetname.isEmpty())
        result += sheetname + '.';
    result += columnName2(column) + QString::number(row + 1);
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

bool XlsxDrawingObject::isAnchoredToCell() const
{
    return (m_positions.contains(FromAnchor));
}


XlsxXmlDrawingReaderContext::XlsxXmlDrawingReaderContext(XlsxXmlWorksheetReaderContext* _worksheetReaderContext, Sheet* _sheet, const QString& _path, const QString& _file)
    : MSOOXML::MsooXmlReaderContext(_worksheetReaderContext->relationships)
    , import(_worksheetReaderContext->import)
    , path(_path)
    , file(_file)
    , themes((_worksheetReaderContext->themes))
    , worksheetReaderContext(_worksheetReaderContext)
    , sheet(_sheet)
    , m_groupDepthCounter(0)
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
    initDrawingML();
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
        readNext();
        if (isEndElement() && name() == "wsDr") {
            break;
        }
        if (isStartElement()) {
            TRY_READ_IF(oneCellAnchor)
            ELSE_TRY_READ_IF(twoCellAnchor)
            ELSE_TRY_READ_IF(absoluteAnchor)
            SKIP_UNKNOWN
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
KoFilter::ConversionStatus XlsxXmlDrawingReader::read_twoCellAnchor()
{
    READ_PROLOGUE

    return read_anchor("twoCellAnchor");

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL oneCellAnchor
KoFilter::ConversionStatus XlsxXmlDrawingReader::read_oneCellAnchor()
{
    READ_PROLOGUE

    return read_anchor("oneCellAnchor");

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL absoluteAnchor
KoFilter::ConversionStatus XlsxXmlDrawingReader::read_absoluteAnchor()
{
    READ_PROLOGUE

    return read_anchor("absoluteAnchor");

    READ_EPILOGUE
}

KoFilter::ConversionStatus XlsxXmlDrawingReader::read_anchor(const QString& reference)
{
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
        readNext();
        if (isEndElement() && name() == reference) {
            break;
        }
        if (isStartElement()) {
            // twoCellAnchor does define the 'from' and 'to' elements which do define the anchor-points
            TRY_READ_IF(from)
            ELSE_TRY_READ_IF(to)
            else if (qualifiedName() == QLatin1String("xdr:ext")) {
                // read_ext expects the qualifed name a:ext
                const QXmlStreamAttributes attrs(attributes());

                READ_ATTR_WITHOUT_NS(cx)
                STRING_TO_INT(cx, m_svgWidth, "ext@cx")
                READ_ATTR_WITHOUT_NS(cy)
                STRING_TO_INT(cy, m_svgHeight, "ext@cy")
                readNext();
            }
            ELSE_TRY_READ_IF(ext)
            // a shape
            ELSE_TRY_READ_IF(sp)
            // the reference to a picture
            ELSE_TRY_READ_IF(pic)
            // a graphic-frame
            ELSE_TRY_READ_IF(graphicFrame)
            ELSE_TRY_READ_IF(cxnSp)
            ELSE_TRY_READ_IF(grpSp)
            SKIP_UNKNOWN
        }
    }

    if (m_currentDrawingObject->m_type != XlsxDrawingObject::Unknown) {
        if (m_currentDrawingObject->isAnchoredToCell()) {
            XlsxDrawingObject::Position pos = m_currentDrawingObject->m_positions[XlsxDrawingObject::FromAnchor];
            Cell* cell = m_context->sheet->cell(pos.m_col, pos.m_row, true);
            cell->appendDrawing(m_currentDrawingObject);
            m_currentDrawingObject = 0;
        }
    }
    return KoFilter::OK;
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
        BREAK_IF_END_OF(CURRENT_EL)
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
        BREAK_IF_END_OF(CURRENT_EL)
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
//! graphicFrame (Graphic Frame)
/*! ECMA-376, 20.5.2.16, p. 3524 (SpreadSheetML)
  This element describes a single graphical object frame for a spreadsheet
  which contains a graphical object.

  Parent Elements:
  - [done] absoluteAnchor (§20.5.2.1)
  - [done] grpSp (§20.5.2.17)
  - [done] oneCellAnchor (§20.5.2.24)
  - [done] twoCellAnchor (§20.5.2.33)

  Child Elements:
  - [done] graphic (Graphic Object) (§20.1.2.2.16)
  - nvGraphicFramePr (Non-Visual Properties for a Graphic Frame) (§20.5.2.20)
  - xfrm (2D Transform for Graphic Frame) (§20.5.2.36)
*/
KoFilter::ConversionStatus XlsxXmlDrawingReader::read_graphicFrame()
{
    READ_PROLOGUE

    //TODO: Create a graphic style for the frame.

    MSOOXML::Utils::XmlWriteBuffer buffer;
    body = buffer.setWriter(body);

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF_NS(a, graphic)
            SKIP_UNKNOWN
        }
    }

    body = buffer.originalWriter();

    if (m_context->graphicObjectIsGroup) {
        body->startElement("draw:g");
    } else {
        body->startElement("draw:frame");
    }

    buffer.releaseWriter();
    body->endElement(); //draw:g/draw:frame

    READ_EPILOGUE
}

XlsxXmlEmbeddedPicture::XlsxXmlEmbeddedPicture():m_pictureWriter(0)
{
    m_pictureBuffer.open(QIODevice::ReadWrite);
}

XlsxXmlEmbeddedPicture::~XlsxXmlEmbeddedPicture()
{
    delete m_pictureWriter;
}


KoXmlWriter* XlsxXmlEmbeddedPicture::pictureWriter()
{
    if (!m_pictureWriter) {
        m_pictureWriter = new KoXmlWriter(&m_pictureBuffer);
    }
    return m_pictureWriter;
}


bool XlsxXmlEmbeddedPicture::saveXml(KoXmlWriter *xmlWriter)   // save all needed attributes to .ods
{
    Q_ASSERT(m_pictureWriter);
    if (!m_pictureWriter || !m_pictureWriter->device()->size()){
        return false;
    }

    xmlWriter->addCompleteElement(m_pictureWriter->device());
    return true;
}


#undef MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_NS "a" // required for the next header file

// in PPTX we do not have pPr, so p@text:style-name should be added earlier
//#define SETUP_PARA_STYLE_IN_READ_P
#include <MsooXmlCommonReaderImpl.h> // adds a:p, a:pPr, a:t, a:r, etc.

#define DRAWINGML_NS "a"
#define DRAWINGML_PIC_NS "xdr" // DrawingML/Picture
#define DRAWINGML_TXBODY_NS "xdr" // DrawingML/Picture
#define XLSXXMLDRAWINGREADER_CPP

#include <MsooXmlCommonReaderDrawingMLImpl.h> // adds p:pic, etc.
//#include <MsooXmlDrawingReaderTableImpl.h>  // adds a:tbl
