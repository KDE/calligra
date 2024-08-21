/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2010-2011 C. Boemann <cbo@boemann.dk>
 * SPDX-FileCopyrightText: 2005-2006 Boudewijn Rempt <boud@valdyas.org>
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "ToolDocker.h"
#include "ToolDocker_p.h"

#include <QAction>
#include <QGridLayout>
#include <QLabel>
#include <QPointer>
#include <QScrollArea>
#include <QScrollBar>
#include <QSet>
#include <QStyleOptionFrame>
#include <QTabWidget>
#include <QToolButton>

#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>

#include <KoDockWidgetTitleBar.h>
#include <KoIcon.h>

ToolDocker::ToolDocker(QWidget *parent)
    : QDockWidget(i18n("Tool Options"), parent)
    , d(new Private(this))
{
    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::TopDockWidgetArea);

    KConfigGroup cfg(KSharedConfig::openConfig(), "DockWidget sharedtooldocker");
    d->tabbed = cfg.readEntry("TabbedMode", false);
    d->hasTitle = cfg.readEntry("Locked", true);

    toggleViewAction()->setVisible(false); // should always be visible, so hide option in menu
    setFeatures(DockWidgetMovable | DockWidgetFloatable);
    if (d->hasTitle) {
        setTitleBarWidget(new KoDockWidgetTitleBar(this));
    } else {
        setTitleBarWidget(new QWidget());
    }
    connect(this, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)), this, SLOT(locationChanged(Qt::DockWidgetArea)));

    d->housekeeperWidget = new QWidget();
    d->housekeeperLayout = new QGridLayout();
    d->housekeeperWidget->setLayout(d->housekeeperLayout);
    d->housekeeperLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    d->hiderWidget = new QWidget(d->housekeeperWidget);
    d->hiderWidget->setVisible(false);

    d->scrollArea = new QScrollArea();
    d->scrollArea->setWidget(d->housekeeperWidget);
    d->scrollArea->setFrameShape(QFrame::NoFrame);
    d->scrollArea->setWidgetResizable(true);

    setWidget(d->scrollArea);

    d->lockButton = new QToolButton(this);
    if (d->hasTitle) {
        d->lockButton->setIcon(d->unlockIcon);
    } else {
        d->lockButton->setIcon(d->lockIcon);
    }
    d->lockButton->setToolTip(i18n("Toggles showing a title bar"));
    d->lockButton->setAutoRaise(true);
    connect(d->lockButton, SIGNAL(clicked()), SLOT(toggleLock()));
    d->lockButton->setVisible(true);
    d->lockButton->resize(d->lockButton->sizeHint());

    d->tabButton = new QToolButton(this); // parent hack in toggleLock to keep it clickable
    d->tabButton->setIcon(d->tabIcon);
    d->tabButton->setToolTip(i18n("Toggles organising the options in tabs or not"));
    d->tabButton->setAutoRaise(true);
    connect(d->tabButton, SIGNAL(clicked()), SLOT(toggleTab()));
    d->tabButton->resize(d->tabButton->sizeHint());
    d->tabButton->setVisible(d->hasTitle);
}

ToolDocker::~ToolDocker()
{
    KConfigGroup cfg(KSharedConfig::openConfig(), "DockWidget sharedtooldocker");
    cfg.writeEntry("TabbedMode", d->tabbed);
    cfg.writeEntry("Locked", d->hasTitle);
    cfg.sync();

    delete d;
}

bool ToolDocker::hasOptionWidget()
{
    return !d->currentWidgetList.isEmpty();
}

void ToolDocker::setOptionWidgets(const QList<QPointer<QWidget>> &optionWidgetList)
{
    d->recreateLayout(optionWidgetList);
}

void ToolDocker::resizeEvent(QResizeEvent *)
{
    int fw = isFloating() ? style()->pixelMetric(QStyle::PM_DockWidgetFrameWidth, nullptr, this) : 0;
    d->lockButton->move(width() - d->lockButton->width() - d->scrollArea->verticalScrollBar()->sizeHint().width(), fw);
    d->tabButton->move(d->lockButton->x() - d->tabButton->width() - 2, d->lockButton->y());
}

#include <moc_ToolDocker.cpp>
