/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoPAPixmapCache.h"

#include <QGlobalStatic>
#include <QPixmapCache>

class KoPAPixmapCache::Singleton
{
public:
    KoPAPixmapCache q;
};

Q_GLOBAL_STATIC(KoPAPixmapCache::Singleton, singleton)

KoPAPixmapCache *KoPAPixmapCache::instance()
{
    return &(singleton->q);
}

KoPAPixmapCache::KoPAPixmapCache() = default;

KoPAPixmapCache::~KoPAPixmapCache() = default;

int KoPAPixmapCache::cacheLimit()
{
    return QPixmapCache::cacheLimit();
}

void KoPAPixmapCache::clear(bool all)
{
    if (all) {
        QPixmapCache::clear();
    } else {
        QMap<QString, QVector<QSize>>::ConstIterator it(m_keySize.constBegin());

        for (; it != m_keySize.constEnd(); ++it) {
            foreach (const QSize &size, it.value()) {
                QString k = generateKey(it.key(), size);
                QPixmapCache::remove(k);
            }
        }
        m_keySize.clear();
    }
}

bool KoPAPixmapCache::find(const QString &key, const QSize &size, QPixmap *pm)
{
    QString k = generateKey(key, size);
    return QPixmapCache::find(k, pm);
}

bool KoPAPixmapCache::insert(const QString &key, const QPixmap &pm, const QSize &size)
{
    QString k = generateKey(key, size.isValid() ? size : pm.size());
    m_keySize[key].append(size.isValid() ? size : pm.size());
    return QPixmapCache::insert(k, pm);
}

void KoPAPixmapCache::remove(const QString &key)
{
    QMap<QString, QVector<QSize>>::iterator it(m_keySize.find(key));

    if (it != m_keySize.end()) {
        foreach (const QSize &size, it.value()) {
            QString k = generateKey(key, size);
            QPixmapCache::remove(k);
        }
        m_keySize.erase(it);
    }
}

void KoPAPixmapCache::setCacheLimit(int n)
{
    QPixmapCache::setCacheLimit(n);
}

QString KoPAPixmapCache::generateKey(const QString &key, const QSize &size)
{
    return QString("%1-%2-%3").arg(key).arg(size.width()).arg(size.height());
}
