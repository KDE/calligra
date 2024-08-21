/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
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
    enum CustomRoles { AuthorNameRole = Qt::UserRole + 1, AuthorEmailRole, TimeRole, OIDRole, ShortMessageRole, MessageRole };

    explicit GitLogModel(QObject *parent = nullptr);
    virtual ~GitLogModel();

    // Reimplemented from QAbstractListModel
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QString repoDir() const;
    void setRepoDir(const QString &repoDir);

public Q_SLOTS:
    void refreshLog();

Q_SIGNALS:
    void repoDirChanged();

private:
    class Private;
    Private *const d;
};

#endif // CALLIGRAGEMINI_GIT_GITLOGMODEL_H
