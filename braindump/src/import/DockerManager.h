/* This file is part of the KDE project
 *
 * Copyright (c) 2008 C. Boemann <cbo@boemann.dk>
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
 */
#ifndef DOCKER_MANAGER_H
#define DOCKER_MANAGER_H

#include <QObject>
#include <QMap>
#include <QPointer>

#include "komain_export.h"

class MainWindow;

/**
   The docker manager makes sure that tool option widgets are shown at the right time.
 */
class DockerManager : public QObject
{
    Q_OBJECT
public:
    explicit DockerManager(MainWindow* mainWindow);
    ~DockerManager();

public Q_SLOTS:
    //void removeUnusedOptionWidgets();
    /**
     * Update the option widgets to the argument ones, removing the currently set widgets.
     */
    void newOptionWidgets(const QList<QPointer<QWidget> > & optionWidgetMap);


private:
    Q_PRIVATE_SLOT(d, void moveToolBars())
    class Private;
    Private * const d;
};

#endif
