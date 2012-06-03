/*
 * Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "rijn.h"

#include <stdlib.h>

#include <kactioncollection.h>
#include <kcomponentdata.h>
#include <kis_debug.h>
#include <kpluginfactory.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kactionmenu.h>

#include "kis_config.h"
#include "kis_global.h"
#include "kis_types.h"
#include "kis_view2.h"

#include <GTLCore/String.h>
#include <OpenRijn/SourcesCollection.h>

#include "kis_rijn_sketch_action.h"

K_PLUGIN_FACTORY(rijnPluginFactory, registerPlugin<rijnPlugin>();)
K_EXPORT_PLUGIN(rijnPluginFactory("krita"))

rijnPlugin::rijnPlugin(QObject *parent, const QVariantList &)
    : KParts::Plugin(parent)
{
    if ( parent->inherits("KisView2") )
    {
        m_view = (KisView2*) parent;

        setXMLFile(KStandardDirs::locate("data","kritaplugins/rijn.rc"), true);
        
        KActionMenu* menu_action = new KActionMenu("Sketches", this);
        
        actionCollection()->addAction("rijn_sketches", menu_action);
        
        m_sourcesCollection = new OpenRijn::SourcesCollection;
        
        std::list<OpenRijn::Source> sketches = m_sourcesCollection->sources();
        
        foreach(OpenRijn::Source sketch, sketches) {
            KAction *action  = new KAction(sketch.name().c_str(), this);
            actionCollection()->addAction("rijn", action );
            menu_action->addAction(action);
            
            KisRijnSketchAction* rs_action = new KisRijnSketchAction(this, m_view, sketch);
            connect(action, SIGNAL(triggered()), rs_action, SLOT(activated()));
        }
    }
}

rijnPlugin::~rijnPlugin()
{
    m_view = 0;
}

#include "rijn.moc"
