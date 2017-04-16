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

// #include <qgit2.h>
// #include <qgit2/qgitglobal.h>

#include <git2.h>
#include <git2/repository.h>

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

    git_repository* repository;
    int error = git_repository_open(&repository, QString("%1/.git").arg(d->repoDir).toLatin1());
    if(error != 0) { const git_error* err = giterr_last(); qDebug() << "Kapow, error code from git2 was" << error << "which is described as" << err->message; return; }

    git_revwalk *walker;
    error = git_revwalk_new(&walker, repository);
    if(error != 0) { const git_error* err = giterr_last(); qDebug() << "Kapow, error code from git2 was" << error << "which is described as" << err->message; return; }
    error = git_revwalk_push_range(walker, "HEAD~100..HEAD");
    if(error != 0) { const git_error* err = giterr_last(); qDebug() << "Kapow, error code from git2 was" << error << "which is described as" << err->message; return; }

    git_oid oid;
    git_commit *commit = NULL;
    while (git_revwalk_next(&oid, walker) == 0) {
        error = git_commit_lookup(&commit, repository, &oid);
        if(error != 0) { const git_error* err = giterr_last(); qDebug() << "Kapow, error code from git2 was" << error << "which is described as" << err->message; return; }

        const git_signature *author = git_commit_author(commit);

        LogEntry* entry = new LogEntry();
        entry->authorName = author->name;
        if(entry->authorName.isEmpty())
            entry->authorName = "Unknown";
        entry->authorEmail = author->email;

        git_time_t time = git_commit_time(commit);
        entry->time = QDateTime::fromMSecsSinceEpoch(time * 1000);

        entry->oid = QString::fromAscii(git_oid_tostr_s(git_commit_id(commit)));
        entry->message = QString::fromAscii(git_commit_message(commit));
        entry->shortMessage = entry->message.left(120).split(QRegExp("(\\r|\\n)")).first();

        d->entries.append(entry);

        git_commit_free(commit);
    }
    git_repository_free(repository);
    endResetModel();
}
