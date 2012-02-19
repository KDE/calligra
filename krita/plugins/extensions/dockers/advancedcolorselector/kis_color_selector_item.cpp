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
#include <KGlobal>
#include <KConfigGroup>

#include <KoColorSpaceRegistry.h>
#include <KoCanvasResourceManager.h>

#include "kis_color_selector_wheel.h"

KisColorSelectorItem::KisColorSelectorItem(QDeclarativeItem* parent): QDeclarativeItem(parent), m_surface(this), m_resourceManager(0)
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
    setAcceptedMouseButtons(Qt::LeftButton);
    
    KConfigGroup cfg = KGlobal::config()->group("advancedColorSelector");
    setConfiguration(KisColorSelectorSurface::Configuration::fromString(cfg.readEntry("colorSelectorConfiguration", KisColorSelectorSurface::Configuration().toString())));
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
        m_surface.resize(width(), height(), 0);
        m_lastSize = currentSize;
    }
    
    m_surface.paint(painter);
}

void KisColorSelectorItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    m_surface.processMouseMove(event->pos().x(), event->pos().y(), event->buttons());
}

void KisColorSelectorItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    m_surface.processMousePress(event->pos().x(), event->pos().y(), event->buttons());
}

void KisColorSelectorItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if(event->button()&Qt::LeftButton || event->buttons()&Qt::RightButton)
    {
        m_surface.processMouseRelease(event->pos().x(), event->pos().y(), event->buttons());
        KoColor currentColor = KoColor(m_surface.currentColor(), colorSpace());
        commitColor(currentColor, Foreground);
    }
}

KoCanvasResourceManager* KisColorSelectorItem::resourceManager() const
{
    return m_resourceManager;
}

void KisColorSelectorItem::setResourceManager(KoCanvasResourceManager* _canvasResourceManager)
{
    m_resourceManager = _canvasResourceManager;
    m_surface.setColor(findGeneratingColor(m_resourceManager->foregroundColor()));
}

void KisColorSelectorItem::setConfiguration(KisColorSelectorSurface::Configuration conf)
{
    m_surface.setConfiguration(conf);
    QObject::connect(m_surface.mainComponent(), SIGNAL(update()), this,   SLOT(update()), Qt::UniqueConnection);
    QObject::connect(m_surface.subComponent(),  SIGNAL(update()), this,   SLOT(update()), Qt::UniqueConnection);
}

void KisColorSelectorItem::update(const QRectF& rect)
{
    QDeclarativeItem::update(rect);
}

