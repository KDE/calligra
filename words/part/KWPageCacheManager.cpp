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

KWPageCache::KWPageCache(KWPageCacheManager *manager, QImage *img)
    : m_manager(manager), cache(img), allExposed(true)
{
    cache->fill(0xffff);
}

KWPageCache::KWPageCache(KWPageCacheManager *manager, int w, int h)
    : m_manager(manager), allExposed(true)
{
    cache = new QImage(w, h, QImage::Format_RGB16);
    cache->fill(0xffff);
}


KWPageCache::~KWPageCache()
{
    // keep the image around in the manager
    m_manager->m_imageQueue.enqueue(cache);
}

KWPageCacheManager::KWPageCacheManager(int cacheSize)
    : m_cache(cacheSize)
{
}

KWPageCacheManager::~KWPageCacheManager()
{
    clear();
}

KWPageCache *KWPageCacheManager::take(const KWPage page)
{
    KWPageCache *cache = 0;
    if (m_cache.contains(page)) {
        cache = m_cache.take(page);
    }
    return cache;
}

void KWPageCacheManager::insert(KWPage page, KWPageCache *cache)
{
    QSize size = cache->cache->size();
    // make sure always at least two pages can be cached
    m_cache.insert(page, cache, qMin(m_cache.maxCost() / 2, size.width() * size.height()));
}

KWPageCache *KWPageCacheManager::cache(QSize size)
{
    KWPageCache *cache = 0;
    while (!cache && m_imageQueue.size() > 0) {
        QImage *img = m_imageQueue.dequeue();
        if (img->size() == size) {
            cache = new KWPageCache(this, img);
        }
        else {
            delete img;
        }
    }
    if (!cache){
        cache = new KWPageCache(this, size.width(), size.height());
    }
    return cache;
}

void KWPageCacheManager::clear()
{
    qDeleteAll(m_imageQueue);
    m_imageQueue.clear();
    m_cache.clear();
}

