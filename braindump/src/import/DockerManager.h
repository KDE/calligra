/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2008 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef DOCKER_MANAGER_H
#define DOCKER_MANAGER_H

#include <QMap>
#include <QObject>
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
    explicit DockerManager(MainWindow *mainWindow);
    ~DockerManager();

public Q_SLOTS:
    // void removeUnusedOptionWidgets();
    /**
     * Update the option widgets to the argument ones, removing the currently set widgets.
     */
    void newOptionWidgets(const QList<QPointer<QWidget>> &optionWidgetMap);

private:
    Q_PRIVATE_SLOT(d, void moveToolBars())
    class Private;
    Private *const d;
};

#endif
