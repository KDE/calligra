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

#include "BraindumpQuickStatesPlugin.h"

#include <QPainter>
#include <QSvgRenderer>
#include <QStandardPaths>

#include <kactioncollection.h>
#include <kactionmenu.h>
#include <kgenericfactory.h>

#include "State.h"
#include "StatesRegistry.h"

#include "QuickStateHandler.h"

// QT5TODO: port to Json-based plugin
typedef KGenericFactory<BraindumpQuickStatesPlugin> BraindumpQuickStatesPluginFactory;
K_EXPORT_COMPONENT_FACTORY(braindumpquickstates, BraindumpQuickStatesPluginFactory("braindump"))

BraindumpQuickStatesPlugin::BraindumpQuickStatesPlugin(QObject *parent, const QStringList &)
    : QObject(parent)
{
    setXMLFile(QStandardPaths::locate(QStandardPaths::GenericDataLocation, "braindump/plugins/quickstates.rc"), true);

    // TODO try to use action list, or to get access to the state menu

    KActionMenu* actionMenu = new KActionMenu(i18n("States"), this);
    actionCollection()->addAction("States", actionMenu);

    foreach(const QString & catId, StatesRegistry::instance()->categorieIds()) {
        foreach(const QString & stateId, StatesRegistry::instance()->stateIds(catId)) {
            const State* state = StatesRegistry::instance()->state(catId, stateId);
            QAction * action = new QAction(state->name(), this);
            actionCollection()->addAction(QString("State_%1_%2").arg(catId).arg(stateId), action);
            actionMenu->addAction(action);
            QuickStateHandler* handler = new QuickStateHandler(catId, stateId, this);
            connect(action, SIGNAL(triggered()), handler, SLOT(activate()));
            QPixmap image(32, 32);
            QPainter p(&image);
            state->renderer()->render(&p, QRectF(0, 0, 32, 32));
            action->setIcon(image);
        }
        actionMenu->addSeparator();
    }
}

BraindumpQuickStatesPlugin::~BraindumpQuickStatesPlugin()
{

}

#include "BraindumpQuickStatesPlugin.moc"
