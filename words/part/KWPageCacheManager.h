/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@kogmbh.com>
 * SPDX-FileCopyrightText: 2011 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KWPAGECACHEMANAGER_H
#define KWPAGECACHEMANAGER_H

#include "KWPage.h"
// Qt
#include <QCache>
#include <QImage>

class QSize;

class KWPageCacheManager;

class KWPageCache
{
public:
    /// create a pagecache object with the existing
    /// QImage.
    // KWPageCache(KWPageCacheManager *manager, QImage *img);
    /// create a new pagecache object with a new QImage
    KWPageCache(KWPageCacheManager *manager, int w, int h);
    ~KWPageCache();

    KWPageCacheManager *m_manager;
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

class KWPageCacheManager
{
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
