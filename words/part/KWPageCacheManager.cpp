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
    : cache(img)
{
    cache->fill(Qt::white);
    qDebug() << "Creating new cache with existing image" << this << "," << cache;
}

KWPageCache::KWPageCache(int w, int h)
    : allExposed(true)
{
    cache = new QImage(w, h, QImage::Format_RGB16);
    cache->fill(Qt::white);
    qDebug() << "creating new cache with new image" << this << "," << cache;
}


KWPageCache::~KWPageCache()
{
    // DO NOT DELETE THE CACHE IMAGE
    qDebug() << "deleting cache " << this << "returning image" << cache;
}

KWPageCacheManager::KWPageCacheManager(const QSize &size, int cacheSize)
    : m_cacheSize(cacheSize)
{
    //qDebug() << "creating new cache manager for size" << size << "creating " << cacheSize <<  "images";
    for (int i = 0; i < cacheSize; ++i) {
        QImage *img = new QImage(size, QImage::Format_RGB16);
        //qDebug() << "\tcreating new cache image" << img;
        m_imageQueue.enqueue(img);
    }
}

KWPageCacheManager::~KWPageCacheManager()
{
    //qDebug() << "delete page cache manager";
    clear();
}

KWPageCache *KWPageCacheManager::take(const KWPage page)
{
    KWPageCache *cache = 0;
    if (m_cache.contains(page)) {
        cache = m_cache.take(page);
        //cache->cache->save(QString("cache_%1.png").arg((int)cache->cache));
        qDebug() << __PRETTY_FUNCTION__ << cache << cache->allExposed << cache->exposed.size();
    }
    qDebug() << __PRETTY_FUNCTION__ << cache;
    return cache;
}

void KWPageCacheManager::insert(KWPage page, KWPageCache *cache)
{
    // first in, first out, let's keep it simple
    if (m_cache.size() > m_cacheSize) {
        KWPageCache *discard = m_cache.take(m_cache.keys().first());
        m_imageQueue.enqueue(discard->cache);
        delete discard;
    }
    qDebug() << __PRETTY_FUNCTION__ << cache;
    m_cache.insert(page, cache);
}

KWPageCache *KWPageCacheManager::cache(QSize size)
{
    //qDebug() << "creating new page cache";
    KWPageCache *cache = 0;
    while (!cache && m_imageQueue.size() > 0) {
        QImage *img = m_imageQueue.dequeue();
        if (img->size() == size) {
            cache = new KWPageCache(img);
        }
        else {
            delete img;
        }
    }
    if (!cache){
        cache = new KWPageCache(size.width(), size.height());
    }
    qDebug() << __PRETTY_FUNCTION__ << cache;
    return cache;
}

void KWPageCacheManager::clear()
{
    qDebug() << "clearing page cache manager";
    qDeleteAll(m_imageQueue);
    m_imageQueue.clear();
    foreach(KWPageCache *cache, m_cache.values()) {
        delete cache->cache;
        delete cache;
    }
    m_cache.clear();

}

