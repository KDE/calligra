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

#include "kis_color_selector_item.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>

#include <KoColorSpaceRegistry.h>
#include <KoCanvasResourceManager.h>

#include "kis_color_selector_wheel.h"

KisColorSelectorItem::KisColorSelectorItem(QDeclarativeItem* parent): QDeclarativeItem(parent), m_component(new KisColorSelectorWheel(this, this)), m_resourceManager(0)
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
    setAcceptedMouseButtons(Qt::LeftButton);
  
    m_component->setConfiguration(KisColorSelectorSurface::hslSH, KisColorSelectorSurface::Wheel);
}

const KoColorSpace* KisColorSelectorItem::colorSpace() const
{
    return KoColorSpaceRegistry::instance()->rgb8();
}

void KisColorSelectorItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* , QWidget* )
{
    QSize currentSize(width(), height());
    if(currentSize != m_lastSize)
    {
        m_component->setGeometry(0, 0, width(), height());
        m_lastSize = currentSize;
    }
    
    m_component->paintEvent(painter);
}

void KisColorSelectorItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    m_component->mouseEvent(event->pos().x(), event->pos().y());
    update();
}

void KisColorSelectorItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    m_component->mouseEvent(event->pos().x(), event->pos().y());
    update();
}

void KisColorSelectorItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if(event->button()&Qt::LeftButton || event->buttons()&Qt::RightButton)
    {
        m_component->mouseEvent(event->pos().x(), event->pos().y());
        KoColor currentColor = KoColor(m_component->currentColor(), colorSpace());
        commitColor(currentColor, Foreground);
    }
    update();
}

KoCanvasResourceManager* KisColorSelectorItem::resourceManager() const
{
    return m_resourceManager;
}

void KisColorSelectorItem::setResourceManager(KoCanvasResourceManager* _canvasResourceManager)
{
    qDebug() << _canvasResourceManager;
    m_resourceManager = _canvasResourceManager;
    m_component->setColor(findGeneratingColor(m_resourceManager->foregroundColor()));
}
