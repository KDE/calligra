/*
 *  Copyright (c) 2010,2011 Cyrille Berger <cberger@cberger.net>
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

#include "ComicBoxesLine.h"

ComicBoxesShape::ComicBoxesShape()
{
    m_lines.push_back(new ComicBoxesLine(QLineF(0,0,0,1)));
    m_lines.push_back(new ComicBoxesLine(QLineF(0,1,1,1)));
    m_lines.push_back(new ComicBoxesLine(QLineF(1,1,1,0)));
    m_lines.push_back(new ComicBoxesLine(QLineF(1,0,0,0)));
    recreatePath();
    setSize(QSizeF(100, 100));
}

ComicBoxesShape::~ComicBoxesShape()
{
    qDeleteAll(m_lines);
}

void ComicBoxesShape::saveOdf(KoShapeSavingContext & context) const
{
    KoXmlWriter &writer = context.xmlWriter();

    writer.startElement( "calligra:comicboxes" );
    writer.addAttribute("xmlns:calligra", "http://calligra-suite.org/");
    saveOdfAttributes( context, OdfAllAttributes );
    saveOdfCommonChildElements( context );
    writer.startElement("lines");
    foreach(ComicBoxesLine* line, m_lines)
    {
        if(!line->isAbsoluteLine())
        {
            writer.startElement("line");
            writer.addAttribute("c1", line->c1());
            writer.addAttribute("c2", line->c2());
            writer.addAttribute("l1", m_lines.indexOf(line->line1()));
            writer.addAttribute("l2", m_lines.indexOf(line->line2()));
            writer.endElement();
        }
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
                    qreal c1 = lineElement.attribute("c1", "0").toDouble();
                    qreal c2 = lineElement.attribute("c2", "1").toDouble();
                    int l1 = lineElement.attribute("l1", "0").toDouble();
                    int l2 = lineElement.attribute("l2", "1").toDouble();
                    m_lines.push_back(new ComicBoxesLine(m_lines[l1], c1, m_lines[l2], c2 ));
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

    foreach(ComicBoxesLine* line, m_lines)
    {
        if(!line->isAbsoluteLine())
        {
            QLineF l = line->line();
            moveTo(l.p1());
            lineTo(l.p2());
        }
    }
    setSize(s);
}

inline bool near(qreal a, qreal b)
{
    return qAbs(a - b) < 1e-6;
}

void ComicBoxesShape::addLine( ComicBoxesLine* _line )
{
    m_lines.push_back(_line);
    recreatePath();
}

QList<ComicBoxesLine*> ComicBoxesShape::lines()
{
    return m_lines;
}

QTransform ComicBoxesShape::lines2ShapeTransform() const
{
    QTransform t;
    t.translate(position().x(), position().y());
    t.scale(size().width(), size().height());
    return t;
}
