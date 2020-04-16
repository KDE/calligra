/* This file is part of the KDE project

   Copyright 2010 Johannes Simon <johannes.simon@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// Calligra
#include <KoUnit.h>
#include <KoDpi.h>

// KoChart
#include "ScreenConversions.h"

// Qt
#include <QPainter>
#include <QWidget>
#include <QSize>
#include <QSizeF>
#include <QRectF>
#include <QRect>

using namespace KoChart;

qreal ScreenConversions::pxToPtX(qreal px)
{
    return KoUnit(KoUnit::Inch).fromUserValue(px / KoDpi::dpiX());
}

qreal ScreenConversions::pxToPtY(qreal px)
{
    return KoUnit(KoUnit::Inch).fromUserValue(px / KoDpi::dpiY());
}

qreal ScreenConversions::ptToPxX(qreal pt, const QPaintDevice* paintDevice)
{
    const int dpiX = paintDevice ? paintDevice->logicalDpiX() : KoDpi::dpiX();
    return KoUnit::toInch(pt) * dpiX;
}

qreal ScreenConversions::ptToPxY(qreal pt, const QPaintDevice* paintDevice)
{
    const int dpiY = paintDevice ? paintDevice->logicalDpiY() : KoDpi::dpiY();
    return KoUnit::toInch(pt) * dpiY;
}

qreal ScreenConversions::ptToPxX(qreal pt)
{
    return KoUnit::toInch(pt) * KoDpi::dpiX();
}

qreal ScreenConversions::ptToPxY(qreal pt)
{
    return KoUnit::toInch(pt) * KoDpi::dpiY();
}

void ScreenConversions::scaleFromPtToPx(QPainter &painter)
{
    const qreal inPerPt = KoUnit::toInch(1.0);
    int dpiX;
    int dpiY;
    if (dynamic_cast<QWidget*>(painter.device()) != 0) {
        dpiX = KoDpi::dpiX();
        dpiY = KoDpi::dpiY();
    } else {
        dpiX = painter.device()->logicalDpiX();
        dpiY = painter.device()->logicalDpiY();
    }
    painter.scale(1.0 / (inPerPt * dpiX), 1.0 / (inPerPt * dpiY));
}

QSize ScreenConversions::scaleFromPtToPx(const QSizeF &size, const QPaintDevice* paintDevice)
{
    return QSizeF(ptToPxX(size.width(), paintDevice),
                  ptToPxY(size.height(), paintDevice)).toSize();
}

QSize ScreenConversions::scaleFromPtToPx(const QSizeF &size, QPainter &painter)
{
    QPaintDevice* paintDevice = painter.device();
    if (dynamic_cast<QWidget*>(paintDevice) != 0) {
        paintDevice = 0;
    }

    return scaleFromPtToPx(size, paintDevice);
}

QSize ScreenConversions::scaleFromPtToPx(const QSizeF &size)
{
    return QSizeF(ptToPxX(size.width()), ptToPxY(size.height())).toSize();
}

QSizeF ScreenConversions::scaleFromPxToPt(const QSize &size)
{
    return QSizeF(pxToPtX(size.width()), pxToPtY(size.height()));
}

QPoint ScreenConversions::scaleFromPtToPx(const QPointF &point, const QPaintDevice* paintDevice)
{
    return QPointF(ptToPxX(point.x(), paintDevice),
                   ptToPxY(point.y(), paintDevice)).toPoint();
}

QRect ScreenConversions::scaleFromPtToPx(const QRectF &rect, QPainter &painter)
{
    QPaintDevice* paintDevice = painter.device();
    if (dynamic_cast<QWidget*>(paintDevice) != 0) {
        paintDevice = 0;
    }

    return QRect(scaleFromPtToPx(rect.topLeft(), paintDevice),
                 scaleFromPtToPx(rect.size(), paintDevice));
}

QSizeF ScreenConversions::toWidgetDpi(QWidget *widget, const QSizeF &size)
{
    QSizeF s = size;
    const qreal scaleX = static_cast<qreal>(widget->logicalDpiX()) / KoDpi::dpiX();
    const qreal scaleY = static_cast<qreal>(widget->logicalDpiY()) / KoDpi::dpiY();
    s.setWidth(s.width() * scaleX);
    s.setHeight(s.height() * scaleY);
    return s;
}

QSizeF ScreenConversions::fromWidgetDpi(QWidget *widget, const QSizeF &size)
{
    QSizeF s = size;
    const qreal scaleX = static_cast<qreal>(KoDpi::dpiX()) / widget->logicalDpiX();
    const qreal scaleY = static_cast<qreal>(KoDpi::dpiY()) / widget->logicalDpiY();
    s.setWidth(s.width() * scaleX);
    s.setHeight(s.height() * scaleY);
    return s;
}

void ScreenConversions::scaleToWidgetDpi(QWidget *widget, QPainter &painter)
{
    // only scale if we paint into a QWidget
    if (dynamic_cast<QWidget*>(painter.device())) {
        const qreal scaleX = static_cast<qreal>(KoDpi::dpiX()) / widget->logicalDpiX();
        const qreal scaleY = static_cast<qreal>(KoDpi::dpiY()) / widget->logicalDpiY();
        painter.scale(scaleX, scaleY);
    }
}
