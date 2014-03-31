/*
 * Copyright (c) 2014 Shivaraman Aiyer<sra392@gmail.com>
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
#include "tool_perspectivegridng.h"
#include "kis_tool_perspectivegridng.h"

#include <kpluginfactory.h>

#include <KoToolRegistry.h>
//#include "RulerAssistant.h"
//#include "EllipseAssistant.h"
//#include "SplineAssistant.h"
//#include "PerspectiveAssistant.h"
//#include "mesh_assistant.h"

K_PLUGIN_FACTORY(PerspectiveGridNgToolFactory, registerPlugin<KisPerspectiveGridNgToolPlugin>();)
K_EXPORT_PLUGIN(PerspectiveGridNgToolFactory("krita"))


KisPerspectiveGridNgToolPlugin::KisPerspectiveGridNgToolPlugin(QObject *parent, const QVariantList &)
        : QObject(parent)
{
    KoToolRegistry * r = KoToolRegistry::instance();
    r->add(new KisPerspectiveGridNgToolFactory());
//    qDebug()<<"Shiva: " << "KisPerspectiveGridNgToolPlugin";
//    KisPerspectiveGridNgFactoryRegistry::instance()->add(new RulerAssistantFactory);
//    KisPerspectiveGridNgFactoryRegistry::instance()->add(new EllipseAssistantFactory);
//    KisPerspectiveGridNgFactoryRegistry::instance()->add(new SplineAssistantFactory);
//    KisPerspectiveGridNgFactoryRegistry::instance()->add(new PerspectiveAssistantFactory);
//    KisPaintingAssistantFactoryRegistry::instance()->add(new MeshAssistantFactory);
}

KisPerspectiveGridNgToolPlugin::~KisPerspectiveGridNgToolPlugin()
{
}

#include "tool_perspectivegridng.moc"
