/*
 *  Copyright (c) 2013 Boudewijn Rempt <boud@valdyas.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "kis_main_window.h"

#include <QDesktopWidget>
#include <QMdiSubWindow>
#include <QMdiArea>
#include <QSignalMapper>
#include <QCloseEvent>
#include <QIcon>

#include <kactioncollection.h>
#include <kaction.h>
#include <ktoggleaction.h>
#include <ktoolbar.h>
#include <kxmlguiclient.h>
#include <kxmlguifactory.h>

#include <KoZoomController.h>
#include <KoView.h>
#include <KoPart.h>

#include "kis_canvas_controller.h"
#include "kis_canvas2.h"
#include "kis_view2.h"
#include "kis_doc2.h"
#include "kis_image_view.h"
#include "dialogs/kis_dlg_preferences.h"
#include "kis_config_notifier.h"
#include "kis_canvas_resource_provider.h"
#include "kis_node.h"
#include "kis_image.h"
#include "kis_group_layer.h"
#include "kis_paintop_settings.h"
#include "kis_paintop_box.h"
#include "kis_config.h"
#include "kis_config_notifier.h"

KisMainWindow::KisMainWindow(KoPart *part, const KComponentData &instance)
    : KoMainWindow(part, instance)
    , m_constructing(true)
    , m_mdiArea(new QMdiArea(this))
{
    KGlobal::setActiveComponent(part ? part->componentData() : KGlobal::mainComponent());

    // 25 px is a distance that works well for Tablet and Mouse events
    qApp->setStartDragDistance(25);

    KisConfig cfg;
    QMdiArea::ViewMode viewMode = (QMdiArea::ViewMode)cfg.readEntry<int>("mdi_viewmode", (int)QMdiArea::TabbedView);

    m_mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_mdiArea->setViewMode(viewMode);
    m_mdiArea->setTabPosition(QTabWidget::North);

    setCentralWidget(m_mdiArea);
    m_mdiArea->show();

    connect(m_mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(updateMenus()));
    m_windowMapper = new QSignalMapper(this);
    connect(m_windowMapper, SIGNAL(mapped(QWidget*)), this, SLOT(setActiveSubWindow(QWidget*)));

    m_guiClient = new KisView2(this);

    m_guiClient->actionCollection()->addAction(KStandardAction::Preferences, "preferences", this, SLOT(slotPreferences()));

    m_mdiCascade = new KAction(i18n("Cascade"), this);
    m_guiClient->actionCollection()->addAction("windows_cascade", m_mdiCascade);
    connect(m_mdiCascade, SIGNAL(triggered()), m_mdiArea, SLOT(cascadeSubWindows()));

    m_mdiTile = new KAction(i18n("Tile"), this);
    m_guiClient->actionCollection()->addAction("windows_tile", m_mdiTile);
    connect(m_mdiTile, SIGNAL(triggered()), m_mdiArea, SLOT(tileSubWindows()));

    m_mdiNextWindow = new KAction(i18n("Next"), this);
    m_guiClient->actionCollection()->addAction("windows_next", m_mdiNextWindow);
    connect(m_mdiNextWindow, SIGNAL(triggered()), m_mdiArea, SLOT(activateNextSubWindow()));

    m_mdiPreviousWindow = new KAction(i18n("Previous"), this);
    m_guiClient->actionCollection()->addAction("windows_previous", m_mdiPreviousWindow);
    connect(m_mdiPreviousWindow, SIGNAL(triggered()), m_mdiArea, SLOT(activatePreviousSubWindow()));

    guiFactory()->addClient(m_guiClient);

    createMainwindowGUI();

    setAutoSaveSettings(part->componentData().componentName(), false);

    foreach (QDockWidget *wdg, dockWidgets()) {
        if ((wdg->features() & QDockWidget::DockWidgetClosable) == 0) {
            wdg->setVisible(true);
        }
    }

    // Create and plug toolbar list for Settings menu
    QList<QAction *> toolbarList;
    foreach(QWidget* it, guiFactory()->containers("ToolBar")) {
        KToolBar * toolBar = ::qobject_cast<KToolBar *>(it);
        if (toolBar) {
            KToggleAction * act = new KToggleAction(i18n("Show %1 Toolbar", toolBar->windowTitle()), this);
            actionCollection()->addAction(toolBar->objectName().toUtf8(), act);
            act->setCheckedState(KGuiItem(i18n("Hide %1 Toolbar", toolBar->windowTitle())));
            connect(act, SIGNAL(toggled(bool)), this, SLOT(slotToolbarToggled(bool)));
            act->setChecked(!toolBar->isHidden());
            toolbarList.append(act);
        } else
            kWarning(30003) << "Toolbar list contains a " << it->metaObject()->className() << " which is not a toolbar!";
    }
    plugActionList("toolbarlist", toolbarList);
    setToolbarList(toolbarList);

    updateMenus();

    m_constructing = false;

    connect(KisConfigNotifier::instance(), SIGNAL(configChanged()), this, SLOT(configChanged()));
}

void KisMainWindow::showView(KoView *view)
{
    KisImageView *imageView = qobject_cast<KisImageView*>(view);
    if (imageView) {
        // XXX: find a better way to initialize this!
        imageView->canvasBase()->setFavoriteResourceManager(m_guiClient->paintOpBox()->favoriteResourcesManager());
        view->guiActivateEvent(true);

        QMdiSubWindow *subwin = m_mdiArea->addSubWindow(view);
        subwin->setWindowIcon(QIcon(imageView->document()->generatePreview(QSize(64,64))));
        subwin->setWindowTitle(view->document()->url().fileName());
        if (m_mdiArea->subWindowList().size() == 1) {
            view->showMaximized();
        }
        else {
            view->show();
        }
        view->setFocus();


        m_guiClient->setCurrentView(view);
    }
}

void KisMainWindow::slotPreferences()
{
    if (KisDlgPreferences::editPreferences()) {
        KisConfigNotifier::instance()->notifyConfigChanged();

        // XXX: should this be changed for the views in other windows as well?
        foreach(QPointer<KoPart> part, KoPart::partList()) {
            foreach(QPointer<KoView> koview, part->views()) {
                KisView2 *view = qobject_cast<KisView2*>(koview);
                if (view) {
                    view->resourceProvider()->resetDisplayProfile(QApplication::desktop()->screenNumber(this));

                    // Update the settings for all nodes -- they don't query
                    // KisConfig directly because they need the settings during
                    // compositing, and they don't connect to the config notifier
                    // because nodes are not QObjects (because only one base class
                    // can be a QObject).
                    KisNode* node = dynamic_cast<KisNode*>(view->image()->rootLayer().data());
                    node->updateSettings();
                }

            }
        }
        m_guiClient->showHideScrollbars();
    }

}


void KisMainWindow::closeEvent(QCloseEvent *e)
{
    m_mdiArea->closeAllSubWindows();
    guiFactory()->removeClient(m_guiClient);
    KoMainWindow::closeEvent(e);
}

void KisMainWindow::updateMenus()
{
    bool enabled = (activeKisView() != 0);
    m_mdiCascade->setEnabled(enabled);
    m_mdiNextWindow->setEnabled(enabled);
    m_mdiPreviousWindow->setEnabled(enabled);
    m_mdiTile->setEnabled(enabled);

    setActiveSubWindow(m_mdiArea->activeSubWindow());
}

void KisMainWindow::updateWindowMenu()
{
}

void KisMainWindow::setActiveSubWindow(QWidget *window)
{
    if (!window) return;
    QMdiSubWindow *subwin = qobject_cast<QMdiSubWindow *>(window);
    if (subwin) {
        m_mdiArea->setActiveSubWindow(subwin);
        KoView *view = qobject_cast<KoView *>(subwin->widget());
        if (view) {
            view->guiActivateEvent(true);
            m_guiClient->setCurrentView(view);
        }
    }
}

void KisMainWindow::configChanged()
{
    KisConfig cfg;
    QMdiArea::ViewMode viewMode = (QMdiArea::ViewMode)cfg.readEntry<int>("mdi_viewmode", (int)QMdiArea::TabbedView);
    m_mdiArea->setViewMode(viewMode);
}

KisImageView *KisMainWindow::activeKisView()
{
    if (!m_mdiArea) return 0;
    QMdiSubWindow *activeSubWindow = m_mdiArea->activeSubWindow();
    if (!activeSubWindow) return 0;
    return qobject_cast<KisImageView *>(activeSubWindow->widget());
}
