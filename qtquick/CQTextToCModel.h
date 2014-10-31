/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2013 Dan Leinir Turthra Jensen <admin@leinir.dk>
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
 *
 */

#ifndef CQTEXTTOCMODEL_H
#define CQTEXTTOCMODEL_H

#include <QAbstractListModel>

class CQTextToCModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QObject* canvas READ canvas WRITE setCanvas NOTIFY canvasChanged);
public:
    enum TextToCModelRoleNames {
        Title = Qt::UserRole + 1,
        Level,
        PageNumber
    };
    explicit CQTextToCModel(QObject* parent = 0);
    virtual ~CQTextToCModel();
    virtual QVariant data(const QModelIndex& index, int role) const;
    virtual int rowCount(const QModelIndex& parent) const;


    QObject* canvas() const;
    void setCanvas(QObject* newCanvas);
Q_SIGNALS:
    void canvasChanged();

private Q_SLOTS:
    void requestGeneration();
    void startDoneTimer();
    void timeout();
    void updateToC();

private:
    class Private;
    Private* const d;
};

#endif // CQTEXTTOCMODEL_H
