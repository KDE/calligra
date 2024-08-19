/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2008-2012 C. Boemann <cbo@boemann.dk>
 * SPDX-FileCopyrightText: 2009 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoDockerManager.h"
#include "KoDockFactoryBase.h"
#include "KoDockerManager_p.h"

#include <KConfig>
#include <KConfigGroup>
#include <KLocalizedString>
#include <MainDebug.h>

#include "KoDockRegistry.h"
#include "KoToolDocker.h"

#include "KoMainWindow.h"
#include "KoView.h"

#include <QFontDatabase>

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
        return new KoToolDocker();
    }

    DockPosition defaultDockPosition() const override
    {
        return DockRight;
    }
};

KoDockerManager::KoDockerManager(KoMainWindow *mainWindow)
    : QObject(mainWindow)
    , d(std::make_unique<Private>(mainWindow))
{
    ToolDockerFactory toolDockerFactory;
    d->toolOptionsDocker = qobject_cast<KoToolDocker *>(mainWindow->createDockWidget(&toolDockerFactory));
    Q_ASSERT(d->toolOptionsDocker);
    d->toolOptionsDocker->setVisible(false);

    connect(mainWindow, &KoMainWindow::restoringDone, this, [this]() {
        d->restoringDone();
    });
}

KoDockerManager::~KoDockerManager() = default;

void KoDockerManager::newOptionWidgets(const QList<QPointer<QWidget>> &optionWidgetList)
{
    d->toolOptionsDocker->setOptionWidgets(optionWidgetList);
    QFont dockWidgetFont = KoDockRegistry::dockFont();

    for (QWidget *w : std::as_const(optionWidgetList)) {
#ifdef Q_OS_MAC
        w->setAttribute(Qt::WA_MacSmallSize, true);
#endif
        w->setFont(dockWidgetFont);
    }
}

void KoDockerManager::removeToolOptionsDocker()
{
    d->toolOptionsDocker->setVisible(false);
    d->showOptionsDocker = false;
}

void KoDockerManager::resetToolDockerWidgets()
{
    d->toolOptionsDocker->resetWidgets();
}

// have to include this because of Q_PRIVATE_SLOT
#include <moc_KoDockerManager.cpp>
