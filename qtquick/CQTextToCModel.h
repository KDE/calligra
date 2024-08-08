/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 */

#ifndef CQTEXTTOCMODEL_H
#define CQTEXTTOCMODEL_H

#include <QAbstractListModel>

class CQTextToCModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QObject *canvas READ canvas WRITE setCanvas NOTIFY canvasChanged);

public:
    enum TextToCModelRoleNames { Title = Qt::UserRole + 1, Level, PageNumber };
    explicit CQTextToCModel(QObject *parent = nullptr);
    virtual ~CQTextToCModel();
    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent) const override;
    QHash<int, QByteArray> roleNames() const;

    QObject *canvas() const;
    void setCanvas(QObject *newCanvas);
Q_SIGNALS:
    void canvasChanged();

private Q_SLOTS:
    void requestGeneration();
    void startDoneTimer();
    void timeout();
    void updateToC();

private:
    class Private;
    Private *const d;
};

#endif // CQTEXTTOCMODEL_H
