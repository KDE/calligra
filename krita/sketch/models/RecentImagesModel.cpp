/* This file is part of the KDE project
 * Copyright (C) 2012 Boudewijn Rempt <boud@kogmbh.com>
 *           (C) 1999 Reginald Stadlbauer <reggie@kde.org>
 *           (C) 1999 Simon Hausmann <hausmann@kde.org>
 *           (C) 2000 Nicolas Hadacek <haadcek@kde.org>
 *           (C) 2000 Kurt Granroth <granroth@kde.org>
 *           (C) 2000 Michael Koch <koch@kde.org>
 *           (C) 2001 Holger Freyther <freyther@kde.org>
 *           (C) 2002 Ellis Whitehead <ellis@kde.org>
 *           (C) 2002 Joseph Wenninger <jowenn@kde.org>
 *           (C) 2003 Andras Mantia <amantia@kde.org>
 *           (C) 2005-2006 Hamish Rodda <rodda@kde.org>
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
#include "RecentImagesModel.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>

#include <KoResourceServerAdapter.h>

#include <kis_view2.h>

#include <kurl.h>
#include <kglobal.h>
#include <kconfiggroup.h>
#include <kconfig.h>
#include <klocale.h>
#include <kstandarddirs.h>

// Much of this is a gui-less clone of KRecentFilesAction, so the format of
// storing recent files is compatible.
class RecentImagesModel::Private {
public:
    Private()
    {
        KConfigGroup grp(KGlobal::config(), "RecentFiles");
        maxItems = grp.readEntry("maxRecentFileItems", 100);

        loadEntries(KConfigGroup(KGlobal::config(), "RecentFiles"));
    }

    void loadEntries(const KConfigGroup &grp)
    {
        recentFiles.clear();
        recentFilesIndex.clear();

        QString value;
        QString nameValue;
        KUrl url;

        KConfigGroup cg = grp;

        if ( cg.name().isEmpty()) {
            cg = KConfigGroup(cg.config(),"RecentFiles");
        }

        // read file list
        for (int i = 1; i <= maxItems; i++) {

            value = cg.readPathEntry(QString("File%1").arg(i), QString());
            if (value.isEmpty()) continue;
            url = KUrl(value);

            // krita sketch only handles local files
            if (!url.isLocalFile())
                continue;

            // Don't restore if file doesn't exist anymore
            if (!QFile::exists(url.toLocalFile()))
                continue;

            value = QDir::toNativeSeparators( value );

            // Don't restore where the url is already known (eg. broken config)
            if (recentFiles.contains(value))
                continue;

            nameValue = cg.readPathEntry(QString("Name%1").arg(i), url.fileName());

            if (!value.isNull())  {
                recentFilesIndex << nameValue;
                recentFiles << value;
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
        for (int i = 1; i <= recentFilesIndex.size(); ++i) {
            // i - 1 because we started from 1
            cg.writePathEntry(QString("File%1").arg(i), recentFiles[i - 1]);
            cg.writePathEntry(QString("Name%1").arg(i), recentFilesIndex[i - 1]);
        }
    }

    int maxItems;
    QStringList recentFilesIndex;
    QStringList recentFiles;
};

RecentImagesModel::RecentImagesModel(QObject *parent)
    : QAbstractListModel(parent)
    , d(new Private())
{
    QHash<int, QByteArray> roles;
    roles[ImageRole] = "image";
    roles[TextRole] = "text";
    roles[UrlRole] = "url";
    roles[NameRole] = "name";
    roles[DateRole] = "date";
    setRoleNames(roles);
}

RecentImagesModel::~RecentImagesModel()
{
    KConfigGroup grp(KGlobal::config(), "RecentFiles");
    grp.writeEntry("maxRecentFileItems", d->maxItems);
    delete d;
}

int RecentImagesModel::rowCount(const QModelIndex &/*parent*/) const
{
    return d->recentFiles.size();
}

QVariant RecentImagesModel::data(const QModelIndex &index, int role) const
{
    QVariant result;
    if (index.isValid())
    {
        Q_ASSERT(index.row() < d->recentFiles.size());

        QString key = d->recentFilesIndex.at(index.row());
        QString value = d->recentFiles.at(index.row());

        switch(role)
        {
        case ImageRole:
            result = QString("image://recentimage/%1").arg(value);
            break;
        case TextRole:
            result = key;
            break;
        case UrlRole:
            result = value;
            break;
        case NameRole:
            result = key;
        case DateRole:
        {
            QFile f(value);
            if (f.exists()) {
                QFileInfo fi(value);
                result = fi.lastModified().toString("dd-mm-yyyy (hh:mm)");
            }
        }
        default:
            result = "";
            break;
        }
    }
    return result;
}

QVariant RecentImagesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation);
    QVariant result;
    if (section == 0)
    {
        switch(role)
        {
        case ImageRole:
            result = QString("Thumbnail");
            break;
        case TextRole:
            result = QString("Name");
            break;
        case UrlRole:
        case NameRole:
        case DateRole:
        default:
            result = "";
            break;
        }
    }
    return result;
}

void RecentImagesModel::addRecent(const KUrl &url)
{
    if (d->recentFiles.size() > d->maxItems) {
        d->recentFiles.removeLast();
        d->recentFilesIndex.removeLast();
    }
    d->recentFiles.insert(0, QDir::toNativeSeparators(url.toLocalFile()));
    d->recentFilesIndex.insert(0, QFile(url.toLocalFile()).fileName());
}

#include "RecentImagesModel.moc"
