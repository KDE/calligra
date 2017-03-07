/* This file is part of the KDE project
   Copyright (C) 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>

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
#include "ODrawClient.h"

#include <QDebug>
#include <QColor>
#include <KoGenStyles.h>
#include <KoXmlWriter.h>
#include <KoEmbeddedDocumentSaver.h>
#include <KoShapeSavingContext.h>
#include <KoTextWriter.h>
#include <KoStyleManager.h>
#include <KoTextDocument.h>

#include "sheet.h"
#include "workbook.h"

#ifndef __GNUC__
  #define __PRETTY_FUNCTION__ __FUNCTION__
#endif /* __PRETTY_FUNCTION__ only exists in gnu c++ */

ODrawClient::ODrawClient(Swinder::Sheet* sheet)
    : m_sheet(sheet), m_zIndex(0), m_styleManager(0)
{
}

static qreal offset( unsigned long dimension, unsigned long offset, qreal factor ) {
    return (float)dimension * (float)offset / factor;
}

static qreal columnWidth(Swinder::Sheet* sheet, unsigned long col) {
    if( sheet->column(col, false) )
        return sheet->column(col)->width();

    return sheet->defaultColWidth();
}

static qreal rowHeight(Swinder::Sheet* sheet, unsigned long row) {
    if( sheet->row(row, false) )
        return sheet->row(row)->height();

    return sheet->defaultRowHeight();
}

QRectF ODrawClient::getRect(const MSO::OfficeArtClientAnchor& clientAnchor)
{
    const MSO::XlsOfficeArtClientAnchor* anchor = clientAnchor.anon.get<MSO::XlsOfficeArtClientAnchor>();
    if (anchor) {
        QRectF r;
        qreal colWidth = columnWidth(m_sheet, anchor->colL);
        r.setLeft(offset(colWidth, anchor->dxL, 1024));
        if (anchor->colR == anchor->colL) {
            r.setRight(offset(colWidth, anchor->dxR, 1024));
        } else {
            qreal width = colWidth - r.left();
            for (int col = anchor->colL + 1; col < anchor->colR; ++col) {
                width += columnWidth(m_sheet, col);
            }
            width += offset(columnWidth(m_sheet, anchor->colR), anchor->dxR, 1024);
            r.setWidth(width);
        }
        qreal rowHgt = rowHeight(m_sheet, anchor->rwT);
        r.setTop(offset(rowHgt, anchor->dyT, 256));
        if (anchor->rwT == anchor->rwB) {
            r.setBottom(offset(rowHgt, anchor->dyB, 256));
        } else {
            qreal height = rowHgt - r.top();
            for (int row = anchor->rwT + 1; row < anchor->rwB; ++row) {
                height += rowHeight(m_sheet, row);
            }
            height += offset(rowHeight(m_sheet, anchor->rwB), anchor->dyB, 256);
            r.setHeight(height);
        }
        return r;
    } else {
        qDebug() << "Invalid client anchor!";
    }
    return QRectF();
}

QRectF ODrawClient::getReserveRect(void)
{
    //NOTE: No XLS test files at the moment.
    return QRectF();
}

QRectF ODrawClient::getGlobalRect(const MSO::OfficeArtClientAnchor &clientAnchor)
{
    const MSO::XlsOfficeArtClientAnchor* anchor = clientAnchor.anon.get<MSO::XlsOfficeArtClientAnchor>();
    if (!anchor) return QRectF();
    QRectF r = getRect(clientAnchor);
    qreal x = 0, y = 0;
    for (int row = 0; row < anchor->rwT; row++) {
        y += rowHeight(m_sheet, row);
    }
    for (int col = 0; col < anchor->colL; col++) {
        x += columnWidth(m_sheet, col);
    }
    return r.adjusted(x, y, x, y);
}


QString ODrawClient::getPicturePath(const quint32 pib)
{
    quint32 offset = 0;
    if (!m_sheet->workbook()->officeArtDggContainer()) {
        return QString();
    }

    QByteArray rgbUid = getRgbUid(*m_sheet->workbook()->officeArtDggContainer(), pib, offset);

    QString fileName;
    if (rgbUid.isEmpty()) {
        qDebug() << "Object in blipStore with pib: " << pib << "was not found.";
    }else {
        fileName = m_sheet->workbook()->pictureName(rgbUid);
    }

    if (!fileName.isEmpty()){
        return "Pictures/" + fileName;
    }
    return QString();
}

bool ODrawClient::processRectangleAsTextBox(const MSO::OfficeArtClientData& cd)
{
    Q_UNUSED(cd);
    return false;
}

bool ODrawClient::onlyClientData(const MSO::OfficeArtClientData &o)
{
    Q_UNUSED(o);
    qDebug() << "NOT YET IMPLEMENTED" << __PRETTY_FUNCTION__;
    return !m_shapeText.text().isEmpty();
}

void ODrawClient::processClientData(const MSO::OfficeArtClientTextBox *ct,
                                    const MSO::OfficeArtClientData &o, Writer &out)
{
    Q_UNUSED(ct);
    Q_UNUSED(o);
    if (m_shapeText.richText()) { // rich-text
        KoTextDocument doc(m_shapeText.richText());
        Q_ASSERT(!doc.styleManager());
        Q_ASSERT(m_styleManager);
        doc.setStyleManager(m_styleManager);

        KoEmbeddedDocumentSaver embeddedSaver;
        KoShapeSavingContext context(out.xml, out.styles, embeddedSaver);
        KoTextWriter textWriter(context);
        textWriter.write(m_shapeText.richText(), 0);

        doc.setStyleManager(0);
    } else { // plain-text
        QStringList lines = m_shapeText.text().split(QRegExp("[\n\r]"));
        foreach (const QString& line, lines) {
            out.xml.startElement("text:p", false);
            int pos = 0;
            while (pos < line.length()) {
                int idx = line.indexOf(QRegExp("[^ ]"), pos);
                if (idx == -1) idx = line.length();
                int cnt = idx - pos;
                if (cnt > 1) {
                    out.xml.startElement("text:s");
                    out.xml.addAttribute("text:c", cnt);
                    out.xml.endElement();
                    pos += cnt; cnt = 0;
                }
                int endPos = qMax(line.length()-1, line.indexOf(' ', pos+cnt));
                out.xml.addTextNode(line.mid(pos, endPos - pos + 1));
                pos = endPos + 1;
            }
            out.xml.endElement();
        }
    }
}

void ODrawClient::processClientTextBox(const MSO::OfficeArtClientTextBox &ct,
                                       const MSO::OfficeArtClientData *cd, Writer &out)
{
    Q_UNUSED(ct);
    Q_UNUSED(cd);
    Q_UNUSED(out);
    qDebug() << "NOT YET IMPLEMENTED" << __PRETTY_FUNCTION__;
}

KoGenStyle ODrawClient::createGraphicStyle(const MSO::OfficeArtClientTextBox *ct,
                                           const MSO::OfficeArtClientData *cd,
                                           const DrawStyle& ds,
                                           Writer &out)
{
    Q_UNUSED(ct);
    Q_UNUSED(cd);
    Q_UNUSED(ds);
    KoGenStyle style = KoGenStyle(KoGenStyle::GraphicAutoStyle, "graphic");
    if (!m_shapeText.text().isEmpty()) {
        switch (m_shapeText.hAlign()) {
        case Swinder::TxORecord::Left:
            style.addProperty("draw:textarea-horizontal-align", "left");
            break;
        case Swinder::TxORecord::Centered:
            style.addProperty("draw:textarea-horizontal-align", "center");
            break;
        case Swinder::TxORecord::Right:
            style.addProperty("draw:textarea-horizontal-align", "right");
            break;
        case Swinder::TxORecord::Justified:   // FIXME: Not supported
        case Swinder::TxORecord::Distributed: // FIXME: Not supported
        default:
            ;
        }
        switch (m_shapeText.vAlign()) {
        case Swinder::TxORecord::Top:
            style.addProperty("draw:textarea-vertical-align", "top");
            break;
        case Swinder::TxORecord::VCentered:
            style.addProperty("draw:textarea-vertical-align", "middle");
            break;
        case Swinder::TxORecord::Bottom:
            style.addProperty("draw:textarea-vertical-align", "bottom");
            break;
        case Swinder::TxORecord::VJustified:   // FIXME: Not supported
        case Swinder::TxORecord::VDistributed: // FIXME: Not supported
        default:
            ;
        }
    }
    //draw:textarea-horizontal-align="justify" draw:textarea-vertical-align="top"
    style.setAutoStyleInStylesDotXml(out.stylesxml);
    return style;
}

void ODrawClient::addTextStyles(const MSO::OfficeArtClientTextBox *clientTextbox,
                                const MSO::OfficeArtClientData *clientData,
                                KoGenStyle &style, Writer &out)
{
    Q_UNUSED(clientTextbox);
    Q_UNUSED(clientData);
    const QString styleName = out.styles.insert(style);
    out.xml.addAttribute("draw:style-name", styleName);

    setZIndexAttribute(out);
}

QColor ODrawClient::toQColor(const MSO::OfficeArtCOLORREF &c)
{
    if (c.fSchemeIndex) {
        return m_sheet->workbook()->color(c.red);
    }
    return QColor(c.red, c.green, c.blue);
}

QString ODrawClient::formatPos(qreal v)
{
    return QString::number(v, 'f', 11) + "pt";
}

const MSO::OfficeArtDggContainer* ODrawClient::getOfficeArtDggContainer()
{
    return m_sheet->workbook()->officeArtDggContainer();
}

const MSO::OfficeArtSpContainer* ODrawClient::getMasterShapeContainer(quint32 spid)
{
    Q_UNUSED(spid);
    //TODO: locate the OfficeArtSpContainer with shapeProp/spid == spid
    MSO::OfficeArtSpContainer* sp = nullptr;
    return sp;
}

void ODrawClient::setShapeText(const Swinder::TxORecord &text)
{
    m_shapeText = text;
}

void ODrawClient::setZIndexAttribute(Writer& out)
{
    out.xml.addAttribute("draw:z-index", m_zIndex);
    m_zIndex++;
}

void ODrawClient::setStyleManager(KoStyleManager* styleManager)
{
    m_styleManager = styleManager;
}

KoStyleManager* ODrawClient::styleManager() const
{
    return m_styleManager;
}
