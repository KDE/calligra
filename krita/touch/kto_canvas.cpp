/*
 * Copyright (C) 2012 Cyrille Berger <cberger@cberger.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 sure
 * USA
 */

#include "kto_canvas.h"

#include <QDebug>
#include <QPainter>
#include <kis_image.h>
#include <kis_doc2.h>
#include <kis_paint_layer.h>
#include "kto_main_window.h"
#include "kto_resource_information_list.h"
#include "kto_resource_information.h"
#include <KoColorSpaceRegistry.h>
#include <KoCanvasResourceManager.h>
#include <kis_painting_information_builder.h>
#include <kis_tool_freehand_helper.h>
#include <KoPointerEvent.h>
#include <KoResourceServerAdapter.h>
#include <KoResourceModel.h>
#include <kis_update_scheduler.h>
#include <kis_undo_stores.h>
#include <kis_post_execution_undo_adapter.h>
#include <QGraphicsSceneMouseEvent>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <kis_paintop_registry.h>
#include <kis_canvas_resource_provider.h>
#include <kis_resource_server_provider.h>
#include <KStandardDirs>

class KtoCanvasNodeListener : public KisNodeGraphListener
{
public:
    KtoCanvasNodeListener(KtoCanvas *canvas)
        : m_ktoCanvas(canvas)
    {
    }

    void aboutToAddANode(KisNode *, int) {}
    void nodeHasBeenAdded(KisNode *, int) {}
    void aboutToRemoveANode(KisNode *, int) {}
    void nodeHasBeenRemoved(KisNode *, int) {}
    void aboutToMoveNode(KisNode *, int, int) {}
    void nodeHasBeenMoved(KisNode *, int, int) {}
    void nodeChanged(KisNode*) {}

    void requestProjectionUpdate(KisNode *node, const QRect& rect)
    {
        Q_UNUSED(node);

        QMutexLocker locker(&m_lock);
        m_ktoCanvas->imageUpdated(rect);
    }

private:
    KtoCanvas *m_ktoCanvas;
    QMutex m_lock;
};

class KtoCanvasDefaultBounds : public KisDefaultBounds
{
public:

    KtoCanvasDefaultBounds(KtoCanvas *canvas)
        : m_canvas(canvas)
    {
    }

    QRect bounds() const {
        return m_canvas->boundingRect().toAlignedRect();
    }

private:
    KtoCanvas *m_canvas;
};

KtoCanvas::KtoCanvas(QDeclarativeItem* parent): QDeclarativeItem(parent), m_mainWindow(0), m_displayProfile(0)
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
    setAcceptedMouseButtons(Qt::LeftButton);
    
    m_nodeListener = new KtoCanvasNodeListener(this);
    
    m_resourceManager = new KoCanvasResourceManager;
    
    KoResourceServer<KisPaintOpPreset> * rserver = KisResourceServerProvider::instance()->paintOpPresetServer();
    KoResourceServerAdapter<KisPaintOpPreset>* proxyAdapter = new KoResourceServerAdapter<KisPaintOpPreset>(rserver);
    m_brushResourceModel = new KtoResourceInformationList(proxyAdapter, this);

    m_infoBuilder = new KisPaintingInformationBuilder();
    m_helper = new KisToolFreehandHelper(m_infoBuilder);
    
    m_updateScheduler = new KisUpdateScheduler(0);
    m_undoStore = new KisSurrogateUndoStore();
    m_undoAdapter = new KisPostExecutionUndoAdapter(m_undoStore, m_updateScheduler);
    
    m_paintDevice = new KisPaintDevice(KoColorSpaceRegistry::instance()->rgb8(), "KritaTouchs");
    
    KoColor defaultColor(Qt::white, KoColorSpaceRegistry::instance()->rgb8());

    
    m_paintLayer = new KisPaintLayer(0, "ScratchPad", OPACITY_OPAQUE_U8, m_paintDevice);
    m_paintLayer->setGraphListener(m_nodeListener);
    m_paintDevice->setDefaultBounds(new KtoCanvasDefaultBounds(this));

    m_paintDevice->setDefaultPixel(defaultColor.data());
    m_paintDevice->clear();
    
    QString paintopid = "paintbrush";
    KGlobal::mainComponent().dirs()->addResourceType("kis_defaultpresets", "data", "krita/defaultpresets/");
    QString defaultName = paintopid + ".kpp";
    QString path = KGlobal::mainComponent().dirs()->findResource("kis_defaultpresets", defaultName);
    
    KisPaintOpPresetSP preset = new KisPaintOpPreset(path);

    if (!preset->load()) {
        preset = KisPaintOpRegistry::instance()->defaultPreset(KoID(paintopid), KisImageWSP(0));
    }

    Q_ASSERT(preset);
    Q_ASSERT(preset->valid());

    QVariant v;
    v.setValue(preset);
    m_resourceManager->setResource(KisCanvasResourceProvider::CurrentPaintOpPreset, v);
    
    v.setValue(KoColor(Qt::black, KoColorSpaceRegistry::instance()->rgb8()));
    m_resourceManager->setResource(KoCanvasResourceManager::ForegroundColor, v);

    v.setValue(KoColor(Qt::black, KoColorSpaceRegistry::instance()->rgb8()));
    m_resourceManager->setResource(KoCanvasResourceManager::BackgroundColor, v);
    
    v.setValue(1.0);
    m_resourceManager->setResource(KisCanvasResourceProvider::Opacity, v);
}

KtoCanvas::~KtoCanvas()
{
    delete m_helper;
    delete m_infoBuilder;

    delete m_undoAdapter;
    delete m_undoStore;
    delete m_updateScheduler;
    delete m_nodeListener;
}

void KtoCanvas::imageUpdated(const QRect& rect)
{
    update(rect);
}

void KtoCanvas::paint(QPainter* painter, const QStyleOptionGraphicsItem* , QWidget* )
{
    painter->fillRect(boundingRect(), Qt::gray);
//     if(m_mainWindow and m_mainWindow->document()->image())
    {
        QRect r = boundingRect().toRect();
        m_paintLayer->updateProjection(r);
        KisPaintDeviceSP projection = m_paintLayer->projection();
        QImage image = projection->convertToQImage(m_displayProfile, r.x(), r.y(), r.width(), r.height());
        
        painter->setRenderHints(QPainter::SmoothPixmapTransform);
        painter->drawImage(QPoint(0, 0), image);
    }
}

void KtoCanvas::setMainWindow(KtoMainWindow* _mainWindow)
{
    Q_ASSERT(m_mainWindow == 0);
    m_mainWindow = _mainWindow;
}

void KtoCanvas::mousePressEvent(QGraphicsSceneMouseEvent* _event)
{
    KoPointerEvent kpEvent(_event, _event->pos());
    m_helper->initPaint(&kpEvent, m_resourceManager,
                        0,
                        m_updateScheduler,
                        m_undoAdapter,
                        m_paintLayer);
}

void KtoCanvas::mouseMoveEvent(QGraphicsSceneMouseEvent* _event)
{
    KoPointerEvent kpEvent(_event, _event->pos());
    m_helper->paint(&kpEvent);
}

void KtoCanvas::mouseReleaseEvent(QGraphicsSceneMouseEvent* _event)
{
    m_helper->endPaint();
}

KoCanvasResourceManager* KtoCanvas::resourceManager() const
{
    return m_resourceManager;
}

KtoResourceInformationList* KtoCanvas::brushResourceModel() const
{
    return m_brushResourceModel;
}

void KtoCanvas::setBrushPreset(QObject* _resourceInformation)
{
    KisPaintOpPresetSP preset = static_cast<KisPaintOpPreset*>(qobject_cast<KtoResourceInformation*>(_resourceInformation)->resource());
    
    QVariant v;
    v.setValue(preset);
    m_resourceManager->setResource(KisCanvasResourceProvider::CurrentPaintOpPreset, v);    
}

void KtoCanvas::undo()
{
    m_undoStore->undo();
}


#include "kto_canvas.moc"
