/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "MusicWidget.h"

#include <QPainter>

#include "../core/Sheet.h"

MusicWidget::MusicWidget(QWidget *parent)
    : QWidget(parent)
    , m_renderer(&m_style)
    , m_sheet(nullptr)
    , m_scale(1.0)
    , m_lastSystem(0)
{
}

void MusicWidget::setSheet(MusicCore::Sheet *sheet)
{
    m_sheet = sheet;
    engrave();
}

MusicCore::Sheet *MusicWidget::sheet() const
{
    return m_sheet;
}

void MusicWidget::setScale(qreal scale)
{
    m_scale = scale;
    engrave();
}

qreal MusicWidget::scale() const
{
    return m_scale;
}

void MusicWidget::engrave()
{
    if (m_sheet) {
        m_engraver.engraveSheet(m_sheet, 0, QSizeF((width() - 1) / m_scale, height() / m_scale), true, &m_lastSystem);
    }
}

void MusicWidget::paintEvent(QPaintEvent *)
{
    if (!m_sheet)
        return;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    painter.scale(m_scale, m_scale);
    m_renderer.renderSheet(painter, m_sheet, 0, m_lastSystem);
}

void MusicWidget::resizeEvent(QResizeEvent *)
{
    engrave();
}
