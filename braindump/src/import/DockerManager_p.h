/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2008, 2010 C. Boemann <cbo@boemann.dk>
 * SPDX-FileCopyrightText: 2009 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef DockerManager_p_h
#define DockerManager_p_h

#include <QGridLayout>
#include <QList>

#include <KLocalizedString>
#include <KToolBar>

#include "DockerManager.h"
#include "KoDockFactoryBase.h"
#include "MainWindow.h"
#include "ToolDocker.h"

class ToolDockerFactory : public KoDockFactoryBase
{
public:
    ToolDockerFactory()
        : KoDockFactoryBase()
    {
    }

    QString id() const override
    {
        return "sharedtooldocker";
    }

    QDockWidget *createDockWidget() override
    {
        ToolDocker *dockWidget = new ToolDocker();
        return dockWidget;
    }

    DockPosition defaultDockPosition() const override
    {
        return DockRight;
    }
};

class ToolBarsDockerFactory : public KoDockFactoryBase
{
public:
    ToolBarsDockerFactory()
        : KoDockFactoryBase()
    {
    }

    QString id() const override
    {
        return "ToolBarDocker";
    }

    QDockWidget *createDockWidget() override
    {
        return new QDockWidget(i18n("Tool Bars"));
    }

    DockPosition defaultDockPosition() const override
    {
        return DockTop;
    }
};

class DockerManager::Private
{
public:
    Private(MainWindow *mw)
        : dockedToolBarsLayout(nullptr)
        , mainWindow(mw)
        , ignore(true)
    {
    }

    ToolDocker *toolOptionsDocker;
    QDockWidget *toolBarsDocker;
    QGridLayout *dockedToolBarsLayout;
    QList<KToolBar *> toolBarList;
    MainWindow *mainWindow;
    bool ignore;

    void moveToolBarsBack()
    {
        foreach (KToolBar *toolBar, toolBarList) {
            mainWindow->addToolBar(toolBar);
        }
        toolBarList.clear();
    }

    void moveToolBars()
    {
        if (ignore)
            return;

        // Move toolbars to docker or back depending on visibility of docker
        if (toolBarsDocker->isVisible()) {
            QList<KToolBar *> tmpList = mainWindow->toolBars();
            toolBarList.append(tmpList);
            foreach (KToolBar *toolBar, tmpList) {
                dockedToolBarsLayout->addWidget(toolBar);
            }
        } else {
            moveToolBarsBack();
        }
    }
};

#endif
