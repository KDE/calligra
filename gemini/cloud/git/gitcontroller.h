/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 */

#ifndef GITCONTROLLER_H
#define GITCONTROLLER_H

#include <QAbstractListModel>
#include <QAction>
#include <QObject>
#include <QRunnable>
#include <QString>

Q_DECLARE_METATYPE(QAbstractListModel *);

class GitController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString cloneDir READ cloneDir WRITE setCloneDir NOTIFY cloneDirChanged)
    Q_PROPERTY(QString currentFile READ currentFile WRITE setCurrentFile NOTIFY currentFileChanged)
    Q_PROPERTY(QAbstractListModel *documents READ documents NOTIFY documentsChanged)

    Q_PROPERTY(QString privateKeyFile READ privateKeyFile WRITE setPrivateKeyFile NOTIFY privateKeyFileChanged)
    Q_PROPERTY(bool needsPrivateKeyPassphrase READ needsPrivateKeyPassphrase WRITE setNeedsPrivateKeyPassphrase NOTIFY needsPrivateKeyPassphraseChanged)
    Q_PROPERTY(QString publicKeyFile READ publicKeyFile WRITE setPublicKeyFile NOTIFY publicKeyFileChanged)
    Q_PROPERTY(QString userForRemote READ userForRemote WRITE setUserForRemote NOTIFY userForRemoteChanged)
public:
    explicit GitController(QObject *parent = nullptr);
    virtual ~GitController();

    QString cloneDir() const;
    void setCloneDir(const QString &newDir);

    QString currentFile() const;
    void setCurrentFile(QString &newFile);

    QAbstractListModel *documents() const;

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
    QAction *commitAndPushCurrentFileAction();
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
    enum GitOperation { PullOperation, PushOperation };
    GitOpsThread(QString privateKey,
                 QString publicKey,
                 QString userForRemote,
                 bool needsPrivateKeyPassphrase,
                 git_signature *signature,
                 QString gitDir,
                 GitOperation operation,
                 QString currentFile,
                 QString message,
                 QObject *parent = nullptr);
    ~GitOpsThread();

    void run() override;

    void abort();

Q_SIGNALS:
    void pushCompleted();
    void pullCompleted();
    void transferProgress(int progress);

private:
    void performPull();
    void performPush();
    class Private;
    Private *const d;
};

#endif // GITCONTROLLER_H
