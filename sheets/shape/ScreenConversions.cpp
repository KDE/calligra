/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2010 Johannes Simon <johannes.simon@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ScreenConversions.h"

// Calligra
#include <KoDpi.h>
#include <KoUnit.h>

// Qt
#include <QPainter>
#include <QWidget>

using namespace Calligra::Sheets;

qreal ScreenConversions::pxToPtX(qreal px)
{
    return KoUnit(KoUnit::Inch).fromUserValue(px / KoDpi::dpiX());
}

qreal ScreenConversions::pxToPtY(qreal px)
{
    return KoUnit(KoUnit::Inch).fromUserValue(px / KoDpi::dpiY());
}

qreal ScreenConversions::ptToPxX(qreal pt, const QPaintDevice *paintDevice)
{
    const int dpiX = paintDevice ? paintDevice->logicalDpiX() : KoDpi::dpiX();
    return KoUnit::toInch(pt) * dpiX;
}

qreal ScreenConversions::ptToPxY(qreal pt, const QPaintDevice *paintDevice)
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
    if (dynamic_cast<QWidget *>(painter.device()) != 0) {
        dpiX = KoDpi::dpiX();
        dpiY = KoDpi::dpiY();
    } else {
        dpiX = painter.device()->logicalDpiX();
        dpiY = painter.device()->logicalDpiY();
    }
    painter.scale(1.0 / (inPerPt * dpiX), 1.0 / (inPerPt * dpiY));
}

QSize ScreenConversions::scaleFromPtToPx(const QSizeF &size, const QPaintDevice *paintDevice)
{
    return QSizeF(ptToPxX(size.width(), paintDevice), ptToPxY(size.height(), paintDevice)).toSize();
}

QSize ScreenConversions::scaleFromPtToPx(const QSizeF &size, QPainter &painter)
{
    QPaintDevice *paintDevice = painter.device();
    if (dynamic_cast<QWidget *>(paintDevice) != 0) {
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

QPoint ScreenConversions::scaleFromPtToPx(const QPointF &point)
{
    return QPointF(ptToPxX(point.x()), ptToPxY(point.y())).toPoint();
}

QPoint ScreenConversions::scaleFromPtToPx(const QPointF &point, const QPaintDevice *paintDevice)
{
    return QPointF(ptToPxX(point.x(), paintDevice), ptToPxY(point.y(), paintDevice)).toPoint();
}

QRect ScreenConversions::scaleFromPtToPx(const QRectF &rect, QPainter &painter)
{
    QPaintDevice *paintDevice = painter.device();
    if (dynamic_cast<QWidget *>(paintDevice) != 0) {
        paintDevice = 0;
    }

    return QRect(scaleFromPtToPx(rect.topLeft(), paintDevice), scaleFromPtToPx(rect.size(), paintDevice));
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
    if (dynamic_cast<QWidget *>(painter.device())) {
        const qreal scaleX = static_cast<qreal>(KoDpi::dpiX()) / widget->logicalDpiX();
        const qreal scaleY = static_cast<qreal>(KoDpi::dpiY()) / widget->logicalDpiY();
        painter.scale(scaleX, scaleY);
    }
}
