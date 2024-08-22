/**
 * SPDX-FileCopyrightText: 2006 C. Boemann (cbo@boemann.dk)
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "KoColorPatch.h"

#include <QPainter>

KoColorPatch::KoColorPatch(QWidget *parent)
    : QFrame(parent)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

KoColorPatch::~KoColorPatch() = default;

QSize KoColorPatch::sizeHint() const
{
    return QSize(12, 12);
}

void KoColorPatch::setColor(const KoColor &c)
{
    m_color = c;

    update();
}

KoColor KoColorPatch::color() const
{
    return m_color;
}

void KoColorPatch::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e);

    Q_EMIT triggered(this);
}

void KoColorPatch::paintEvent(QPaintEvent *pe)
{
    QColor qc;
    m_color.toQColor(&qc);

    QFrame::paintEvent(pe);
    QPainter painter(this);
    painter.setPen(QPen(qc, 0));
    painter.setBrush(QBrush(qc));
    painter.drawRect(contentsRect());
}
