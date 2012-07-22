/*
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
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

#include "overviewdocker_dock.h"

#include <QGridLayout>

#include <klocale.h>

#include <KoCanvasResourceManager.h>
#include <KoCanvasBase.h>
#include <KoZoomController.h>
#include "KoCanvasControllerWidget.h"

#include "kis_coordinates_converter.h"
#include "kis_zoom_manager.h"
#include "kis_canvas2.h"
#include "kis_view2.h"
#include "kis_doc2.h"
#include "kis_config.h"
#include "kis_canvas_controller.h"

OverviewDockerDock::OverviewDockerDock( )
    : QDockWidget(i18n("Image Overview"))
    , m_localCanvas(0)
    , m_canvas(0)
    , m_viewConverter(0)
    , m_canvasController(0)
    , m_resourceProvider(0)
{
}

void OverviewDockerDock::setCanvas(KoCanvasBase * canvas)
{
    // "Every connection you make emits a signal, so duplicate connections emit two signals"
    if (m_canvas) {
        m_canvas->disconnectCanvasObserver(this);
    }

    m_canvas = dynamic_cast<KisCanvas2*>(canvas);

    qDebug() << m_canvas << "..." << canvas;

    Q_ASSERT(m_canvas);
    
    if (!m_canvas) return;

    connect(m_canvas->resourceManager(), SIGNAL(resourceChanged(int, const QVariant&)),
            this, SLOT(resourceChanged(int, const QVariant&)));

    delete m_localCanvas;
    m_localCanvas = 0;
    delete m_viewConverter;
    m_viewConverter = 0;
    delete m_canvasController;
    m_canvasController = 0;
    m_resourceProvider = 0;

    if (m_canvas && m_canvas->view()) {

        m_viewConverter = new KisCoordinatesConverter();

//        m_canvasController = new KisCanvasController(this, m_canvas->view()->actionCollection());
//        m_canvasController->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//        m_canvasController->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//        m_canvasController->setDrawShadow(false);
//        m_canvasController->setCanvasMode(KoCanvasController::Infinite);
//        m_canvasController->setZoomWithWheel(true);
//        m_canvasController->setVastScrolling(false);

        m_resourceProvider = m_canvas->view()->resourceProvider();

        // XXX: implement view interface so this class can be passed?
        m_localCanvas = new KisCanvas2(m_viewConverter, m_canvas->view(), m_canvas->view()->document()->shapeController());

        //connect(m_resourceProvider, SIGNAL(sigDisplayProfileChanged(const KoColorProfile *)), m_localCanvas, SLOT(slotSetDisplayProfile(const KoColorProfile *)));

//        m_canvasController->setCanvas(m_localCanvas);

        m_viewConverter->setZoom(KoZoomMode::ZOOM_PAGE);

        setWidget(m_localCanvas->canvasWidget());
    }
}

void OverviewDockerDock::resourceChanged(int key, const QVariant& v)
{
}

#include "overviewdocker_dock.moc"
