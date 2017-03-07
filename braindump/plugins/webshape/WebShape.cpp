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

#include <../../src/Xml.h>

WebShape::WebShape() : m_webPage(new QWebPage), m_cached(false), m_cacheLocked(false), m_loaded(false), m_firstLoad(false), m_zoom(1.0), m_scrollPosition(0, 0)
{
    m_webPage->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
    m_webPage->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
    connect(m_webPage, SIGNAL(loadFinished(bool)), SLOT(loadFinished(bool)));
}

WebShape::~WebShape()
{
}

void WebShape::paint(QPainter &painter,
                     const KoViewConverter &converter, KoShapePaintingContext &)
{
    QRectF target = converter.documentToView(QRectF(QPointF(0, 0), size()));
    m_webPage->setViewportSize(target.size().toSize());
    qreal cz = target.width() / size().width();
    m_webPage->mainFrame()->setZoomFactor(m_zoom * cz);
    m_webPage->mainFrame()->setScrollPosition(m_scrollPosition.toPoint());
    m_webPage->mainFrame()->render(&painter);
}

void WebShape::saveOdf(KoShapeSavingContext & context) const
{
    KoXmlWriter &writer = context.xmlWriter();

    writer.startElement("braindump:web");
    Xml::writeBraindumpNS(writer);
    writer.addAttribute("url", m_url.url());
    writer.addAttribute("scroll_x", m_scrollPosition.x());
    writer.addAttribute("scroll_y", m_scrollPosition.y());
    writer.addAttribute("zoom", m_zoom);
    saveOdfAttributes(context, OdfAllAttributes);
    saveOdfCommonChildElements(context);
    if(m_cached) {
        writer.addAttribute("cached", "true");
    }
    writer.startElement("cache");
    writer.addTextNode(m_cache); // Save after the attribute otherwise it is seen as
    writer.endElement(); // Cache
    writer.endElement(); // braindump:web
}

bool WebShape::loadOdf(const KoXmlElement & element, KoShapeLoadingContext &context)
{
    loadOdfAttributes(element, context, OdfAllAttributes);
    m_url.setUrl(element.attribute("url"));
    m_scrollPosition.setX(element.attribute("scroll_x", "0").toDouble());
    m_scrollPosition.setY(element.attribute("scroll_y", "0").toDouble());
    m_zoom = element.attribute("zoom", "1.0").toDouble();
    if(element.attribute("cached") == "true") {
        m_cached = true;
        m_cacheLocked = true;
    } else {
        m_cached = false;
        m_cacheLocked = false;
    }
    KoXmlElement childElement;
    forEachElement(childElement, element) {
        if(childElement.tagName() == "cache") {
            m_cache = childElement.text();
            m_firstLoad = true;
            m_webPage->mainFrame()->setContent(m_cache.toUtf8());
        }
    }
    if(!m_cached) {
        setUrl(m_url);
    }
    return true;
}

const QUrl &WebShape::url()
{
    return m_url;
}

void WebShape::setUrl(const QUrl &_url)
{
    m_url = _url;
    m_webPage->mainFrame()->load(_url);

    notifyChanged();
    update();
    m_loaded = false;
    m_cacheLocked = false;
}

void WebShape::loadFinished(bool)
{
    update();
    m_loaded = true;
    if(!m_cacheLocked) {
        updateCache();
    }
    m_firstLoad = false;
}

void WebShape::updateCache()
{
    m_cache = m_webPage->mainFrame()->toHtml();
    m_cacheLocked = true;
}

bool WebShape::isCached() const
{
    return m_cached;
}

void WebShape::setCached(bool _cache)
{
    m_cached = _cache;
    if(m_cached) {
        m_cacheLocked = false;
        if(m_loaded) {
            updateCache();
        }
    } else {
        m_webPage->mainFrame()->load(m_url);
    }
    update();
}

void WebShape::setCache(const QString& _cache)
{
    m_cache = _cache;
    m_cacheLocked = true;
    m_webPage->mainFrame()->setContent(_cache.toUtf8());
    update();
}

const QString& WebShape::cache() const
{
    return m_cache;
}

void WebShape::scrollOf(const QPointF& _scroll)
{
    m_scrollPosition += _scroll / m_zoom;
}

void WebShape::zoomOf(qreal z)
{
    m_zoom *= z;
    if(m_zoom <= 0.01) m_zoom = 0.01;
}

QPointF WebShape::scroll() const
{
    return m_scrollPosition;
}

void WebShape::setScroll(const QPointF& point)
{
    m_scrollPosition = point;
}

qreal WebShape::zoom() const
{
    return m_zoom;
}

void WebShape::setZoom(qreal _zoom)
{
    m_zoom = _zoom;
}
