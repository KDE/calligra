/* This file is part of the KDE project
 * Copyright (C) 2012 Boudewijn Rempt <boud@kogmbh.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "RecentFileManager.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QUrl>
#include <QDebug>

#include <KSharedConfig>
#include <kconfiggroup.h>
#include <kconfig.h>


// Much of this is a gui-less clone of KRecentFilesAction, so the format of
// storing recent files is compatible.
struct RecentFileEntry {
    QString fileName;
    QString filePath;
};

class RecentFileManager::Private {
public:
    Private()
    {
        KConfigGroup grp(KSharedConfig::openConfig(), "RecentFiles");
        maxItems = grp.readEntry("maxRecentFileItems", 100);

        loadEntries(grp);
    }

    void loadEntries(const KConfigGroup &grp)
    {
        recents.clear();

        QString value;
        QString nameValue;
        QUrl url;

        KConfigGroup cg = grp;

        if ( cg.name().isEmpty()) {
            cg = KConfigGroup(cg.config(),"RecentFiles");
        }

        // read file list
        for (int i = 1; i <= maxItems; i++) {

            value = cg.readPathEntry(QString("File%1").arg(i), QString());
            if (value.isEmpty()) continue;
            url = QUrl(value);

            // gemini only handles local files
            // yes, i know the second half here isn't good on windows... but without it we fail on linux, and second part for windows
            if (!url.isLocalFile() && !value.startsWith('/') && value.midRef(2, 1) != QLatin1String(":")) {
                qDebug() << "Not a local file:" << url;
                continue;
            }

            // Don't restore if file doesn't exist anymore
            if (!QFile::exists(url.toLocalFile()) && !QFile::exists(value)) {
                qDebug() << "Recent file apparently no longer exists:" << url.toLocalFile();
                continue;
            }

            value = QDir::toNativeSeparators( value );

            // Don't restore where the url is already known (eg. broken config)
            for (const RecentFileEntry& entry : recents) {
                if (entry.filePath == value) {
                    continue;
                }
            }

            nameValue = cg.readPathEntry(QString("Name%1").arg(i), url.fileName());

            if (!value.isNull())  {
                RecentFileEntry entry;
                entry.fileName = nameValue;
                entry.filePath = value;
                recents << entry;
           }
        }
    }

    void saveEntries( const KConfigGroup &grp)
    {
        KConfigGroup cg = grp;

        if (cg.name().isEmpty()) {
            cg = KConfigGroup(cg.config(),"RecentFiles");
        }
        cg.deleteGroup();

        // write file list
        for (int i = 1; i <= recents.size(); ++i) {
            // i - 1 because we started from 1
            const RecentFileEntry &item = recents[i-1];
            cg.writePathEntry(QString("File%1").arg(i), item.filePath);
            cg.writePathEntry(QString("Name%1").arg(i), item.fileName);
        }
    }

    int maxItems;
    QList<RecentFileEntry> recents;
};




RecentFileManager::RecentFileManager(QObject *parent)
    : QObject(parent)
    , d(new Private())
{
}

RecentFileManager::~RecentFileManager()
{
    KConfigGroup grp(KSharedConfig::openConfig(), "RecentFiles");
    grp.writeEntry("maxRecentFileItems", d->maxItems);
    delete d;
}


QStringList RecentFileManager::recentFileNames() const
{
    QStringList files;
    for(const RecentFileEntry &item : d->recents) {
        files << item.fileName;
    }
    return files;
}

QStringList RecentFileManager::recentFiles() const
{
    QStringList files;
    for(const RecentFileEntry &item : d->recents) {
        files << item.filePath;
    }
    return files;
}

void RecentFileManager::addRecent(const QString &_url)
{
    if (d->recents.size() > d->maxItems) {
        d->recents.removeLast();
    }

    RecentFileEntry newEntry;
    newEntry.filePath = QDir::toNativeSeparators(_url);
    newEntry.fileName  = QFileInfo(_url).fileName();

    QMutableListIterator<RecentFileEntry> i(d->recents);
    while (i.hasNext()) {
        i.next();
        if (i.value().filePath == newEntry.filePath) {
            i.remove();
        }
    }
    d->recents.insert(0, newEntry);

    d->saveEntries(KConfigGroup(KSharedConfig::openConfig(), "RecentFiles"));
    emit recentFilesListChanged();
}

int RecentFileManager::size()
{
    return d->recents.size();
}

QString RecentFileManager::recentFile(int index) const
{
    if (index < d->recents.size()) {
        return d->recents.at(index).filePath;
    }
    return QString();
}

QString RecentFileManager::recentFileName(int index) const
{
    if (index < d->recents.size()) {
        return d->recents.at(index).fileName;
    }
    return QString();
}
