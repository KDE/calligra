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

#include <QBuffer>
#include <QPainter>
#include <QSvgGenerator>
#include <QSvgRenderer>
#include <QWebPage>
#include <QWebFrame>

#include <KoShapeSavingContext.h>
#include <KoViewConverter.h>
#include <KoXmlWriter.h>
#include <KoXmlReader.h>

WebShape::WebShape() : m_webPage(new QWebPage), m_cached(false), m_cacheLocked(false), m_loaded(false)
{
  connect(m_webPage, SIGNAL(loadFinished(bool)), SLOT(loadFinished(bool)));
}

WebShape::~WebShape()
{
}

void WebShape::paint( QPainter &painter,
                const KoViewConverter &converter )
{
  QRectF target = converter.documentToView(QRectF(QPointF(0,0), size()));
  if(m_cached and m_cacheLocked) {
    QSvgRenderer renderer(m_cache.toUtf8());
    renderer.render(&painter, target);
  } else {
    m_webPage->setViewportSize(m_webPage->mainFrame()->contentsSize());
    QImage image(m_webPage->viewportSize(), QImage::Format_ARGB32);
    QPainter imgPainter(&image);

    m_webPage->mainFrame()->render(&imgPainter);
    imgPainter.end();

    painter.drawImage(target.toRect(), image, QRect(0, 0, image.width(), image.height()));
  }
}

void WebShape::saveOdf(KoShapeSavingContext & context) const
{
  KoXmlWriter &writer = context.xmlWriter();

  writer.startElement( "braindump:web" );
  writer.addAttribute( "url", m_url.url());
  if(m_cached)
  {
    writer.addAttribute( "cached", m_cached);
    writer.addTextNode(m_cache);
  }
  saveOdfAttributes( context, OdfAllAttributes );
  saveOdfCommonChildElements( context );
  writer.endElement(); // braindump:web
}

bool WebShape::loadOdf(const KoXmlElement & element, KoShapeLoadingContext &context)
{
  loadOdfAttributes( element, context, OdfAllAttributes );
  setUrl(element.attribute("url"));
  if(element.attribute("cached") == "true")
  {
    m_cache = element.text();
    m_cached = true;
    m_cacheLocked = true;
  } else {
    m_cached = false;
    m_cacheLocked = false;
  }
  return true;
}

const KUrl& WebShape::url() {
  return m_url;
}

void WebShape::setUrl( const KUrl& _url) {
  m_url = _url;
  m_webPage->mainFrame()->load( _url );
  
  notifyChanged();
  update();
  m_loaded = false;
  m_cacheLocked = false;
}

void WebShape::loadFinished(bool) {
  update();
  m_loaded = true;
  if(m_cached and not m_cacheLocked)
  {
    QSvgGenerator svgGenerator;
    QBuffer buffer;
    svgGenerator.setOutputDevice(&buffer);
    QPainter painter(&svgGenerator);
    m_webPage->mainFrame()->render(&painter);
    painter.end();
    m_cache = buffer.data();
    m_cacheLocked = true;
  }
}


#include "WebShape.moc"
