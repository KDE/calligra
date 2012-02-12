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

#ifndef _KTO_CANVAS_H_
#define _KTO_CANVAS_H_

#include <QDeclarativeItem>
#include <kis_types.h>

class KisPostExecutionUndoAdapter;
class KisUpdateScheduler;
class KisUndoStore;
class KisPaintingInformationBuilder;
class KisToolFreehandHelper;
class KoCanvasResourceManager;
class KtoCanvasNodeListener;
class KoColorProfile;
class KtoMainWindow;

class KtoCanvas : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(KoCanvasResourceManager* resourceManager READ resourceManager)
public:
    KtoCanvas(QDeclarativeItem* parent = 0);
    virtual ~KtoCanvas();
    virtual void paint(QPainter* , const QStyleOptionGraphicsItem* , QWidget* );
    void setMainWindow(KtoMainWindow* _mainWindow);
    // A callback for our own node graph listener
    void imageUpdated(const QRect &rect);
    KoCanvasResourceManager* resourceManager() const;
protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
private:
    KtoMainWindow* m_mainWindow;
    const KoColorProfile* m_displayProfile;
    
    KisPaintDeviceSP m_paintDevice;
    KisPaintLayerSP  m_paintLayer;
    
    KtoCanvasNodeListener*          m_nodeListener;
    KoCanvasResourceManager*        m_resourceManager;    
    KisPaintingInformationBuilder*  m_infoBuilder;
    KisToolFreehandHelper*          m_helper;
    KisUpdateScheduler*             m_updateScheduler;
    KisUndoStore*                   m_undoStore;
    KisPostExecutionUndoAdapter*    m_undoAdapter;
};

#endif
