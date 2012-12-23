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
    setWidget(widget);

    connect(this, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)), SLOT(updateLayout(Qt::DockWidgetArea)));

    bnFirstItem->setIcon(koIcon("arrow-up-double"));
    bnFirstItem->setToolTip(i18n("Go to the first image in the current flipbook"));

    bnPreviousItem->setIcon(koIcon("arrow-up"));
    bnPreviousItem->setToolTip(i18n("Show previous image"));

    bnNextItem->setIcon(koIcon("arrow-down"));
    bnNextItem->setToolTip(i18n("Show next image"));

    bnLastItem->setIcon(koIcon("arrow-down-double"));
    bnLastItem->setToolTip(i18n("Go to the last image in the current flipblook"));

    bnAddItem->setIcon(koIcon("list-add"));
    bnAddItem->setToolTip(i18n("Add one or more images to the current flipbook"));

    bnDeleteItem->setIcon(koIcon("list-remove"));
    bnDeleteItem->setToolTip(i18n("Remove selected images from the current flipbook"));

    bnDeleteFlipbook->setIcon(koIcon("edit-delete"));
    bnDeleteFlipbook->setToolTip(i18n("Delete current flipbook from disk"));

    bnLoadFlipbook->setIcon(koIcon("document-open"));
    bnLoadFlipbook->setToolTip(i18n("Open a flipbook file"));

    bnNewFlipbook->setIcon(koIcon("document-new"));
    bnNewFlipbook->setToolTip(i18n("Create a new flipbook"));
    //cmbFlipbooks;
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
        bnFirstItem->setIcon(koIcon("arrow-up-double"));
        bnPreviousItem->setIcon(koIcon("arrow-up"));
        bnNextItem->setIcon(koIcon("arrow-down"));
        bnLastItem->setIcon(koIcon("arrow-down-double"));
    }
    else {
        listFlipbook->setFlow(QListView::LeftToRight);
        bnFirstItem->setIcon(koIcon("arrow-left-double"));
        bnPreviousItem->setIcon(koIcon("arrow-left"));
        bnNextItem->setIcon(koIcon("arrow-right"));
        bnLastItem->setIcon(koIcon("arrow-left-double"));
    }
}

#include "flipbookdocker_dock.moc"
