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

#include <qdebug.h>
#include <QColor>
#include <KoXmlWriter.h>
#include "sheet.h"
#include "workbook.h"

#ifndef __GNUC__ 
  #define __PRETTY_FUNCTION__ __FUNCTION__
#endif /* __PRETTY_FUNCTION__ only exists in gnu c++ */

ODrawClient::ODrawClient(Swinder::Sheet* sheet)
    : m_sheet(sheet)
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
    if(sheet->row(row, false) )
        return sheet->row(row)->height();

    return sheet->defaultRowHeight();
}

QRectF ODrawClient::getRect(const MSO::OfficeArtClientAnchor& clientAnchor)
{
    const MSO::XlsOfficeArtClientAnchor anchor = clientAnchor.anon().get<MSO::XlsOfficeArtClientAnchor>();
    if (anchor.isValid()) {
        // obtain values or use defaults: TODO find out defaults
        const quint16 colL = (anchor.colL().isPresent()) ?*anchor.colL() :0;
        const quint16 colR = (anchor.colR().isPresent()) ?*anchor.colR() :0;
        const qint16 dxL = (anchor.dxL().isPresent()) ?*anchor.dxL() :0;
        const qint16 dxR = (anchor.dxR().isPresent()) ?*anchor.dxR() :0;
        const quint16 rwT = (anchor.rwT().isPresent()) ?*anchor.rwT() :0;
        const quint16 rwB = (anchor.rwB().isPresent()) ?*anchor.rwB() :0;
        const qint16 dyT = (anchor.dyT().isPresent()) ?*anchor.dyT() :0;
        const qint16 dyB = (anchor.dyB().isPresent()) ?*anchor.dyB() :0;
        QRectF r;
        qreal colWidth = columnWidth(m_sheet, colL);
        r.setLeft(offset(colWidth, dxL, 1024));
        if (colR == colL) {
            r.setRight(offset(colWidth, dxR, 1024));
        } else {
            qreal width = colWidth - r.left();
            for (int col = colL + 1; col < colR; ++col) {
                width += columnWidth(m_sheet, col);
            }
            width += offset(columnWidth(m_sheet, colR), dxR, 1024);
            r.setWidth(width);
        }
        qreal rowHgt = rowHeight(m_sheet, rwT);
        r.setTop(offset(rowHgt, dyT, 256));
        if (rwT == rwB) {
            r.setBottom(offset(rowHgt, dyB, 256));
        } else {
            qreal height = rowHgt - r.top();
            for (int row = rwT + 1; row < rwB; ++row) {
                height += rowHeight(m_sheet, row);
            }
            height += offset(rowHeight(m_sheet, rwB), dyB, 256);
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
    const MSO::XlsOfficeArtClientAnchor anchor = clientAnchor.anon().get<MSO::XlsOfficeArtClientAnchor>();
    if (!anchor.isValid() || !anchor.rwT().isPresent() || !anchor.colL().isPresent()) return QRectF();
    QRectF r = getRect(clientAnchor);
    qreal x = 0, y = 0;
    for (int row = 0; row < *anchor.rwT(); row++) {
        y += rowHeight(m_sheet, row);
    }
    for (int col = 0; col < *anchor.colL(); col++) {
        x += columnWidth(m_sheet, col);
    }
    return r.adjusted(x, y, x, y);
}


QString ODrawClient::getPicturePath(const quint32 pib)
{
    qDebug() << "NOT YET IMPLEMENTED" << __PRETTY_FUNCTION__;
    Q_UNUSED(pib);
    return QString();
}

bool ODrawClient::processRectangleAsTextBox(const MSO::OfficeArtClientData& cd)
{
    Q_UNUSED(cd);
    return false;
}

bool ODrawClient::onlyClientData(const MSO::OfficeArtClientData &o)
{
    qDebug() << "NOT YET IMPLEMENTED" << __PRETTY_FUNCTION__;
    return !m_shapeText.m_text.isEmpty();
}

void ODrawClient::processClientData(const MSO::OfficeArtClientTextBox *ct,
                                    const MSO::OfficeArtClientData &o, Writer &out)
{
    qDebug() << "NOT YET IMPLEMENTED" << __PRETTY_FUNCTION__;
    QStringList lines = m_shapeText.m_text.split(QRegExp("[\n\r]"));
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

void ODrawClient::processClientTextBox(const MSO::OfficeArtClientTextBox &ct,
                                       const MSO::OfficeArtClientData *cd, Writer &out)
{
    qDebug() << "NOT YET IMPLEMENTED" << __PRETTY_FUNCTION__;
}

KoGenStyle ODrawClient::createGraphicStyle(const MSO::OfficeArtClientTextBox *ct,
                                           const MSO::OfficeArtClientData *cd,
                                           const DrawStyle& ds,
                                           Writer &out)
{
    Q_UNUSED(ds);
    KoGenStyle style = KoGenStyle(KoGenStyle::GraphicAutoStyle, "graphic");
    if (!m_shapeText.m_text.isEmpty()) {
        switch (m_shapeText.halign) {
        case Swinder::TxORecord::Left:
            style.addProperty("draw:textarea-horizontal-align", "left");
            break;
        case Swinder::TxORecord::Centered:
            style.addProperty("draw:textarea-horizontal-align", "center");
            break;
        case Swinder::TxORecord::Right:
            style.addProperty("draw:textarea-horizontal-align", "right");
            break;
        }
        switch (m_shapeText.valign) {
        case Swinder::TxORecord::Top:
            style.addProperty("draw:textarea-vertical-align", "top");
            break;
        case Swinder::TxORecord::VCentered:
            style.addProperty("draw:textarea-vertical-align", "middle");
            break;
        case Swinder::TxORecord::Bottom:
            style.addProperty("draw:textarea-vertical-align", "bottom");
            break;
        }
    }
    //draw:textarea-horizontal-align="justify" draw:textarea-vertical-align="top"
    style.setAutoStyleInStylesDotXml(out.stylesxml);
    return style;
}

void ODrawClient::addTextStyles(const quint16 msospt,
                                const MSO::OfficeArtClientTextBox *clientTextbox,
                                const MSO::OfficeArtClientData *clientData,
                                KoGenStyle &style, Writer &out)
{
    Q_UNUSED(msospt);
    const QString styleName = out.styles.insert(style);
    out.xml.addAttribute("draw:style-name", styleName);
}

MSO::OfficeArtDggContainer ODrawClient::getOfficeArtDggContainer()
{
    return m_sheet->workbook()->officeArtDggContainer();
}

MSO::OfficeArtSpContainer ODrawClient::getMasterShapeContainer(quint32 spid)
{
    //TODO: locate the OfficeArtSpContainer with shapeProp/spid == spid
    MSO::OfficeArtSpContainer sp;
    return sp;
}

QColor ODrawClient::toQColor(const MSO::OfficeArtCOLORREF &c)
{
    if (c.fSchemeIndex()) {
        return m_sheet->workbook()->color(c.red());
    }
    return QColor(c.red(), c.green(), c.blue());
}

QString ODrawClient::formatPos(qreal v)
{
    return QString::number(v, 'f', 11) + "pt";
}

void ODrawClient::setShapeText(const Swinder::TxORecord &text)
{
    m_shapeText = text;
}
