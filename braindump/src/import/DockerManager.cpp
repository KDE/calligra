/* This file is part of the KDE project
 *
 * Copyright (c) 2008,2010 C. Boemann <cbo@boemann.dk>
 * Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
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
#include "DockerManager.h"
#include "DockerManager_p.h"

#include <QList>
#include <QGridLayout>

#include "KoDockFactoryBase.h"

#include "ToolDocker.h"
#include "MainWindow.h"


DockerManager::DockerManager(MainWindow *mainWindow)
    : QObject(mainWindow), d(new Private(mainWindow))
{
    ToolDockerFactory toolDockerFactory;
    ToolBarsDockerFactory toolBarsDockerFactory;
    d->toolOptionsDocker =
        qobject_cast<ToolDocker*>(mainWindow->createDockWidget(&toolDockerFactory));
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

void DockerManager::newOptionWidgets(const QList<QPointer<QWidget> > &optionWidgetMap)
{
    d->toolOptionsDocker->setOptionWidgets(optionWidgetMap);
}

#include <moc_DockerManager.cpp>
