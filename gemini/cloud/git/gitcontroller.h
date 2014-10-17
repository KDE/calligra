/* This file is part of the KDE project
 * Copyright 2014  Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef GITCONTROLLER_H
#define GITCONTROLLER_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QAbstractListModel>
#include <QtGui/QAction>

Q_DECLARE_METATYPE(QAbstractListModel*);

class GitController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString cloneDir READ cloneDir WRITE setCloneDir NOTIFY cloneDirChanged)
    Q_PROPERTY(QString currentFile READ currentFile WRITE setCurrentFile NOTIFY currentFileChanged)
    Q_PROPERTY(QAbstractListModel* documents READ documents NOTIFY documentsChanged)
public:
    explicit GitController(QObject* parent = 0);
    virtual ~GitController();

    QString cloneDir() const;
    void setCloneDir(const QString& newDir);

    QString currentFile() const;
    void setCurrentFile(QString& newFile);

    QAbstractListModel* documents() const;

public Q_SLOTS:
    QAction* commitAndPushCurrentFileAction();
    void commitAndPushCurrentFile();

Q_SIGNALS:
    void cloneDirChanged();
    void currentFileChanged();
    void documentsChanged();

private:
    class Private;
    Private *d;
};

#endif // GITCONTROLLER_H
