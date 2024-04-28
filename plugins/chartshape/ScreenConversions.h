/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2010 Johannes Simon <johannes.simon@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCHART_SCREEN_CONVERSIONS_H
#define KCHART_SCREEN_CONVERSIONS_H

class QPainter;
class QPaintDevice;
class QPoint;
class QPointF;
class QSize;
class QSizeF;
class QRect;
class QRectF;

namespace KoChart
{

class ScreenConversions
{
public:
    // pt --> px
    static qreal ptToPxX(qreal pt, const QPaintDevice *paintDevice);
    static qreal ptToPxY(qreal pt, const QPaintDevice *paintDevice);
    static qreal ptToPxX(qreal pt);
    static qreal ptToPxY(qreal pt);
    static void scaleFromPtToPx(QPainter &painter);
    static QPoint scaleFromPtToPx(const QPointF &point, const QPaintDevice *paintDevice);
    static QSize scaleFromPtToPx(const QSizeF &size, const QPaintDevice *paintDevice);
    static QSize scaleFromPtToPx(const QSizeF &size, QPainter &painter);
    static QSize scaleFromPtToPx(const QSizeF &size);
    static QRect scaleFromPtToPx(const QRectF &rect, QPainter &painter);

    // px --> pt
    static qreal pxToPtX(qreal px);
    static qreal pxToPtY(qreal px);
    static QSizeF scaleFromPxToPt(const QSize &size);

    // auxcilliary
    static QSizeF toWidgetDpi(QWidget *widget, const QSizeF &size);
    static QSizeF fromWidgetDpi(QWidget *widget, const QSizeF &size);
    static void scaleToWidgetDpi(QWidget *widget, QPainter &painter);
};

} // namespace KoChart

#endif // KCHART_SCREEN_CONVERSIONS_H
