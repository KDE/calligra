/*
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "ComicBoxesShape.h"

#include <QPainter>
#include <QSvgRenderer>

#include <kdebug.h>

#include <KoShapeLoadingContext.h>
#include <KoShapeSavingContext.h>
#include <KoViewConverter.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>

ComicBoxesShape::ComicBoxesShape()
{
    recreatePath();
    setSize(QSizeF(100, 100));
}

ComicBoxesShape::~ComicBoxesShape()
{
}

void ComicBoxesShape::saveOdf(KoShapeSavingContext & context) const
{
    KoXmlWriter &writer = context.xmlWriter();

    writer.startElement( "calligra:comicboxes" );
    writer.addAttribute("xmlns:calligra", "http://calligra-suite.org/");
    saveOdfAttributes( context, OdfAllAttributes );
    saveOdfCommonChildElements( context );
    writer.startElement("lines");
    foreach(const QLineF& line, m_lines)
    {
        writer.startElement("line");
        writer.addAttribute("x1", line.p1().x());
        writer.addAttribute("y1", line.p1().y());
        writer.addAttribute("x2", line.p2().x());
        writer.addAttribute("y2", line.p2().y());
        writer.endElement();
    }
    writer.endElement();
    writer.endElement(); // braindump:shape
}

bool ComicBoxesShape::loadOdf(const KoXmlElement & element, KoShapeLoadingContext &context)
{
    KoXmlElement childElement;
    forEachElement(childElement, element)
    {
        if(childElement.tagName() == "lines")
        {
            KoXmlElement lineElement;
            forEachElement(lineElement, childElement)
            {
                if(lineElement.tagName() == "line")
                {
                    qreal x1 = lineElement.attribute("x1", "0").toDouble();
                    qreal y1 = lineElement.attribute("y1", "0").toDouble();
                    qreal x2 = lineElement.attribute("x2", "1").toDouble();
                    qreal y2 = lineElement.attribute("y2", "1").toDouble();
                    m_lines.push_back(QLineF(x1, y1, x2, y2));
                }
            }
        }
    }
    recreatePath();
    loadOdfAttributes( element, context, OdfAllAttributes );
    return true;
}

void ComicBoxesShape::recreatePath()
{
    QSizeF s = size();
    clear();
    moveTo(QPointF(0, 0));
    lineTo(QPointF(0, 1.0));
    lineTo(QPointF(1.0, 1.0));
    lineTo(QPointF(1.0, 0));
    lineTo(QPointF(0, 0));
    
    foreach(const QLineF& line, m_lines)
    {
        moveTo(line.p1());
        lineTo(line.p2());
    }
    setSize(s);
}

void ComicBoxesShape::addLine( const QLineF& _line )
{
    Q_ASSERT(_line.p1().x() >= 0.0);
    Q_ASSERT(_line.p1().x() <= 1.0);
    Q_ASSERT(_line.p2().x() >= 0.0);
    Q_ASSERT(_line.p2().x() <= 1.0);
    Q_ASSERT(_line.p1().y() >= 0.0);
    Q_ASSERT(_line.p1().y() <= 1.0);
    Q_ASSERT(_line.p2().y() >= 0.0);
    Q_ASSERT(_line.p2().y() <= 1.0);
    Q_ASSERT(_line.p1().x() == 0.0 || _line.p1().y() == 0.0 || _line.p1().x() == 1.0 || _line.p1().y() == 1.0 );
    Q_ASSERT(_line.p2().x() == 0.0 || _line.p2().y() == 0.0 || _line.p2().x() == 1.0 || _line.p2().y() == 1.0 );
    m_lines.push_back(_line);
    recreatePath();
}
