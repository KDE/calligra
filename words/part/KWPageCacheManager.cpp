/* This file is part of the KDE project
 * Copyright (C) 2011 Boudewijn Rempt <boud@kogmbh.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <QCache>
#include <QRectF>
#include <QImage>
#include <QQueue>
#include <QSize>
#include <QObject>

#include "KWPageCacheManager.h"

KWPageCache::KWPageCache(QImage *img)
{
    cache = img;
    cache->fill(Qt::white);
}

KWPageCache::KWPageCache(int w, int h)
    : allExposed(true)
{
    cache = new QImage(w, h, QImage::Format_ARGB32);
    cache->fill(Qt::white);
}


KWPageCache::~KWPageCache()
{
    // DO NOT DELETE THE CACHE IMAGE
}

KWPageCacheManager::KWPageCacheManager(const QSize &size, int weight, qreal scale)
    : m_destructing(false)
{
    for (int i = 0; i < weight / scale; ++i) {
        QImage *img = new QImage(size, QImage::Format_ARGB32);
        m_imageQueue.enqueue(img);
    }
}

KWPageCacheManager::~KWPageCacheManager()
{
    m_destructing = true;
    clear();
}

KWPageCache *KWPageCacheManager::take(const KWPage page)
{
    return m_cache.take(page);
}

void KWPageCacheManager::insert(KWPage page, KWPageCache *cache, int weight)
{
    m_cache.insert(page, cache, weight);
}

KWPageCache *KWPageCacheManager::cache(QSize size)
{
    KWPageCache *cache = 0;
    while (!cache && m_imageQueue.size() > 0) {
        QImage *img = m_imageQueue.dequeue();
        if (img->size() == size) {
            cache = new KWPageCache(img);
        }
    }
    if (!cache){
        cache = new KWPageCache(size.width(), size.height());
    }
    connect(cache, SIGNAL(destroyed(QObject*)), this, SLOT(queueImage(QObject*)));
    m_cacheMap.insert(cache, cache->cache);
    return cache;
}

void KWPageCacheManager::clear()
{
    qDeleteAll(m_imageQueue);
    m_imageQueue.clear();

    m_cache.clear();

    qDeleteAll(m_cacheMap);
    m_cacheMap.clear();


}

void KWPageCacheManager::queueImage(QObject *obj)
{
    if (m_cacheMap.contains(obj)) {
        m_imageQueue.enqueue(m_cacheMap.take(obj));
    }
}

