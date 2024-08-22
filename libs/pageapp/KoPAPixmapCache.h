/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOPAPIXMAPCACHE_H
#define KOPAPIXMAPCACHE_H

#include <QMap>
#include <QSize>

class QString;
class QPixmap;

/**
 * This class is a cache for pixmaps which will be cached for different sizes
 * of the same pixmap If a key is removed from the cache all cached sizes will
 * be removed from the cache.
 *
 * The API is similar to QPixmpaCache. The only addition is that you need to
 * specify the size of the pixmap when you search it.
 *
 * The implementation uses QPixmapCache.
 *
 * This class is a singleton.
 */
class KoPAPixmapCache
{
public:
    class Singleton;

    /**
     * Get the pixmap cache singleton
     */
    static KoPAPixmapCache *instance();

    ~KoPAPixmapCache();

    /**
     * Returns the cache limit (in kilobytes)
     */
    int cacheLimit();

    /**
     * Removes all pixmaps from the cache.
     *
     * @param all If true QPixmpaCache::clear will be called.
     *            If false only the pixmaps which were added via this object
     *            will be removed
     */
    void clear(bool all = true);

    /**
     * Looks for a cached pixmap associated with the key in the cache.
     *
     * If the pixmap is found, the function sets pm to that pixmap and returns true;
     * otherwise it leaves pm alone and returns false.
     *
     * @param key the key of the pixmap
     * @param size the size you want to have the pixmap
     * @param pm the pixmap
     */
    bool find(const QString &key, const QSize &size, QPixmap *pm);

    /**
     * Insert a copy of the pixmap into the cache.
     *
     * The size is taken from the pixmap.
     */
    bool insert(const QString &key, const QPixmap &pm, const QSize &size = QSize());

    /**
     * Remove all pixmaps associated with key from the cache
     */
    void remove(const QString &key);

    /**
     * Sets the cache limit to n kilobytes
     */
    void setCacheLimit(int n);

private:
    KoPAPixmapCache();
    KoPAPixmapCache(const KoPAPixmapCache &) = delete;
    KoPAPixmapCache operator=(const KoPAPixmapCache &) = delete;

    QString generateKey(const QString &key, const QSize &size);
    QMap<QString, QVector<QSize>> m_keySize;
};

#endif /* KOPAPIXMAPCACHE_H */
