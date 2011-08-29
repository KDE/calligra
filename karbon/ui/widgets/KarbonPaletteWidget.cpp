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
#include <KLocale>
#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>

KarbonPaletteWidget::KarbonPaletteWidget(QWidget *parent)
    : QWidget(parent)
    , m_orientation(Qt::Horizontal)
    , m_scrollOffset(0)
    , m_palette(0)
    , m_pressedIndex(-1)
{
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
    m_scrollOffset = qMin(m_scrollOffset+1, maximalScrollOffset());
    update();
}

void KarbonPaletteWidget::scrollBackward()
{
    m_scrollOffset = qMin(qMax(0, m_scrollOffset-1), maximalScrollOffset());
    update();
}

void KarbonPaletteWidget::setPalette(KoColorSet *colorSet)
{
    m_palette = colorSet;
    m_scrollOffset = 0;
    if (m_palette) {
        setToolTip(QString(i18n("Color Palette: %1").arg(m_palette->name())));
    } else {
        setToolTip(i18n("No color palette available"));
    }
    update();
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

void KarbonPaletteWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (!m_palette)
        return;

    const int releasedIndex = indexFromPosition(event->pos());
    if (releasedIndex == m_pressedIndex) {
        if (releasedIndex < m_palette->nColors()) {
            emit colorSelected(m_palette->getColor(releasedIndex).color);
        }
    }
}

int KarbonPaletteWidget::indexFromPosition(const QPoint &position)
{
    QSize colorSize = patchSize();
    if (m_orientation == Qt::Horizontal) {
        return position.x() / colorSize.width() + m_scrollOffset;
    } else {
        return position.y() / colorSize.height() + m_scrollOffset;
    }
}

QSize KarbonPaletteWidget::patchSize() const
{
    const int patchSize = m_orientation == Qt::Horizontal ? height() : width();
    return QSize(patchSize-1, patchSize-1);

}
#include "KarbonPaletteWidget.moc"
