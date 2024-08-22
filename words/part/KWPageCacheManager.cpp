/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@kogmbh.com>
 * SPDX-FileCopyrightText: 2011 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
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
    : m_manager(manager)
    , m_tilesx(1)
    , m_tilesy(1)
    , m_size(w, h)
    , allExposed(true)
{
    if (w > MAX_TILE_SIZE || h > MAX_TILE_SIZE) {
        m_tilesx = w / MAX_TILE_SIZE;
        if (w % MAX_TILE_SIZE != 0)
            m_tilesx++;
        m_tilesy = h / MAX_TILE_SIZE;
        if (h % MAX_TILE_SIZE != 0)
            m_tilesy++;

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

KWPageCache::~KWPageCache() = default;

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
    KWPageCache *cache = nullptr;
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
    KWPageCache *cache = nullptr;
    if (!cache) {
        cache = new KWPageCache(this, size.width(), size.height());
    }
    return cache;
}

void KWPageCacheManager::clear()
{
    m_cache.clear();
}
