/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2009 Jan Hambrecht <jaham@gmx.net>
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

#include "KarbonPrintJob.h"
#include "KarbonView.h"
#include "KarbonCanvas.h"
#include "KarbonPart.h"

#include <KoShapeManager.h>

#include <QtGui/QPainter>

KarbonPrintJob::KarbonPrintJob(KarbonView *view)
        : KoPrintingDialog(view),
        m_view(view)
{
    setShapeManager(m_view->canvasWidget()->shapeManager());
    printer().setFromTo(1, 1);

    QSizeF pageSize = m_view->part()->document().pageSize();
    if (pageSize.width() > pageSize.height())
        printer().setOrientation(QPrinter::Landscape);
    else
        printer().setOrientation(QPrinter::Portrait);
}

QRectF KarbonPrintJob::preparePage(int)
{
    // if we have any custom tabs, here is where can can read them out and do our thing.

    const QSizeF contentSize = m_view->part()->document().pageSize();
    const QRectF pageRectPt = printer().pageRect(QPrinter::Point);
    const double scale = POINT_TO_INCH(printer().resolution());

    qreal zoom = 1.0;
    // fit document page to printer page if it is bigger than the printing page rect
    if (contentSize.width() > pageRectPt.width() || contentSize.height() > pageRectPt.height()) {
        qreal zoomX = pageRectPt.width() / contentSize.width();
        qreal zoomY = pageRectPt.height() / contentSize.height();
        zoom = qMin(zoomX, zoomY);
    }

    painter().scale(zoom, zoom);
    painter().setRenderHint(QPainter::Antialiasing);

    return QRectF(QPointF(), scale * contentSize);
}

QList<KoShape*> KarbonPrintJob::shapesOnPage(int)
{
    return shapeManager()->shapes();
}

QList<QWidget*> KarbonPrintJob::createOptionWidgets() const
{
    return QList<QWidget*>();
}
