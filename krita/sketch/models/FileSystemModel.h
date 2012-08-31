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



#ifndef CALLIGRAMOBILE_FILESYSTEMMODEL_H
#define CALLIGRAMOBILE_FILESYSTEMMODEL_H

#include <QtDeclarative/QDeclarativeParserStatus>
#include <QtDeclarative/QtDeclarative>

#include <KDE/KDirSortFilterProxyModel>

class FileSystemModel : public KDirSortFilterProxyModel, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativeParserStatus)
    Q_PROPERTY(QString rootPath READ rootPath WRITE setRootPath)
    Q_PROPERTY(QString parentFolder READ parentFolder)
    Q_PROPERTY(QString filter READ filter WRITE setFilter)

public:
    enum FileRoles
    {
        FileNameRole = Qt::UserRole,
        FilePathRole,
        FileIconRole,
        FileTypeRole,
        FileThumbnailRole,
    };

    explicit FileSystemModel(QObject* parent = 0);
    virtual ~FileSystemModel();

    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    virtual void classBegin();
    virtual void componentComplete();

    virtual QString rootPath();
    virtual void setRootPath(const QString& path);

    virtual QString parentFolder();

    virtual QString filter();
    virtual void setFilter(const QString& filter);

private:
    class Private;
    Private * const d;
};

#endif // CALLIGRAMOBILE_FILESYSTEMMODEL_H
