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

#ifndef CALLIGRAGEMINI_GIT_GITLOGMODEL_H
#define CALLIGRAGEMINI_GIT_GITLOGMODEL_H

#include <QAbstractListModel>

class GitLogModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString repoDir READ repoDir WRITE setRepoDir NOTIFY repoDirChanged)
public:
    enum CustomRoles {
        AuthorNameRole = Qt::UserRole + 1,
        AuthorEmailRole,
        TimeRole,
        OIDRole,
        ShortMessageRole,
        MessageRole
    };

    explicit GitLogModel(QObject *parent = 0);
    virtual ~GitLogModel();

    // Reimplemented from QAbstractListModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    QString repoDir() const;
    void setRepoDir(const QString& repoDir);

public Q_SLOTS:
    void refreshLog();

Q_SIGNALS:
    void repoDirChanged();

private:
    class Private;
    Private * const d;
};

#endif // CALLIGRAGEMINI_GIT_GITLOGMODEL_H
