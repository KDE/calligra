/* This file is part of the KDE project
 *
 * Copyright (c) 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
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



#include "FileSystemModel.h"
#include <KDE/KDirLister>
#include <KDE/KDirModel>

class FileSystemModel::Private
{
public:
    KDirModel* dirModel;
};

FileSystemModel::FileSystemModel(QObject* parent)
    : KDirSortFilterProxyModel(parent), d(new Private)
{
    d->dirModel = new KDirModel(this);
    setSourceModel(d->dirModel);
    setSortFoldersFirst(true);

    QHash<int, QByteArray> roles;
    roles.insert(FileNameRole, "fileName");
    roles.insert(FilePathRole, "path");
    roles.insert(FileIconRole, "icon");
    roles.insert(FileTypeRole, "fileType");
    roles.insert(FileThumbnailRole, "thumbnail");
    setRoleNames(roles);
}

FileSystemModel::~FileSystemModel()
{
    delete d;
}

QVariant FileSystemModel::data(const QModelIndex& index, int role) const
{
    if(index.isValid()) {
        KFileItem item = d->dirModel->itemForIndex(mapToSource(index));
        if(!item.isNull()) {
            switch(role) {
                case FileNameRole:
                    return item.text();
                    break;
                case FilePathRole:
                    return item.url().toLocalFile();
                    break;
                case FileIconRole:
                    return item.mimetype() == "inode/directory" ? "image://icon/inode-directory" : QString("image://recentimage/%1").arg(item.url().toLocalFile());
                    break;
                case FileTypeRole:
                    return item.mimetype();
                    break;
            }
        }
    }
    return KDirSortFilterProxyModel::data(index, role);
}

void FileSystemModel::classBegin()
{

}

void FileSystemModel::componentComplete()
{
    setRootPath(QDir::homePath());
}

QString FileSystemModel::rootPath()
{
    return d->dirModel->dirLister()->url().toLocalFile();
}

void FileSystemModel::setRootPath(const QString& path)
{
    d->dirModel->dirLister()->openUrl(KUrl::fromPath(path));
}

QString FileSystemModel::parentFolder()
{
    KUrl root = d->dirModel->dirLister()->url();
    root.cd("..");
    return root.toLocalFile();
}

QString FileSystemModel::filter()
{
    return d->dirModel->dirLister()->nameFilter();
}

void FileSystemModel::setFilter(const QString& filter)
{
    d->dirModel->dirLister()->setNameFilter(filter);
    d->dirModel->dirLister()->emitChanges();
}

