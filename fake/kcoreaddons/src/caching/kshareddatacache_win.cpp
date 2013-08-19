/*
 * This file is part of the KDE project.
 * Copyright © 2010 Michael Pyne <mpyne@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License version 2 as published by the Free Software Foundation.
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

/**
 * This is a horrifically simple implementation of KSharedDataCache that is
 * basically missing the "shared" part to it, for use on Windows or other platforms
 * that don't support POSIX.
 */
#include "kshareddatacache.h"

#include <QtCore/QString>
#include <QtCore/QByteArray>
#include <QtCore/QCache>

class KSharedDataCache::Private
{
    public:
    KSharedDataCache::EvictionPolicy evictionPolicy;
    QCache<QString, QByteArray> cache;
};

KSharedDataCache::KSharedDataCache(const QString &cacheName,
                 unsigned defaultCacheSize,
                 unsigned expectedItemSize)
    : d(new Private)
{
    d->cache.setMaxCost(defaultCacheSize);

    Q_UNUSED(cacheName);
    Q_UNUSED(expectedItemSize);
}

KSharedDataCache::~KSharedDataCache()
{
    delete d;
}

KSharedDataCache::EvictionPolicy KSharedDataCache::evictionPolicy() const
{
    return d->evictionPolicy;
}

void KSharedDataCache::setEvictionPolicy(KSharedDataCache::EvictionPolicy newPolicy)
{
    d->evictionPolicy = newPolicy;
}

bool KSharedDataCache::insert(const QString &key, const QByteArray &data)
{
    return d->cache.insert(key, new QByteArray(data));
}

bool KSharedDataCache::find(const QString &key, QByteArray *destination) const
{
    QByteArray *value = d->cache.object(key);

    if (value) {
        destination = value;
        return true;
    }
    else {
        return false;
    }
}

void KSharedDataCache::clear()
{
    d->cache.clear();
}

void KSharedDataCache::deleteCache(const QString &cacheName)
{
    Q_UNUSED(cacheName);
}

bool KSharedDataCache::contains(const QString &key) const
{
    return d->cache.contains(key);
}

unsigned KSharedDataCache::totalSize() const
{
    return static_cast<unsigned>(d->cache.maxCost());
}

unsigned KSharedDataCache::freeSize() const
{
    if (d->cache.totalCost() < d->cache.maxCost()) {
        return static_cast<unsigned>(d->cache.maxCost() - d->cache.totalCost());
    }
    else {
        return 0;
    }
}

unsigned KSharedDataCache::timestamp() const
{
    return 0;
}

void KSharedDataCache::setTimestamp(unsigned newTimestamp)
{
}
