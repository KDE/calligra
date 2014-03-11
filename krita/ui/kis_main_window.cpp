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

#include <KoXMLGUIClient.h>
#include <KoXMLGUIFactory.h>
#include <kactioncollection.h>
#include <kaction.h>

#include <KoView.h>
#include <KoPart.h>

#include "kis_view2.h"
#include "dialogs/kis_dlg_preferences.h"
#include "kis_config_notifier.h"
#include "kis_canvas_resource_provider.h"
#include "kis_node.h"
#include "kis_image.h"
#include "kis_group_layer.h"


class KisMainGui : public KoXMLGUIClient {
public:
    KisMainGui(KisMainWindow *mw)
        : KoXMLGUIClient(mw)
    {
        setXMLFile("kritashell.rc", true);
    }

};

KisMainWindow::KisMainWindow(KoPart *part, const KComponentData &instance)
    : KoMainWindow(part, instance)
    , m_mdiArea(new QMdiArea(this))
{
    m_mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    setCentralWidget(m_mdiArea);
    m_mdiArea->show();

    connect(m_mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(updateMenus()));
    m_windowMapper = new QSignalMapper(this);
    connect(m_windowMapper, SIGNAL(mapped(QWidget*)), this, SLOT(setActiveSubWindow(QWidget*)));

    m_guiClient = new KisMainGui(this);

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

    updateMenus();
}

void KisMainWindow::showView(KoView *view)
{
    setActiveView(view);
    m_mdiArea->addSubWindow(view);
    if (m_mdiArea->subWindowList().size() == 1) {
        view->showMaximized();
    }
    else {
        view->show();
    }
    view->setFocus();
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
    }
}


void KisMainWindow::closeEvent(QCloseEvent *e)
{
    m_mdiArea->closeAllSubWindows();
    KoMainWindow::closeEvent(e);
}

void KisMainWindow::updateMenus()
{
    bool enabled = (activeKisView() != 0);
    m_mdiCascade->setEnabled(enabled);
    m_mdiNextWindow->setEnabled(enabled);
    m_mdiPreviousWindow->setEnabled(enabled);
    m_mdiTile->setEnabled(enabled);
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
        setActiveView(subwin->widget());
    }
}

KisView2 *KisMainWindow::activeKisView()
{
    if (QMdiSubWindow *activeSubWindow = m_mdiArea->activeSubWindow())
        return qobject_cast<KisView2 *>(activeSubWindow->widget());
    return 0;
}

