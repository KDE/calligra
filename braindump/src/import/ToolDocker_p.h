/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2010-2011 C. Boemann <cbo@boemann.dk>
 * SPDX-FileCopyrightText: 2005-2006 Boudewijn Rempt <boud@valdyas.org>
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef ToolDocker_p_h
#define ToolDocker_p_h

#include "ToolDocker.h"

#include <KoDockWidgetTitleBar.h>
#include <KoIcon.h>

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

class ToolDocker::Private
{
public:
    Private(ToolDocker *dock)
        : q(dock)
        , tabbed(false)
        , hasTitle(false)
        , lockIcon(koIcon("object-locked"))
        , unlockIcon(koIcon("object-unlocked"))
        , tabIcon(koIcon("tab-new"))
        , unTabIcon(koIcon("tab-close"))
    {
    }

    QList<QPointer<QWidget>> currentWidgetList;
    QSet<QWidget *> currentAuxWidgets;
    QScrollArea *scrollArea;
    QWidget *hiderWidget; // non current widgets are hidden by being children of this
    QWidget *housekeeperWidget;
    QGridLayout *housekeeperLayout;
    ToolDocker *q;
    Qt::DockWidgetArea dockingArea;
    bool tabbed : 1;
    bool hasTitle : 1;
    QIcon lockIcon;
    QIcon unlockIcon;
    QIcon tabIcon;
    QIcon unTabIcon;
    QToolButton *lockButton;
    QToolButton *tabButton;

    void recreateLayout(const QList<QPointer<QWidget>> &optionWidgetList)
    {
        foreach (QPointer<QWidget> widget, currentWidgetList) {
            widget->setParent(hiderWidget);
        }
        qDeleteAll(currentAuxWidgets);
        currentAuxWidgets.clear();

        currentWidgetList = optionWidgetList;

        if (tabbed && currentWidgetList.size() > 1) {
            QTabWidget *t;
            housekeeperLayout->addWidget(t = new QTabWidget(), 0, 0);
            currentAuxWidgets.insert(t);
            foreach (QWidget *widget, currentWidgetList) {
                if (widget->objectName().isEmpty()) {
                    Q_ASSERT(!(widget->objectName().isEmpty()));
                    continue; // skip this docker in release build when assert don't crash
                }
                t->addTab(widget, widget->windowTitle());
            }
        } else {
            int cnt = 0;
            switch (dockingArea) {
            case Qt::TopDockWidgetArea:
            case Qt::BottomDockWidgetArea:
                housekeeperLayout->setHorizontalSpacing(2);
                housekeeperLayout->setVerticalSpacing(0);
                foreach (QWidget *widget, currentWidgetList) {
                    QFrame *s;
                    QLabel *l;
                    if (widget->objectName().isEmpty()) {
                        continue; // skip this docker in release build when assert don't crash
                    }
                    housekeeperLayout->addWidget(l = new QLabel(widget->windowTitle()), 0, 2 * cnt);
                    currentAuxWidgets.insert(l);
                    housekeeperLayout->addWidget(widget, 1, 2 * cnt);
                    widget->show();
                    if (widget != currentWidgetList.last()) {
                        housekeeperLayout->addWidget(s = new QFrame(), 0, 2 * cnt + 1, 2, 1);
                        s->setFrameShape(QFrame::VLine);
                        currentAuxWidgets.insert(s);
                    }
                    cnt++;
                }
                break;
            case Qt::LeftDockWidgetArea:
            case Qt::RightDockWidgetArea:
                housekeeperLayout->setHorizontalSpacing(0);
                housekeeperLayout->setVerticalSpacing(2);
                cnt = 0;
                foreach (QWidget *widget, currentWidgetList) {
                    QFrame *s;
                    QLabel *l;
                    if (widget->objectName().isEmpty()) {
                        Q_ASSERT(!(widget->objectName().isEmpty()));
                        continue; // skip this docker in release build when assert don't crash
                    }
                    housekeeperLayout->addWidget(l = new QLabel(widget->windowTitle()), 3 * cnt, 0);
                    currentAuxWidgets.insert(l);
                    housekeeperLayout->addWidget(widget, 3 * cnt + 1, 0);
                    widget->show();
                    if (widget != currentWidgetList.last()) {
                        housekeeperLayout->addWidget(s = new QFrame(), 3 * cnt + 2, 0);
                        s->setFrameShape(QFrame::HLine);
                        currentAuxWidgets.insert(s);
                    }
                    cnt++;
                }
                break;
            default:
                break;
            }
        }
        housekeeperLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
        housekeeperLayout->invalidate();
    }

    void locationChanged(Qt::DockWidgetArea area)
    {
        dockingArea = area;
        recreateLayout(currentWidgetList);
    }

    void toggleLock()
    {
        if (!hasTitle) {
            q->setTitleBarWidget(new KoDockWidgetTitleBar(q));
            hasTitle = true;
            lockButton->setIcon(unlockIcon);
            tabButton->setVisible(true);
            // parent hack to keep it clickable
            tabButton->setParent(q);
            tabButton->show();
            lockButton->setParent(nullptr);
            lockButton->setParent(q);
            lockButton->show();
        } else {
            q->setTitleBarWidget(new QWidget());
            hasTitle = false;
            lockButton->setIcon(lockIcon);
            tabButton->setVisible(false);
            // parent hack to keep it clickable
            tabButton->setParent(nullptr);
            lockButton->setParent(nullptr);
            lockButton->setParent(q);
            lockButton->show();
        }
        q->resizeEvent(nullptr);
    }
    void toggleTab()
    {
        if (!tabbed) {
            tabbed = true;
            tabButton->setIcon(unTabIcon);
        } else {
            tabbed = false;
            tabButton->setIcon(tabIcon);
        }
        recreateLayout(currentWidgetList);
    }
};
#endif
