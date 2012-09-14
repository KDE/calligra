/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2012  Dan Leinir Turthra Jensen <admin@leinir.dk>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#ifndef TOOLMANAGER_H
#define TOOLMANAGER_H

#include <qdeclarativeitem.h>


class ToolManager : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(QObject* view READ view WRITE setView NOTIFY viewChanged)
public:
    ToolManager(QDeclarativeItem* parent = 0);
    virtual ~ToolManager();

    QObject* view() const;
    void setView(QObject* newView);

    Q_INVOKABLE void requestToolChange(QString toolID);

Q_SIGNALS:
    void viewChanged();

private:
    class Private;
    Private* d;
};

#endif // TOOLMANAGER_H
