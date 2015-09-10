/* This file is part of the KDE project
 * Copyright (C) 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "ScribbleArea.h"
#include <QMouseEvent>
#include <QPainter>

ScribbleArea::ScribbleArea(QQuickItem* parent)
    : QQuickPaintedItem(parent)
    , scribbling(false)
    , myPenWidth(10)
    , myPenColor(QColor(242, 178, 0))
{
    setAcceptedMouseButtons(Qt::LeftButton);\
    image = QImage(width(), height(), QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);
}

ScribbleArea::~ScribbleArea()
{
}

void ScribbleArea::clear()
{
    image.fill(Qt::transparent);
}

QColor ScribbleArea::color() const
{
    return myPenColor;
}

void ScribbleArea::setColor(const QColor& newColor)
{
    myPenColor = newColor;
    emit colorChanged();
}

int ScribbleArea::penWidth() const
{
    return myPenWidth;
}

void ScribbleArea::setPenWidth(const int& newWidth)
{
    myPenWidth = newWidth;
    emit penWidthChanged();
}

bool ScribbleArea::event(QEvent* event)
{
    if(event->type() == QEvent::Resize) {
        image = QImage(width(), height(), QImage::Format_ARGB32_Premultiplied);
        image.fill(Qt::transparent);
    }
    return QQuickPaintedItem::event(event);
}

void ScribbleArea::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        lastPoint = event->pos();
        scribbling = true;
        emit paintingStarted();
    }
}

void ScribbleArea::mouseMoveEvent(QMouseEvent* event)
{
    if ((event->buttons() & Qt::LeftButton) && scribbling)
        drawLineTo(event->pos());
}

void ScribbleArea::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && scribbling) {
        drawLineTo(event->pos());
        scribbling = false;
        emit paintingStopped();
    }
}

void ScribbleArea::paint(QPainter* painter)
{
    painter->drawImage(boundingRect(), image);
}

void ScribbleArea::drawLineTo(const QPointF& endPoint)
{
    if(image.isNull()) {
        image = QImage(width(), height(), QImage::Format_ARGB32_Premultiplied);
        image.fill(Qt::transparent);
    }
    QPainter painter(&image);
    painter.setPen(QPen(myPenColor, myPenWidth, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawLine(lastPoint, endPoint);
    painter.end();

    int rad = (myPenWidth / 2) + 2;
    update(QRectF(lastPoint, endPoint).normalized()
                                    .adjusted(-rad, -rad, +rad, +rad).toRect());
    lastPoint = endPoint;
}
