/* This file is part of the KDE project
 *
 * Copyright (c) 2008 Casper Boemann <cbr@boemann.dk>
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

#include <kglobal.h>
#include <klocale.h>
#include <kconfig.h>
#include <kconfiggroup.h>
#include <kdebug.h>

#include "ToolDockerFactory.h"
#include "ToolDocker.h"
#include <MainWindow.h>

#include <QAction>

class DockerManager::Private {
public:
    Private() : view(0) {}
    MainWindow *view;
    QMap<QString, ToolDocker *> toolDockerMap;
    QMap<QString, bool> toolDockerVisibillityMap;
    QMap<QString, ToolDocker *> activeToolDockerMap;
};

DockerManager::DockerManager(MainWindow *view)
    : QObject(view), d( new Private() )
{
    d->view = view;

    KConfigGroup cfg = KGlobal::config()->group("KoDockerManager");

    QStringList strList = cfg.readEntry("StatusOfTheseToolDockers", QStringList());

    QStringListIterator j(strList);
    while (j.hasNext()) {
        QString name = j.next();
        ToolDockerFactory toolDockerFactory(name);
        ToolDocker *td = qobject_cast<ToolDocker *>(d->view->createDockWidget(&toolDockerFactory));
        d->toolDockerMap[name] = td;
        d->toolDockerVisibillityMap[name] = true;
    }
}

DockerManager::~DockerManager()
{
    KConfigGroup cfg = KGlobal::config()->group("KoDockerManager");
    QStringList strList;
    QMapIterator<QString, ToolDocker *> j(d->toolDockerMap);
    while (j.hasNext()) {
        j.next();
        if(d->toolDockerVisibillityMap[j.key()]) {
          strList += j.key();
        }
    }
    cfg.writeEntry("StatusOfTheseToolDockers", strList);
    cfg.sync();
    delete d;
}

void DockerManager::newOptionWidgets(const QMap<QString, QWidget *> & optionWidgetMap)
{
    //removeUnusedOptionWidgets(); // will probably be needed to fix multi view problems

    // First remove the previous active dockers from sight and docker menu
    QMapIterator<QString, ToolDocker *> j(d->activeToolDockerMap);
    while (j.hasNext()) {
        j.next();
        j.value()->toggleViewAction()->setVisible(false);
        d->toolDockerVisibillityMap[j.key()] = j.value()->isVisible();
        j.value()->setVisible(false);
        j.value()->setEnabled(false);
    }
    d->activeToolDockerMap.clear();

    // Now show new active dockers (maybe even create) and show in docker menu
    QMapIterator<QString, QWidget *> i(optionWidgetMap);
    while (i.hasNext()) {
        i.next();
        if (i.value()->objectName().isEmpty()) {
            kDebug(30004) << "tooldocker widget have no name " << i.key() << " " << i.value()->objectName();
            Q_ASSERT(!(i.value()->objectName().isEmpty()));
            continue; // skip this docker in release build when assert don't crash
        }

        ToolDocker *td = d->toolDockerMap[i.value()->objectName()];

        if(!td) {
            ToolDockerFactory toolDockerFactory(i.value()->objectName());
            td = qobject_cast<ToolDocker*>(d->view->createDockWidget(&toolDockerFactory));
            if (!td)
                return;
            d->toolDockerMap[i.value()->objectName()] = td;
            d->toolDockerVisibillityMap[i.value()->objectName()] =  true;
        }
        td->setEnabled(true);
        td->setWindowTitle(i.key());
        td->newOptionWidget(i.value());
        d->view->restoreDockWidget(td);
        td->setVisible(d->toolDockerVisibillityMap[i.value()->objectName()]);
        td->toggleViewAction()->setVisible(true);
        d->activeToolDockerMap[i.value()->objectName()] = td;
        td->raise();
    }
}

#include "DockerManager.moc"
