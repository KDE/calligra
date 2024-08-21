/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "StaffElementPreviewWidget.h"

#include "../MusicStyle.h"
#include "../Renderer.h"

#include "../core/Clef.h"
#include "../core/Staff.h"

#include <QBrush>
#include <QPainter>

using namespace MusicCore;

StaffElementPreviewWidget::StaffElementPreviewWidget(QWidget *parent)
    : QWidget(parent)
    , m_style(nullptr)
    , m_renderer(nullptr)
{
    m_staff = new Staff(nullptr);
    m_clef = new Clef(m_staff, 0, Clef::Trebble, 2, 0);
}

StaffElementPreviewWidget::~StaffElementPreviewWidget()
{
    delete m_renderer;
}

void StaffElementPreviewWidget::setMusicStyle(MusicStyle *style)
{
    m_style = style;
    delete m_renderer;
    m_renderer = new MusicRenderer(m_style);
}

QSize StaffElementPreviewWidget::sizeHint() const
{
    return QSize(180, 75);
}

void StaffElementPreviewWidget::setStaffElement(MusicCore::StaffElement *se)
{
    m_element = se;
    update();
}

Staff *StaffElementPreviewWidget::staff()
{
    return m_staff;
}

void StaffElementPreviewWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.fillRect(rect(), QBrush(Qt::white));
    if (!m_style)
        return;
    painter.translate(0, height() / 2);

    painter.scale(1.5, 1.5);
    painter.setPen(m_style->staffLinePen());
    for (int i = -2; i <= 2; i++) {
        painter.drawLine(QPointF(0, 5.0 * i), QPointF(width(), 5.0 * i));
    }

    m_style->renderClef(painter, 5, 5.0 /* staff line distance */, MusicCore::Clef::Trebble);

    MusicRenderer::RenderState state;
    state.clef = m_clef;
    m_renderer->renderStaffElement(painter, m_element, QPointF(20.0 + m_clef->width(), -10.0), state);
}
