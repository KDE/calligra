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
}

ComicBoxesShape::~ComicBoxesShape()
{
}

void ComicBoxesShape::paint( QPainter &painter,
                const KoViewConverter &converter )
{
  QRectF target = converter.documentToView(QRectF(QPointF(0,0), size()));
  painter.drawRect(target);
}

void ComicBoxesShape::saveOdf(KoShapeSavingContext & context) const
{
    KoXmlWriter &writer = context.xmlWriter();

    writer.startElement( "calligra:comicboxes" );
    writer.addAttribute("xmlns:calligra", "http://calligra-suite.org/");
    saveOdfAttributes( context, OdfAllAttributes );
    saveOdfCommonChildElements( context );
    writer.endElement(); // braindump:shape
}

bool ComicBoxesShape::loadOdf(const KoXmlElement & element, KoShapeLoadingContext &context)
{
    loadOdfAttributes( element, context, OdfAllAttributes );
    return true;
}
