/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Fredy Yanardi <fyanardi@gmail.com>
 * SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoBookmarkManager.h"
#include "KoBookmark.h"

#include "TextDebug.h"
#include <QHash>

class KoBookmarkManagerPrivate
{
public:
    KoBookmarkManagerPrivate() = default;
    QHash<QString, KoBookmark *> bookmarkHash;
    QList<QString> bookmarkNameList;
    int lastId;
};

KoBookmarkManager::KoBookmarkManager()
    : d(new KoBookmarkManagerPrivate)
{
}

KoBookmarkManager::~KoBookmarkManager()
{
    delete d;
}

void KoBookmarkManager::insert(const QString &name, KoBookmark *bookmark)
{
    bookmark->setName(name);
    d->bookmarkHash[name] = bookmark;
    d->bookmarkNameList.append(name);
}

void KoBookmarkManager::remove(const QString &name)
{
    d->bookmarkHash.remove(name);
    d->bookmarkNameList.removeAll(name);
}

void KoBookmarkManager::rename(const QString &oldName, const QString &newName)
{
    QHash<QString, KoBookmark *>::iterator i = d->bookmarkHash.begin();

    while (i != d->bookmarkHash.end()) {
        if (i.key() == oldName) {
            KoBookmark *bookmark = d->bookmarkHash.take(i.key());
            bookmark->setName(newName);
            d->bookmarkHash.insert(newName, bookmark);
            int listPos = d->bookmarkNameList.indexOf(oldName);
            d->bookmarkNameList.replace(listPos, newName);
            return;
        }
        ++i;
    }
}

KoBookmark *KoBookmarkManager::bookmark(const QString &name) const
{
    KoBookmark *bookmark = d->bookmarkHash.value(name);
    return bookmark;
}

QList<QString> KoBookmarkManager::bookmarkNameList() const
{
    return d->bookmarkNameList;
}
