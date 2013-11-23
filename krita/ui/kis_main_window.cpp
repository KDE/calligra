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

#include <kxmlguiclient.h>
#include <kxmlguifactory.h>
#include <kactioncollection.h>

#include <KoView.h>
#include <KoPart.h>

#include "kis_view2.h"
#include "dialogs/kis_dlg_preferences.h"
#include "kis_config_notifier.h"
#include "kis_canvas_resource_provider.h"
#include "kis_node.h"
#include "kis_image.h"
#include "kis_group_layer.h"


class KisMainGui : public KXMLGUIClient {
public:
    KisMainGui(KisMainWindow *mw) {
        setXMLFile("kritashell.rc", true);
        actionCollection()->addAction(KStandardAction::Preferences, "preferences", mw, SLOT(slotPreferences()));
    }
};

KisMainWindow::KisMainWindow(KoPart *part, const KComponentData &instance)
    : KoMainWindow(part, instance)
    , m_mdiArea(new QMdiArea(this))
{
    setCentralWidget(m_mdiArea);
    m_mdiArea->show();
    guiFactory()->addClient(new KisMainGui(this));
}

void KisMainWindow::showView(KoView *view)
{
    setActiveView(view);
    m_mdiArea->addSubWindow(view);
    view->show();
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

