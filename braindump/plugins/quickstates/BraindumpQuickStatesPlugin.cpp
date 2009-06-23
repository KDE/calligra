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

#include <KActionCollection>
#include <KGenericFactory>
#include <KStandardDirs>

typedef KGenericFactory<BraindumpQuickStatesPlugin> BraindumpQuickStatesPluginFactory;
K_EXPORT_COMPONENT_FACTORY(braindumpquickstates, BraindumpQuickStatesPluginFactory("braindump"))

BraindumpQuickStatesPlugin::BraindumpQuickStatesPlugin(QObject *parent, const QStringList &)
        : KParts::Plugin(parent)
{
  setXMLFile(KStandardDirs::locate("data", "braindump/plugins/quickstates.rc"), true);

//                    KAction *action  = new KAction(i18n("&Uncheck"), this);
//   actionCollection()->addAction("QuickStates_Unchecked", action);
//   connect(action, SIGNAL(triggered()), this, SLOT(slotUnchecked()));
}

BraindumpQuickStatesPlugin::~BraindumpQuickStatesPlugin() {
  
}

#include "BraindumpQuickStatesPlugin.moc"
