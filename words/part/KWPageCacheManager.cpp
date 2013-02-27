/* This file is part of the KDE project
 * Copyright (C) 2011 Boudewijn Rempt <boud@kogmbh.com>
 * Copyright (C) 2011 Marijn Kruisselbrink <mkruisselbrink@kde.org>
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

#include "KWPageCacheManager.h"

#include <QImage>

static const int MAX_TILE_SIZE = 1024;

/*
KWPageCache::KWPageCache(KWPageCacheManager *manager, QImage *img)
    : m_manager(manager), cache(img), allExposed(true)
{
    cache->fill(0xffff);
}*/

KWPageCache::KWPageCache(KWPageCacheManager *manager, int w, int h)
    : m_manager(manager), m_tilesx(1), m_tilesy(1), m_size(w, h), allExposed(true)
{
    if (w > MAX_TILE_SIZE || h > MAX_TILE_SIZE) {
        m_tilesx = w / MAX_TILE_SIZE;
        if (w % MAX_TILE_SIZE != 0) m_tilesx++;
        m_tilesy = h / MAX_TILE_SIZE;
        if (h % MAX_TILE_SIZE != 0) m_tilesy++;

        for (int x = 0; x < m_tilesx; x++) {
            for (int y = 0; y < m_tilesy; y++) {
                int tilew = qMin(MAX_TILE_SIZE, w - x * MAX_TILE_SIZE);
                int tileh = qMin(MAX_TILE_SIZE, h - y * MAX_TILE_SIZE);
                cache.push_back(QImage(tilew, tileh, QImage::Format_RGB16));
            }
        }
    } else {
        cache.push_back(QImage(w, h, QImage::Format_RGB16));
    }
}


KWPageCache::~KWPageCache()
{
}

KWPageCacheManager::KWPageCacheManager(int cacheSize)
    : m_cache(cacheSize)
{
}

KWPageCacheManager::~KWPageCacheManager()
{
    clear();
}

KWPageCache *KWPageCacheManager::take(const KWPage &page)
{
    KWPageCache *cache = 0;
    if (m_cache.contains(page)) {
        cache = m_cache.take(page);
    }
    return cache;
}

void KWPageCacheManager::insert(const KWPage &page, KWPageCache *cache)
{
    QSize size = cache->m_size;
    // make sure always at least two pages can be cached
    m_cache.insert(page, cache, qMin(m_cache.maxCost() / 2, size.width() * size.height()));
}

KWPageCache *KWPageCacheManager::cache(const QSize &size)
{
    KWPageCache *cache = 0;
    if (!cache){
        cache = new KWPageCache(this, size.width(), size.height());
    }
    return cache;
}

void KWPageCacheManager::clear()
{
    m_cache.clear();
}

