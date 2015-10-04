/* This file is part of the KDE project
 * Copyright (C) 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KarbonPaletteWidget.h"
#include <KoColorSet.h>

#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QToolTip>

KarbonPaletteWidget::KarbonPaletteWidget(QWidget *parent)
    : QWidget(parent)
    , m_orientation(Qt::Horizontal)
    , m_scrollOffset(0)
    , m_palette(0)
    , m_pressedIndex(-1)
    , m_hasDragged(false)
{
    setFocusPolicy(Qt::ClickFocus);
}

KarbonPaletteWidget::~KarbonPaletteWidget()
{
}

void KarbonPaletteWidget::setOrientation(Qt::Orientation orientation)
{
    if (orientation == m_orientation)
        return;
    m_orientation = orientation;
    update();
}

Qt::Orientation KarbonPaletteWidget::orientation() const
{
    return m_orientation;
}

int KarbonPaletteWidget::maximalScrollOffset() const
{
    if (!m_palette)
        return 0;

    const int colorCount = m_palette->nColors();

    if (m_orientation == Qt::Horizontal) {
        const int colorSize = patchSize().width();
        return qMax(0, colorCount - width() / colorSize);
    } else {
        const int colorSize = patchSize().height();
        return qMax(0, colorCount - height() / colorSize);
    }
}

int KarbonPaletteWidget::currentScrollOffset() const
{
    return m_scrollOffset;
}

void KarbonPaletteWidget::scrollForward()
{
    applyScrolling(+1);
}

void KarbonPaletteWidget::scrollBackward()
{
    applyScrolling(-1);
}

void KarbonPaletteWidget::setPalette(KoColorSet *colorSet)
{
    m_palette = colorSet;
    m_scrollOffset = 0;
    update();
    emit scrollOffsetChanged();
}

KoColorSet *KarbonPaletteWidget::palette() const
{
    return m_palette;
}

void KarbonPaletteWidget::paintEvent(QPaintEvent *event)
{
    if (!m_palette) {
        QWidget::paintEvent(event);
        return;
    }

    QSize colorSize = patchSize();
    QPoint colorOffset = m_orientation == Qt::Horizontal ? QPoint(colorSize.width(), 0) : QPoint(0, colorSize.height());
    QPoint colorPos(0, 0);

    QPainter painter(this);

    const int colorCount = m_palette->nColors();
    for (int i = m_scrollOffset; i < colorCount; ++i) {
        // draw color patch
        painter.setBrush(m_palette->getColor(i).color.toQColor());
        painter.drawRect(QRect(colorPos, colorSize));
        // advance drawing position
        colorPos += colorOffset;
        // break when widget border is reached
        if (colorPos.x() > width() || colorPos.y() > height())
            break;
    }
}

void KarbonPaletteWidget::mousePressEvent(QMouseEvent *event)
{
    m_pressedIndex = indexFromPosition(event->pos());
}

void KarbonPaletteWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_pressedIndex != -1) {
        int index = indexFromPosition(event->pos());
        if (index != m_pressedIndex) {
            m_hasDragged = true;
            applyScrolling(m_pressedIndex-index);
            m_pressedIndex = indexFromPosition(event->pos());
        }
    }
}

void KarbonPaletteWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (!m_palette)
        return;

    const int releasedIndex = indexFromPosition(event->pos());
    if (!m_hasDragged && releasedIndex == m_pressedIndex && releasedIndex != -1) {
        emit colorSelected(m_palette->getColor(releasedIndex).color);
    }
    m_pressedIndex = -1;
    m_hasDragged = false;
}

void KarbonPaletteWidget::wheelEvent(QWheelEvent *event)
{
    applyScrolling(-event->delta()/10);
}

void KarbonPaletteWidget::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()) {
    case Qt::Key_Left:
        if (m_orientation == Qt::Horizontal)
            applyScrolling(-1);
        break;
    case Qt::Key_Right:
        if (m_orientation == Qt::Horizontal)
            applyScrolling(1);
        break;
    case Qt::Key_Up:
        if (m_orientation == Qt::Vertical)
            applyScrolling(-1);
        break;
    case Qt::Key_Down:
        if (m_orientation == Qt::Vertical)
            applyScrolling(1);
        break;
    case Qt::Key_PageDown:
        if (m_orientation == Qt::Vertical)
            applyScrolling(height()/patchSize().height());
        else
            applyScrolling(width()/patchSize().width());
        break;
    case Qt::Key_PageUp:
        if (m_orientation == Qt::Vertical)
            applyScrolling(-height()/patchSize().height());
        else
            applyScrolling(-width()/patchSize().width());
        break;
    }
}

bool KarbonPaletteWidget::event(QEvent *event)
{
    if (event->type() == QEvent::ToolTip) {
         QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
         int index = indexFromPosition(helpEvent->pos());
         if (index != -1) {
             KoColorSetEntry entry = m_palette->getColor(index);
             QString text;
             if (!entry.name.isEmpty())
                text += "<center><b>" + entry.name + "</b></center>";
             QColor color = entry.color.toQColor();
             text += QString("%1 (%2 %3 %4)").arg(color.name())
                     .arg(color.red()).arg(color.green()).arg(color.blue());

             QToolTip::showText(helpEvent->globalPos(), text);
         } else {
             QToolTip::hideText();
             event->ignore();
         }

         return true;
     }
     return QWidget::event(event);
}

int KarbonPaletteWidget::indexFromPosition(const QPoint &position)
{
    if (!m_palette)
        return -1;

    QSize colorSize = patchSize();
    int index = -1;
    if (m_orientation == Qt::Horizontal) {
        index = position.x() / colorSize.width() + m_scrollOffset;
    } else {
        index = position.y() / colorSize.height() + m_scrollOffset;
    }
    if (index < 0 || index >= m_palette->nColors())
        return -1;

    return index;
}

QSize KarbonPaletteWidget::patchSize() const
{
    const int patchSize = m_orientation == Qt::Horizontal ? height() : width();
    return QSize(patchSize-1, patchSize-1);
}

void KarbonPaletteWidget::applyScrolling(int delta)
{
    int newScrollOffset = qBound(0, m_scrollOffset+delta, maximalScrollOffset());
    if (newScrollOffset != m_scrollOffset) {
        m_scrollOffset = newScrollOffset;
        update();
        emit scrollOffsetChanged();
    }
}

