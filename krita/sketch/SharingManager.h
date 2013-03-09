/*
 * Copyright 2013  Dan Leinir Turthra Jensen <admin@leinir.dk>
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

#ifndef SHARINGMANAGER_H
#define SHARINGMANAGER_H

#include <QtCore/QObject>

class SharingManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject* view READ view WRITE setView NOTIFY viewChanged);

public:
    SharingManager(QObject* parent = 0);
    virtual ~SharingManager();

    Q_INVOKABLE QObject* sharingHandler(QString name);

    void setView(QObject* newView);
    QObject* view() const;

    Q_INVOKABLE void clearCookies(QObject* qmlEngine);
Q_SIGNALS:
    void viewChanged();
    void sharingSuccessful(const QString& message, const QString& url);

private:
    class Private;
    Private* d;
};

#endif // SHARINGMANAGER_H
