/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ListLevelChooser.h"

#include <QDebug>
#include <QPainter>
#include <QPushButton>

ListLevelChooser::ListLevelChooser(const int offset, QWidget *parent)
    : QPushButton("", parent)
    , m_offset(offset)
{
    setFlat(true);
    setMinimumSize(QSize(256, 20));
}

void ListLevelChooser::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPushButton::paintEvent(event);
    QPainter painter(this);
    painter.save();
    painter.setPen(QPen(painter.pen().brush(), 1, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin));
    QRect rectang = rect();
    // painter.fillRect(rectang, QBrush(QColor(Qt::white)));
    painter.translate(m_offset, 1.5);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawText(rectang, Qt::AlignVCenter, QString::fromUtf8("●"));

    int lineY = rectang.y() + (rectang.height() / 2);
    painter.drawLine(13, lineY, rectang.bottomRight().x() - m_offset - 15, lineY);

    painter.restore();
}
