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

#include "WebShape.h"

#include <QPainter>
#include <QWebPage>
#include <QWebFrame>

#include <KoShapeSavingContext.h>
#include <KoViewConverter.h>
#include <KoXmlWriter.h>
#include <KoXmlReader.h>

WebShape::WebShape() : m_webPage(new QWebPage)
{
}

WebShape::~WebShape()
{
}

void WebShape::paint( QPainter &painter,
                const KoViewConverter &converter )
{
  m_webPage->setViewportSize(m_webPage->mainFrame()->contentsSize());
  QImage image(m_webPage->viewportSize(), QImage::Format_ARGB32);
  QPainter imgPainter(&image);

  m_webPage->mainFrame()->render(&imgPainter);
  imgPainter.end();

  QRectF target = converter.documentToView(QRectF(QPointF(0,0), size()));
  painter.drawImage(target.toRect(), image, QRect(0, 0, image.width(), image.height()));
}

void WebShape::saveOdf(KoShapeSavingContext & context) const
{
  KoXmlWriter &writer = context.xmlWriter();

  writer.startElement( "braindump:web" );
  writer.addAttribute( "url", m_url.url());
  saveOdfAttributes( context, OdfAllAttributes );
  saveOdfCommonChildElements( context );
  writer.endElement(); // braindump:web
}

bool WebShape::loadOdf(const KoXmlElement & element, KoShapeLoadingContext &context)
{
  loadOdfAttributes( element, context, OdfAllAttributes );
  setUrl(element.attribute("url"));
  return true;
}

/*QWebPage* WebShape::webPage()
{
  return m_webPage;
}
*/

const KUrl& WebShape::url() {
  return m_url;
}

void WebShape::setUrl( const KUrl& _url) {
  m_url = _url;
  m_webPage->mainFrame()->load( _url );
}
