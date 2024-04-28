/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2008, 2010 C. Boemann <cbo@boemann.dk>
 * SPDX-FileCopyrightText: 2009 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "DockerManager.h"
#include "DockerManager_p.h"

#include <QGridLayout>
#include <QList>

#include "KoDockFactoryBase.h"

#include "MainWindow.h"
#include "ToolDocker.h"

DockerManager::DockerManager(MainWindow *mainWindow)
    : QObject(mainWindow)
    , d(new Private(mainWindow))
{
    ToolDockerFactory toolDockerFactory;
    ToolBarsDockerFactory toolBarsDockerFactory;
    d->toolOptionsDocker = qobject_cast<ToolDocker *>(mainWindow->createDockWidget(&toolDockerFactory));
    Q_ASSERT(d->toolOptionsDocker);
    d->toolOptionsDocker->setVisible(false);

    d->toolBarsDocker = mainWindow->createDockWidget(&toolBarsDockerFactory);
    Q_ASSERT(d->toolBarsDocker);

    QWidget *dockedToolBarsWidget = new QWidget();
    d->dockedToolBarsLayout = new QGridLayout();
    d->dockedToolBarsLayout->setHorizontalSpacing(2);
    d->dockedToolBarsLayout->setVerticalSpacing(0);
    dockedToolBarsWidget->setLayout(d->dockedToolBarsLayout);
    d->toolBarsDocker->setAllowedAreas(Qt::TopDockWidgetArea);
    d->toolBarsDocker->setFeatures(QDockWidget::DockWidgetClosable);
    d->toolBarsDocker->setWidget(dockedToolBarsWidget);
    d->toolBarsDocker->setTitleBarWidget(new QWidget());
    d->toolBarsDocker->setVisible(false);

    connect(d->toolBarsDocker, SIGNAL(visibilityChanged(bool)), this, SLOT(moveToolBars()));
}

DockerManager::~DockerManager()
{
    delete d;
}

void DockerManager::newOptionWidgets(const QList<QPointer<QWidget>> &optionWidgetMap)
{
    d->toolOptionsDocker->setOptionWidgets(optionWidgetMap);
}

#include <moc_DockerManager.cpp>
