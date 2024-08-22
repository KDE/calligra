/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 */

#include "ScribbleArea.h"
#include <QMouseEvent>
#include <QPainter>

ScribbleArea::ScribbleArea(QQuickItem *parent)
    : QQuickPaintedItem(parent)
    , scribbling(false)
    , myPenWidth(10)
    , myPenColor(QColor(242, 178, 0))
{
    setAcceptedMouseButtons(Qt::LeftButton);
    image = QImage(width(), height(), QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);
}

ScribbleArea::~ScribbleArea() = default;

void ScribbleArea::clear()
{
    image.fill(Qt::transparent);
}

QColor ScribbleArea::color() const
{
    return myPenColor;
}

void ScribbleArea::setColor(const QColor &newColor)
{
    myPenColor = newColor;
    emit colorChanged();
}

int ScribbleArea::penWidth() const
{
    return myPenWidth;
}

void ScribbleArea::setPenWidth(const int &newWidth)
{
    myPenWidth = newWidth;
    emit penWidthChanged();
}

bool ScribbleArea::event(QEvent *event)
{
    if (event->type() == QEvent::Resize) {
        image = QImage(width(), height(), QImage::Format_ARGB32_Premultiplied);
        image.fill(Qt::transparent);
    }
    return QQuickPaintedItem::event(event);
}

void ScribbleArea::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        lastPoint = event->pos();
        scribbling = true;
        emit paintingStarted();
    }
}

void ScribbleArea::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) && scribbling)
        drawLineTo(event->pos());
}

void ScribbleArea::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && scribbling) {
        drawLineTo(event->pos());
        scribbling = false;
        emit paintingStopped();
    }
}

void ScribbleArea::paint(QPainter *painter)
{
    painter->drawImage(boundingRect(), image);
}

void ScribbleArea::drawLineTo(const QPointF &endPoint)
{
    if (image.isNull()) {
        image = QImage(width(), height(), QImage::Format_ARGB32_Premultiplied);
        image.fill(Qt::transparent);
    }
    QPainter painter(&image);
    painter.setPen(QPen(myPenColor, myPenWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawLine(lastPoint, endPoint);
    painter.end();

    int rad = (myPenWidth / 2) + 2;
    update(QRectF(lastPoint, endPoint).normalized().adjusted(-rad, -rad, +rad, +rad).toRect());
    lastPoint = endPoint;
}
