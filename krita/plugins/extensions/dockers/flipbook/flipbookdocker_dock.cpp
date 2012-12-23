/*
 *  Copyright (c) 2012 Boudewijn Rempt <boud@valdyas.org>
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

#include "flipbookdocker_dock.h"

#include <QListWidget>

#include <klocale.h>
#include <kactioncollection.h>

#include <KoIcon.h>
#include <KoCanvasBase.h>

#include <kis_view2.h>
#include <kis_canvas2.h>


FlipbookDockerDock::FlipbookDockerDock( )
    : QDockWidget(i18n("Flipbook"))
    , m_canvas(0)
{
    QWidget* widget = new QWidget(this);
    setupUi(widget);

    connect(this, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)), SLOT(updateLayout()));
}

FlipbookDockerDock::~FlipbookDockerDock()
{
}

void FlipbookDockerDock::setCanvas(KoCanvasBase * canvas)
{
    if (m_canvas && m_canvas->view()) {
         m_canvas->view()->actionCollection()->disconnect(this);
         foreach(KXMLGUIClient* client, m_canvas->view()->childClients()) {
            client->actionCollection()->disconnect(this);
        }
    }
    m_canvas = dynamic_cast<KisCanvas2*>(canvas);
}

void FlipbookDockerDock::unsetCanvas()
{
    m_canvas = 0;
}

void FlipbookDockerDock::updateLayout(Qt::DockWidgetArea area)
{
    if (area == Qt::LeftDockWidgetArea || area == Qt::RightDockWidgetArea) {
        listFlipbook->setFlow(QListView::TopToBottom);
    }
    else {
        listFlipbook->setFlow(QListView::LeftToRight);
    }
}

#include "flipbookdocker_dock.moc"
