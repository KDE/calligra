/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Boudewijn Rempt <boud@kogmbh.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "RecentFilesModel.h"

#include "RecentFileManager.h"

#include <QDateTime>
#include <QFile>
#include <QFileInfo>

class RecentFilesModel::Private
{
public:
    RecentFileManager *recentFileManager;
};

RecentFilesModel::RecentFilesModel(QObject *parent)
    : QAbstractListModel(parent)
    , d(new Private())
{
    d->recentFileManager = nullptr;
}

RecentFilesModel::~RecentFilesModel()
{
    delete d;
}

QHash<int, QByteArray> RecentFilesModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ImageRole] = "image";
    roles[TextRole] = "text";
    roles[UrlRole] = "url";
    roles[NameRole] = "name";
    roles[DateRole] = "filedate";
    return roles;
}

int RecentFilesModel::rowCount(const QModelIndex & /*parent*/) const
{
    if (d->recentFileManager)
        return d->recentFileManager->size();
    else
        return 0;
}

QVariant RecentFilesModel::data(const QModelIndex &index, int role) const
{
    QVariant result;
    if (!d->recentFileManager)
        return result;
    if (index.isValid()) {
        Q_ASSERT(index.row() < d->recentFileManager->size());

        QString key = d->recentFileManager->recentFileName(index.row());
        QString value = d->recentFileManager->recentFile(index.row());

        switch (role) {
        case ImageRole:
            result = QString("image://recentimage/%1").arg(value);
            break;
        case TextRole:
            result = QFileInfo(value).completeBaseName();
            break;
        case UrlRole:
            result = value;
            break;
        case NameRole:
            result = key;
        case DateRole: {
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

QVariant RecentFilesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation);
    QVariant result;
    if (section == 0) {
        switch (role) {
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

QObject *RecentFilesModel::recentFileManager() const
{
    return d->recentFileManager;
}

void RecentFilesModel::setRecentFileManager(QObject *recentFileManager)
{
    disconnect(d->recentFileManager);
    d->recentFileManager = qobject_cast<RecentFileManager *>(recentFileManager);
    connect(d->recentFileManager, &RecentFileManager::recentFilesListChanged, this, &RecentFilesModel::recentFilesListChanged);
    emit recentFileManagerChanged();
}

void RecentFilesModel::recentFilesListChanged()
{
    beginResetModel();
    endResetModel();
}

void RecentFilesModel::addRecent(const QString &_url)
{
    if (d->recentFileManager)
        d->recentFileManager->addRecent(_url);
}
