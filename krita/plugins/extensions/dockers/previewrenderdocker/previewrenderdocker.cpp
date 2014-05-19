/*
 *  Copyright (c) 2014 Spencer Brown <sbrown655@gmail.com>
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

#include <opengl/kis_opengl.h>

#ifdef HAVE_OPENGL

#include "previewrenderdocker.h"
#include <stdlib.h>

// TODO: organize includes and get rid of unnecessary ones from the copy
#include <QTimer>

#include <kactioncollection.h>
#include <kcomponentdata.h>
#include <kis_debug.h>
#include <kpluginfactory.h>
#include <klocale.h>
#include <kstandarddirs.h>

#include <KoDockFactoryBase.h>

#include "kis_config.h"
#include "kis_cursor.h"
#include "kis_global.h"
#include "kis_types.h"
#include "kis_view2.h"

#include "previewrenderdocker_dock.h"
#include <KoDockRegistry.h>

K_PLUGIN_FACTORY(PreviewRenderDockerPluginFactory, registerPlugin<PreviewRenderDockerPlugin>();)
K_EXPORT_PLUGIN(PreviewRenderDockerPluginFactory( "krita" ) )

class PreviewRenderDockerDockFactory : public KoDockFactoryBase {
public:
    PreviewRenderDockerDockFactory()
    {
    }

    virtual QString id() const
    {
        return QString( "PreviewRenderDocker" );
    }

    virtual Qt::DockWidgetArea defaultDockWidgetArea() const
    {
        return Qt::RightDockWidgetArea;
    }

    virtual QDockWidget* createDockWidget()
    {
        PreviewRenderDockerDock * dockWidget = new PreviewRenderDockerDock();
        dockWidget->setObjectName(id());

        return dockWidget;
    }

    DockPosition defaultDockPosition() const
    {
        return DockMinimized;
    }
private:


};


PreviewRenderDockerPlugin::PreviewRenderDockerPlugin(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    KoDockRegistry::instance()->add(new PreviewRenderDockerDockFactory());
}

PreviewRenderDockerPlugin::~PreviewRenderDockerPlugin()
{
    m_view = 0;
}

#include "previewrenderdocker.moc"
#endif // HAVE_OPENGL
