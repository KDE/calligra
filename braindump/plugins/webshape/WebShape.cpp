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
#include <kdebug.h>

WebShape::WebShape() : m_webPage(new QWebPage), m_cached(false), m_cacheLocked(false), m_loaded(false), m_firstLoad(false)
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
  QSvgRenderer renderer(m_cache.toUtf8());
  renderer.render(&painter, target);
}

void WebShape::saveOdf(KoShapeSavingContext & context) const
{
  KoXmlWriter &writer = context.xmlWriter();

  writer.startElement( "braindump:web" );
  writer.addAttribute( "url", m_url.url());
  saveOdfAttributes( context, OdfAllAttributes );
  saveOdfCommonChildElements( context );
  if(m_cached)
  {
    writer.addAttribute( "cached", "true");
  }
  writer.startElement("cache");
  writer.addTextNode(m_cache); // Save after the attribute otherwise it is seen as 
  writer.endElement(); // Cache
  writer.endElement(); // braindump:web
}

bool WebShape::loadOdf(const KoXmlElement & element, KoShapeLoadingContext &context)
{
  loadOdfAttributes( element, context, OdfAllAttributes );
  setUrl(element.attribute("url"));
  if(element.attribute("cached") == "true")
  {
    m_cached = true;
    m_cacheLocked = true;
  } else {
    m_cached = false;
    m_cacheLocked = false;
  }
  KoXmlElement childElement;
  forEachElement(childElement, element) {
    if(childElement.tagName() == "cache")
    {
      m_cache = childElement.text();
      m_firstLoad = true;
    }
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
  if(not m_cacheLocked)
  {
    updateCache();
  }
  m_firstLoad = false;
}

void WebShape::updateCache() {
  QSvgGenerator svgGenerator;
  QBuffer buffer;
  svgGenerator.setOutputDevice(&buffer);
  QPainter painter(&svgGenerator);
  m_webPage->setViewportSize(m_webPage->mainFrame()->contentsSize());
  m_webPage->mainFrame()->render(&painter);
  painter.end();
  m_cache = buffer.data();
  m_cacheLocked = true;
}

bool WebShape::isCached() const {
  return m_cached;
}

void WebShape::setCached(bool _cache) {
  m_cached = _cache;
  if(m_cached) {
    m_cacheLocked = false;
    if(m_loaded)
    {
      updateCache();
    }
  }
}

void WebShape::setCache(const QString& _cache) {
  m_cache = _cache;
  m_cacheLocked = true;
}

const QString& WebShape::cache() const {
  return m_cache;
}

#include "WebShape.moc"
