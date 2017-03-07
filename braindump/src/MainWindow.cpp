/*
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "MainWindow.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QDockWidget>
#include <QLayout>
#include <QTabBar>
#include <QDebug>
#include <QFontDatabase>

#include <kactioncollection.h>
#include <kactionmenu.h>
#include <kconfiggroup.h>
#include <kstandardaction.h>
#include <ksharedconfig.h>
#include <klocalizedstring.h>

#include <KoDockRegistry.h>
#include <KoDockFactoryBase.h>
#include <KoCanvasObserverBase.h>

#include "RootSection.h"
#include "View.h"
#include "Canvas.h"
#include "import/DockerManager.h"
#include <kxmlguifactory.h>

#include "StatusBarItem.h"

MainWindow::MainWindow(RootSection* document) : m_doc(document), m_activeView(0), m_dockerManager(0)
{
    // then, setup our actions
    setupActions();

    // Create the docker manager after setting up the action
    m_dockerManager = new DockerManager(this);

    // Setup the view
    view = new View(m_doc, this);
    setCentralWidget(view);

    // a call to KXmlGuiWindow::setupGUI() populates the GUI
    // with actions, using KXMLGUI.
    // It also applies the saved mainwindow settings, if any, and ask the
    // mainwindow to automatically save settings if changed: window size,
    // toolbar position, icon size, etc.
    setupGUI();

    activateView(view);

    setAutoSaveSettings(qApp->applicationName());

    const int scnum = QApplication::desktop()->screenNumber(parentWidget());
    QRect desk = QApplication::desktop()->screenGeometry(scnum);

    // if the desktop is virtual then use virtual screen size
    if(QApplication::desktop()->isVirtualDesktop())
        desk = QApplication::desktop()->screenGeometry(QApplication::desktop()->screen());

    KConfigGroup config(KSharedConfig::openConfig(), qApp->applicationName());
    const QSize size(config.readEntry(QString::fromLatin1("Width %1").arg(desk.width()), 0),
                         config.readEntry(QString::fromLatin1("Height %1").arg(desk.height()), 0));
    resize(size);

    foreach(QDockWidget * wdg, m_dockWidgets) {
        if((wdg->features() & QDockWidget::DockWidgetClosable) == 0) {
            wdg->setVisible(true);
        }
    }

    forceDockTabFonts();
}

MainWindow::~MainWindow()
{
    // The view need to be deleted before the dockermanager
    delete view;
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    Q_UNUSED(e);
    const int scnum = QApplication::desktop()->screenNumber(parentWidget());
    QRect desk = QApplication::desktop()->screenGeometry(scnum);

    if(QApplication::desktop()->isVirtualDesktop()) {
        desk = QApplication::desktop()->screenGeometry(QApplication::desktop()->screen());
    }

    KConfigGroup config(KSharedConfig::openConfig(), qApp->applicationName());
    config.writeEntry(QString::fromLatin1("Width %1").arg(desk.width()), width());
    config.writeEntry(QString::fromLatin1("Height %1").arg(desk.height()), height());
}

void MainWindow::setupActions()
{
    KStandardAction::quit(qApp, SLOT(closeAllWindows()), actionCollection());
    m_doc->createActions(actionCollection());
    m_dockWidgetMenu  = new KActionMenu(i18n("Dockers"), this);
    actionCollection()->addAction("settings_dockers_menu", m_dockWidgetMenu);
    m_dockWidgetMenu->setVisible(false);
}

QDockWidget* MainWindow::createDockWidget(KoDockFactoryBase* factory)
{
    QDockWidget* dockWidget = 0;

    if(!m_dockWidgetMap.contains(factory->id())) {
        dockWidget = factory->createDockWidget();

        // It is quite possible that a dock factory cannot create the dock; don't
        // do anything in that case.
        if(!dockWidget) return 0;
        m_dockWidgets.push_back(dockWidget);

        dockWidget->setObjectName(factory->id());
        dockWidget->setParent(this);

        if(dockWidget->widget() && dockWidget->widget()->layout())
            dockWidget->widget()->layout()->setContentsMargins(1, 1, 1, 1);

        Qt::DockWidgetArea side = Qt::RightDockWidgetArea;
        bool visible = true;

        switch(factory->defaultDockPosition()) {
        case KoDockFactoryBase::DockTornOff:
            dockWidget->setFloating(true); // position nicely?
            break;
        case KoDockFactoryBase::DockTop:
            side = Qt::TopDockWidgetArea; break;
        case KoDockFactoryBase::DockLeft:
            side = Qt::LeftDockWidgetArea; break;
        case KoDockFactoryBase::DockBottom:
            side = Qt::BottomDockWidgetArea; break;
        case KoDockFactoryBase::DockRight:
            side = Qt::RightDockWidgetArea; break;
        case KoDockFactoryBase::DockMinimized:
            visible = false; break;
        default:;
        }

        addDockWidget(side, dockWidget);
        if(dockWidget->features() & QDockWidget::DockWidgetClosable) {
            m_dockWidgetMenu->addAction(dockWidget->toggleViewAction());
            if(!visible)
                dockWidget->hide();
        }

        m_dockWidgetMap.insert(factory->id(), dockWidget);
    } else {
        dockWidget = m_dockWidgetMap[ factory->id()];
    }

    connect(dockWidget, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)), this, SLOT(forceDockTabFonts()));

    return dockWidget;
}

void MainWindow::forceDockTabFonts()
{
    QObjectList chis = children();
    for(int i = 0; i < chis.size(); ++i) {
        if(chis.at(i)->inherits("QTabBar")) {
            ((QTabBar *)chis.at(i))->setFont(KoDockRegistry::dockFont());
        }
    }
}

DockerManager* MainWindow::dockerManager()
{
    return m_dockerManager;
}

void MainWindow::activateView(View* view)
{
    Q_ASSERT(factory());
    // Desactivate previous view
    if(m_activeView) {
        factory()->removeClient(m_activeView);
        foreach(StatusBarItem * item, m_statusBarItems[m_activeView]) {
            item->ensureItemHidden(statusBar());
        }
    }

    // Set the new view
    m_activeView = view;
    if(m_activeView) {
        factory()->addClient(view);
        // Show the status widget for the current view
        foreach(StatusBarItem * item, m_statusBarItems[m_activeView]) {
            item->ensureItemShown(statusBar());
        }
    }
}

void MainWindow::addStatusBarItem(QWidget* _widget, int _stretch, View* _view)
{
    Q_ASSERT(_widget);
    Q_ASSERT(_view);
    QList<StatusBarItem*>& list = m_statusBarItems[_view];
    StatusBarItem* item = new StatusBarItem(_widget, _stretch, _view);
    if(_view == m_activeView) {
        item->ensureItemShown(statusBar());
    }
    list.append(item);
}

void MainWindow::removeStatusBarItem(QWidget* _widget)
{
    foreach(View * key, m_statusBarItems.keys()) {
        QList<StatusBarItem*>& list = m_statusBarItems[key];
        foreach(StatusBarItem * item, list) {
            if(item->m_widget == _widget) {
                list.removeAll(item);
                item->ensureItemHidden(statusBar());
                delete item;
                return;
            }
        }
    }
    qWarning() << "Widget " << _widget << " not found in the status bar";
}

QList<KoCanvasObserverBase*> MainWindow::canvasObservers() const
{
    QList<KoCanvasObserverBase*> observers;

    foreach(QDockWidget * docker, m_dockWidgets) {
        KoCanvasObserverBase *observer = dynamic_cast<KoCanvasObserverBase*>(docker);
        if(observer) {
            observers << observer;
        }
    }
    return observers;
}
