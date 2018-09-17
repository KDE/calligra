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
#ifndef DockerManager_p_h
#define DockerManager_p_h

#include <QList>
#include <QGridLayout>

#include <ktoolbar.h>
#include <KLocalizedString>

#include "DockerManager.h"
#include "KoDockFactoryBase.h"
#include "ToolDocker.h"
#include "MainWindow.h"

class ToolDockerFactory : public KoDockFactoryBase
{
public:
    ToolDockerFactory() : KoDockFactoryBase() { }

    QString id() const {
        return "sharedtooldocker";
    }

    QDockWidget* createDockWidget() {
        ToolDocker * dockWidget = new ToolDocker();
        return dockWidget;
    }

    DockPosition defaultDockPosition() const {
        return DockRight;
    }
};

class ToolBarsDockerFactory : public KoDockFactoryBase
{
public:
    ToolBarsDockerFactory() : KoDockFactoryBase() { }

    QString id() const {
        return "ToolBarDocker";
    }

    QDockWidget* createDockWidget() {
        return new QDockWidget(i18n("Tool Bars"));
    }

    DockPosition defaultDockPosition() const {
        return DockTop;
    }
};

class DockerManager::Private
{
public:
    Private(MainWindow *mw) :
        dockedToolBarsLayout(0)
        , mainWindow(mw)
        , ignore(true) {
    }

    ToolDocker *toolOptionsDocker;
    QDockWidget *toolBarsDocker;
    QGridLayout *dockedToolBarsLayout;
    QList<KToolBar *> toolBarList;
    MainWindow *mainWindow;
    bool ignore;

    void moveToolBarsBack() {
        foreach(KToolBar * toolBar, toolBarList) {
            mainWindow->addToolBar(toolBar);
        }
        toolBarList.clear();
    }

    void moveToolBars() {
        if(ignore)
            return;

        // Move toolbars to docker or back depending on visibility of docker
        if(toolBarsDocker->isVisible()) {
            QList<KToolBar *> tmpList = mainWindow->toolBars();
            toolBarList.append(tmpList);
            foreach(KToolBar * toolBar, tmpList) {
                dockedToolBarsLayout->addWidget(toolBar);
            }
        } else {
            moveToolBarsBack();
        }
    }
};

#endif
