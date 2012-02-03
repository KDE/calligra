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
#include "kto_main_window.h"

KtoCanvas::KtoCanvas(QDeclarativeItem* parent): QDeclarativeItem(parent), m_mainWindow(0), m_displayProfile(0)
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
    setAcceptedMouseButtons(Qt::LeftButton);
}

KtoCanvas::~KtoCanvas()
{

}

void KtoCanvas::paint(QPainter* painter, const QStyleOptionGraphicsItem* , QWidget* )
{
    painter->fillRect(boundingRect(), Qt::gray);
    if(m_mainWindow and m_mainWindow->document()->image())
    {
        KisPaintDeviceSP projection = m_mainWindow->document()->image()->projection();
        QRect r = boundingRect().toRect();
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

void KtoCanvas::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    qDebug() << "oy";
    QGraphicsItem::mousePressEvent(event);
}

#include "kto_canvas.moc"
