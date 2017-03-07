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
#ifndef KWPAGECACHEMANAGER_H
#define KWPAGECACHEMANAGER_H

#include "KWPage.h"
// Qt
#include <QCache>
#include <QImage>

class QSize;

class KWPageCacheManager;

class KWPageCache {


public:

    /// create a pagecache object with the existing
    /// QImage.
    //KWPageCache(KWPageCacheManager *manager, QImage *img);
    /// create a new pagecache object with a new QImage
    KWPageCache(KWPageCacheManager *manager, int w, int h);
    ~KWPageCache();

    KWPageCacheManager* m_manager;
    QVector<QImage> cache;
    int m_tilesx, m_tilesy;
    QSize m_size;
    // List of logical exposed rects in view coordinates
    // These are the rects that are queued for updating, not
    // the rects that have already been painted.
    QVector<QRect> exposed;
    // true if the whole page should be repainted
    bool allExposed;
};

class KWPageCacheManager {

public:

    explicit KWPageCacheManager(int cacheSize);

    ~KWPageCacheManager();

    KWPageCache *take(const KWPage &page);

    void insert(const KWPage &page, KWPageCache *cache);

    KWPageCache *cache(const QSize &size);

    void clear();

private:
    QCache<KWPage, KWPageCache> m_cache;
    friend class KWPageCache;
};

#endif
