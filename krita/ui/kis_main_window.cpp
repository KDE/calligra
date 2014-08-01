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

KisMainWindow::KisMainWindow(KoPart *part, const KComponentData &instance)
    : KoMainWindow(part, instance)
    , m_constructing(true)
    , m_mdiArea(new QMdiArea(this))
{
    // 25 px is a distance that works well for Tablet and Mouse events
    qApp->setStartDragDistance(25);

    m_mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

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

    KGlobal::setActiveComponent(part ? part->componentData() : KGlobal::mainComponent());

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
}

void KisMainWindow::showView(KoView *view)
{
    KisImageView *imageView = qobject_cast<KisImageView*>(view);
    if (imageView) {
        // XXX: find a better way to initialize this!
        imageView->canvasBase()->setFavoriteResourceManager(m_guiClient->paintOpBox()->favoriteResourcesManager());
        view->guiActivateEvent(true);

        QMdiSubWindow *subwin = m_mdiArea->addSubWindow(view);
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

KisImageView *KisMainWindow::activeKisView()
{
    if (!m_mdiArea) return 0;
    QMdiSubWindow *activeSubWindow = m_mdiArea->activeSubWindow();
    if (!activeSubWindow) return 0;
    return qobject_cast<KisImageView *>(activeSubWindow->widget());
}

bool KisMainWindow::event( QEvent* event )
{
    if (m_constructing) return false;

    if (!activeKisView()) {
        event->accept();
        return true;
    }

    switch(static_cast<int>(event->type()))
    {
        case ViewModeSwitchEvent::AboutToSwitchViewModeEvent: {
            ViewModeSynchronisationObject* syncObject = static_cast<ViewModeSwitchEvent*>(event)->synchronisationObject();

            KisCanvasResourceProvider* provider = m_guiClient->resourceProvider();
            syncObject->backgroundColor = provider->bgColor();
            syncObject->foregroundColor = provider->fgColor();
            syncObject->exposure = provider->HDRExposure();
            syncObject->gamma = provider->HDRGamma();
            syncObject->compositeOp = provider->currentCompositeOp();
            syncObject->pattern = provider->currentPattern();
            syncObject->gradient = provider->currentGradient();
            syncObject->node = provider->currentNode();
            syncObject->paintOp = provider->currentPreset();
            syncObject->opacity = provider->opacity();
            syncObject->globalAlphaLock = provider->globalAlphaLock();

            syncObject->documentOffset = m_guiClient->canvasControllerWidget()->scrollBarValue() - pos();
            syncObject->zoomLevel = activeKisView()->zoomController()->zoomAction()->effectiveZoom();
            syncObject->rotationAngle = m_guiClient->canvasBase()->rotationAngle();

            syncObject->activeToolId = KoToolManager::instance()->activeToolId();

            syncObject->initialized = true;

            QMainWindow* mainWindow = qobject_cast<QMainWindow*>(qApp->activeWindow());
            if(mainWindow) {
                QList<QDockWidget*> dockWidgets = mainWindow->findChildren<QDockWidget*>();
                foreach(QDockWidget* widget, dockWidgets) {
                    if (widget->isFloating()) {
                        widget->hide();
                    }
                }
            }

            return true;
        }
        case ViewModeSwitchEvent::SwitchedToDesktopModeEvent: {
            ViewModeSynchronisationObject* syncObject = static_cast<ViewModeSwitchEvent*>(event)->synchronisationObject();
            m_guiClient->canvasControllerWidget()->setFocus();
            qApp->processEvents();

            if(syncObject->initialized) {
                KisCanvasResourceProvider* provider = m_guiClient->resourceProvider();

                provider->setPaintOpPreset(syncObject->paintOp);
                qApp->processEvents();

                KoToolManager::instance()->switchToolRequested(syncObject->activeToolId);
                qApp->processEvents();

                KisPaintOpPresetSP preset = m_guiClient->resourceProvider()->resourceManager()->resource(KisCanvasResourceProvider::CurrentPaintOpPreset).value<KisPaintOpPresetSP>();
                preset->settings()->setProperty("CompositeOp", syncObject->compositeOp);
                if (preset->settings()->hasProperty("OpacityValue"))
                    preset->settings()->setProperty("OpacityValue", syncObject->opacity);
                provider->setPaintOpPreset(preset);

                provider->setBGColor(syncObject->backgroundColor);
                provider->setFGColor(syncObject->foregroundColor);
                provider->setHDRExposure(syncObject->exposure);
                provider->setHDRGamma(syncObject->gamma);
                provider->slotPatternActivated(syncObject->pattern);
                provider->slotGradientActivated(syncObject->gradient);
                provider->slotNodeActivated(syncObject->node);
                provider->setOpacity(syncObject->opacity);
                provider->setGlobalAlphaLock(syncObject->globalAlphaLock);
                provider->setCurrentCompositeOp(syncObject->compositeOp);

                actionCollection()->action("zoom_in")->trigger();
                qApp->processEvents();


                QList<QDockWidget*> dockWidgets = findChildren<QDockWidget*>();
                foreach(QDockWidget* widget, dockWidgets) {
                    if (widget->isFloating()) {
                        widget->show();
                    }
                }


                activeKisView()->zoomController()->setZoom(KoZoomMode::ZOOM_CONSTANT, syncObject->zoomLevel);
                m_guiClient->canvasControllerWidget()->rotateCanvas(syncObject->rotationAngle - activeKisView()->canvasBase()->rotationAngle());

                QPoint newOffset = syncObject->documentOffset + pos();
                qApp->processEvents();
                m_guiClient->canvasControllerWidget()->setScrollBarValue(newOffset);
            }

            return true;
        }
        default:
            break;
    }

    return QWidget::event( event );
}

