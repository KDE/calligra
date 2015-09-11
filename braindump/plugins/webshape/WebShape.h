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

#ifndef _WEBSHAPE_H_
#define _WEBSHAPE_H_

#include <QUrl>
#include <KoShape.h>

#define WEBSHAPEID "WebShape"

class QWebPage;

class WebShape : public QObject, public KoShape
{
    Q_OBJECT
public:
    WebShape();
    ~WebShape();

    // absolutly necessary:
    void paint(QPainter &painter,
               const KoViewConverter &converter, KoShapePaintingContext &paintcontext);
    virtual void saveOdf(KoShapeSavingContext & context) const;
    virtual bool loadOdf(const KoXmlElement & element, KoShapeLoadingContext &context);
    const QUrl &url();
    void setUrl(const QUrl &_url);
    bool isCached() const;
    void setCached(bool _cache);
    void setCache(const QString&);
    const QString& cache() const;
    void scrollOf(const QPointF& _scroll);
    void zoomOf(qreal z);
    QPointF scroll() const;
    void setScroll(const QPointF& point);
    qreal zoom() const;
    void setZoom(qreal _zoom);
private:
    void updateCache();
private Q_SLOTS:
    void loadFinished(bool);
private:
    QUrl m_url;
    QWebPage* m_webPage;
    bool m_cached;
    QString m_cache;
    bool m_cacheLocked;
    bool m_loaded;
    bool m_firstLoad;
    qreal m_zoom;
    QPointF m_scrollPosition;
};


#endif
