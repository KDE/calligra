/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2012 Shantanu Tushar <shaan7in@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include "CACanvasItem.h"

#include <KoCanvasBase.h>

#include <KDebug>
#include <KoCanvasController.h>

#include <QGraphicsWidget>
#include <QTimer>

CACanvasItem::CACanvasItem(QDeclarativeItem* parent)
    : QDeclarativeItem(parent)
    , m_koCanvas(0)
    , m_shouldIgnoreGeometryChange(false)
    , m_editable(false)
{
    setFlag (QGraphicsItem::ItemHasNoContents, false);
}

KoCanvasBase* CACanvasItem::koCanvas()
{
    return m_koCanvas;
}

void CACanvasItem::setKoCanvas(KoCanvasBase* koCanvas)
{
    if (!koCanvas)
        return;
    if (m_koCanvasGraphicsWidget) {
        m_koCanvasGraphicsWidget->removeEventFilter(this);
        m_koCanvasGraphicsWidget->disconnect(this);
    }
    m_koCanvas = koCanvas;
    m_koCanvasGraphicsWidget = dynamic_cast<QGraphicsWidget*>(koCanvas);
    m_koCanvasGraphicsWidget->setParentItem(this);
    m_koCanvasGraphicsWidget->installEventFilter(this);
    m_koCanvasGraphicsWidget->setVisible(true);
    m_koCanvasGraphicsWidget->setGeometry(x(), y(), width(), height());
    connect(m_koCanvasGraphicsWidget, SIGNAL(geometryChanged()), SLOT(resizeToCanvas()));
}

bool CACanvasItem::eventFilter(QObject* o, QEvent* e)
{
    if (o == m_koCanvasGraphicsWidget) {
        if (e->type() == QEvent::Move) {
            m_shouldIgnoreGeometryChange = true;
            QMoveEvent *moveEvent = static_cast<QMoveEvent*>(e);
            setPos(QPointF(moveEvent->pos()));
        } else if (e->type() == QEvent::Resize) {
            m_shouldIgnoreGeometryChange = true;
            QResizeEvent *resizeEvent = static_cast<QResizeEvent*>(e);
            setWidth(resizeEvent->size().width());
            setHeight(resizeEvent->size().height());
        } else if (m_editable) {
            return false;
        } else if (e->type() == QEvent::MouseButtonPress ||
                   e->type() == QEvent::MouseButtonRelease ||
                   e->type() == QEvent::MouseMove ||
                   e->type() == QEvent::MouseTrackingChange)  {
            return true;
        } else {
            return false;
        }
    }
    QTimer::singleShot(0, this, SLOT(resetShouldIgnoreGeometryChange()));
    return QDeclarativeItem::eventFilter(o, e);
}

void CACanvasItem::resetShouldIgnoreGeometryChange()
{
    m_shouldIgnoreGeometryChange = false;
}

void CACanvasItem::resizeToCanvas()
{
    if (!m_koCanvasGraphicsWidget)
        return;
    m_shouldIgnoreGeometryChange = true;
    setPos(m_koCanvasGraphicsWidget->geometry().topLeft());
    setWidth(m_koCanvasGraphicsWidget->geometry().size().width());
    setHeight(m_koCanvasGraphicsWidget->geometry().size().height());

    QTimer::singleShot(0, this, SLOT(resetShouldIgnoreGeometryChange()));
}

bool CACanvasItem::editable() const
{
    return m_editable;
}

void CACanvasItem::setEditable(bool value)
{
    m_editable = value;
    emit editableChanged();
}

void CACanvasItem::updateDocumentSize(QSize sz, bool recalculateCenter)
{
    setHeight(sz.height());
    setWidth(sz.width());

    if (m_koCanvasGraphicsWidget) {
        m_koCanvasGraphicsWidget->setGeometry(x(), y(), width(), height());
    }
}

CACanvasItem::~CACanvasItem()
{

}

#include "CACanvasItem.moc"
