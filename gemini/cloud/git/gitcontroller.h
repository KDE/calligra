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

#include <QObject>
#include <QString>
#include <QAbstractListModel>
#include <QAction>
#include <QRunnable>

Q_DECLARE_METATYPE(QAbstractListModel*);

class GitController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString cloneDir READ cloneDir WRITE setCloneDir NOTIFY cloneDirChanged)
    Q_PROPERTY(QString currentFile READ currentFile WRITE setCurrentFile NOTIFY currentFileChanged)
    Q_PROPERTY(QAbstractListModel* documents READ documents NOTIFY documentsChanged)

    Q_PROPERTY(QString privateKeyFile READ privateKeyFile WRITE setPrivateKeyFile NOTIFY privateKeyFileChanged)
    Q_PROPERTY(bool needsPrivateKeyPassphrase READ needsPrivateKeyPassphrase WRITE setNeedsPrivateKeyPassphrase NOTIFY needsPrivateKeyPassphraseChanged)
    Q_PROPERTY(QString publicKeyFile READ publicKeyFile WRITE setPublicKeyFile NOTIFY publicKeyFileChanged)
    Q_PROPERTY(QString userForRemote READ userForRemote WRITE setUserForRemote NOTIFY userForRemoteChanged)
public:
    explicit GitController(QObject* parent = 0);
    virtual ~GitController();

    QString cloneDir() const;
    void setCloneDir(const QString& newDir);

    QString currentFile() const;
    void setCurrentFile(QString& newFile);

    QAbstractListModel* documents() const;

    QString privateKeyFile() const;
    void setPrivateKeyFile(QString newFile);

    QString publicKeyFile() const;
    void setPublicKeyFile(QString newFile);

    bool needsPrivateKeyPassphrase() const;
    void setNeedsPrivateKeyPassphrase(bool needsPassphrase);

    QString userForRemote() const;
    void setUserForRemote(QString newUser);

    // Update the local copy of the git repository in cloneDir() from the remote
    Q_INVOKABLE void pull();

public Q_SLOTS:
    QAction* commitAndPushCurrentFileAction();
    void commitAndPushCurrentFile();

    void clearOpThread();
    void disableCommitAndPushAction();
Q_SIGNALS:
    void operationBegun(QString message);
    void transferProgress(int progress);
    void pushCompleted();
    void pullCompleted();

    void cloneDirChanged();
    void currentFileChanged();
    void documentsChanged();

    void privateKeyFileChanged();
    void needsPrivateKeyPassphraseChanged();
    void publicKeyFileChanged();
    void userForRemoteChanged();
private:
    class Private;
    Private *d;
};

class git_signature;
class GitOpsThread : public QObject, public QRunnable
{
    Q_OBJECT
public:
    enum GitOperation {
        PullOperation,
        PushOperation
    };
    GitOpsThread(QString privateKey, QString publicKey, QString userForRemote, bool needsPrivateKeyPassphrase, git_signature* signature, QString gitDir, GitOperation operation, QString currentFile, QString message, QObject *parent = 0);
    ~GitOpsThread();

    void run();

    void abort();

Q_SIGNALS:
    void pushCompleted();
    void pullCompleted();
    void transferProgress(int progress);

private:
    void performPull();
    void performPush();
    class Private;
    Private * const d;
};

#endif // GITCONTROLLER_H
