/*
   KoReport Library
   Copyright (C) 2011 by Dag Andersen (danders@get2net.dk)

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "KoSimpleOdtTextBox.h"
#include <KoXmlWriter.h>
#include <KoDpi.h>
#include <KoOdfGraphicStyles.h>
#include <KoGenStyle.h>
#include <KoGenStyles.h>

#include "renderobjects.h"

#include <QColor>
#include <QFont>
#include <QPen>

#include <kdebug.h>

KoSimpleOdtTextBox::KoSimpleOdtTextBox(OROTextBox *primitive)
    : KoSimpleOdtPrimitive(primitive)
{
}

KoSimpleOdtTextBox::~KoSimpleOdtTextBox()
{
}

OROTextBox *KoSimpleOdtTextBox::textBox() const
{
    return dynamic_cast<OROTextBox*>(m_primitive);
}

void KoSimpleOdtTextBox::createStyle(KoGenStyles &coll)
{
    KoGenStyle ps(KoGenStyle::ParagraphStyle, "paragraph");
    QFont font = textBox()->textStyle().font;
    m_paragraphStyleName = coll.insert(ps, "P");

    // text style
    KoGenStyle ts(KoGenStyle::TextStyle, "text");
    ts.addProperty("fo:font-family", font.family());
    ts.addPropertyPt("fo:font-size", font.pointSizeF());
    m_textStyleName = coll.insert(ts, "T");

    KoGenStyle gs(KoGenStyle::GraphicStyle, "graphic");
    QPen pen;
    pen.setColor(textBox()->lineStyle().lineColor);
    pen.setWidthF(textBox()->lineStyle().weight);
    pen.setStyle(textBox()->lineStyle().style);
    kDebug()<<"------"<<pen;
    KoOdfGraphicStyles::saveOdfStrokeStyle(gs, coll, pen);
    m_frameStyleName = coll.insert(gs, "F");
    kDebug()<<coll.styles().values();
}

void KoSimpleOdtTextBox::createBody(KoXmlWriter *bodyWriter) const
{

    kDebug()<<textBox()->text()<<":"<<m_primitive->position()<<m_primitive->size();

    // convert to inches
    qreal x = m_primitive->position().x() / KoDpi::dpiX();
    qreal y = m_primitive->position().y() / KoDpi::dpiX();
    qreal w = m_primitive->size().width() / KoDpi::dpiX();
    qreal h = m_primitive->size().height() / KoDpi::dpiY();

    bodyWriter->startElement("draw:frame");
    bodyWriter->addAttribute("draw:id", itemName());
    bodyWriter->addAttribute("draw:name", itemName());
    bodyWriter->addAttribute("text:anchor-type", "page");
    bodyWriter->addAttribute("text:anchor-page-number", pageNumber());
    bodyWriter->addAttribute("draw:style-name", m_frameStyleName);
    bodyWriter->addAttribute("svg:x", QString("%1in").arg(x));
    bodyWriter->addAttribute("svg:y", QString("%1in").arg(y));
    bodyWriter->addAttribute("svg:width", QString("%1in").arg(w));
    bodyWriter->addAttribute("svg:height", QString("%1in").arg(h));
    bodyWriter->addAttribute("draw:z-index", "3");

    bodyWriter->startElement("draw:text-box");

    bodyWriter->startElement("text:p");
    bodyWriter->addAttribute("text:style-name", m_paragraphStyleName);
    bodyWriter->startElement("text:span");
    bodyWriter->addAttribute("text:style-name", m_textStyleName);
    bodyWriter->addTextNode(textBox()->text());

    bodyWriter->endElement(); // text:span
    bodyWriter->endElement(); // text:p
    bodyWriter->endElement(); // draw:text-box
    bodyWriter->endElement(); // draw:frame
}
