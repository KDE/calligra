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

#include "gitlogmodel.h"

#include <qgit2.h>
#include <qgit2/qgitglobal.h>

#include <QDateTime>
#include <QDebug>

struct LogEntry {
public:
    LogEntry() {}
    QString authorName;
    QString authorEmail;
    QDateTime time;
    QString oid;
    QString shortMessage;
    QString message;
};

class GitLogModel::Private
{
public:
    Private() {}
    ~Private()
    {
        qDeleteAll(entries);
    }
    QString repoDir;
    QList<LogEntry*> entries;
};

GitLogModel::GitLogModel(QObject* parent)
    : QAbstractListModel(parent)
    , d(new Private)
{
    QHash<int, QByteArray> roleNames;
    roleNames[AuthorNameRole] = "authorName";
    roleNames[AuthorEmailRole] = "authorEmail";
    roleNames[TimeRole] = "time";
    roleNames[OIDRole] = "oid";
    roleNames[ShortMessageRole] = "shortMessage";
    roleNames[MessageRole] = "message";
    setRoleNames(roleNames);
}

GitLogModel::~GitLogModel()
{
    delete d;
}

int GitLogModel::rowCount(const QModelIndex &parent) const
{
    if ( parent.isValid() )
        return 0;
    return d->entries.count();
}

QVariant GitLogModel::data(const QModelIndex &index, int role) const
{
    QVariant data;
    if(index.isValid() && index.row() < d->entries.count()) {
        LogEntry * entry = d->entries.at(index.row());
        switch(role)
        {
            case AuthorNameRole:
                data = entry->authorName;
                break;
            case AuthorEmailRole:
                data = entry->authorEmail;
                break;
            case TimeRole:
                data = entry->time;
                break;
            case OIDRole:
                data = entry->oid;
                break;
            case ShortMessageRole:
                data = entry->shortMessage;
                break;
            case MessageRole:
                data = entry->message;
                break;
            default:
                data = "Unknown Role";
                break;
        }
    }
    return data;
}

QString GitLogModel::repoDir() const
{
    return d->repoDir;
}

void GitLogModel::setRepoDir(const QString& repoDir)
{
    if(d->repoDir != repoDir) {
        d->repoDir = repoDir;
        refreshLog();
        emit repoDirChanged();
    }
}

void GitLogModel::refreshLog()
{
    beginResetModel();
    qDeleteAll(d->entries);
    d->entries.clear();
    try {
        LibQGit2::Repository repo;
        repo.open(QString("%1/.git").arg(d->repoDir));

        LibQGit2::RevWalk rw(repo);

        rw.setSorting(LibQGit2::RevWalk::Topological);

        rw.pushHead();

        LibQGit2::Commit commit;
        // loop control, limit the run to the hundred most recent commits
        int i = 100;
        while(rw.next(commit)) {
            if(--i < 0) {
                break;
            }
            LogEntry* entry = new LogEntry();
            entry->authorName = commit.author().name();
            if(entry->authorName.isEmpty())
                entry->authorName = "Unknown";
            entry->authorEmail = commit.author().email();
            entry->time = commit.dateTime();
            entry->oid = commit.oid().format();
            entry->shortMessage = commit.shortMessage();
            entry->message = commit.message();
            d->entries.append(entry);
        }

    } catch (const LibQGit2::Exception& ex) {
        qDebug() << ex.what() << ex.category();
    }
    endResetModel();
}
