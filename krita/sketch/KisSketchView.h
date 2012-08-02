/* This file is part of the KDE project
 * Copyright (C) 2012 Boudewijn Rempt <boud@kogmbh.com>
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
#ifndef KRITA_SKETCH_VIEW_H
#define KRITA_SKETCH_VIEW_H

#include "CanvasControllerDeclarative.h"

class KisSketchView : public CanvasControllerDeclarative
{
    Q_OBJECT
    Q_PROPERTY(QObject* document READ doc)

public:
    KisSketchView(QDeclarativeItem* parent = 0);
    virtual ~KisSketchView();

    QObject* doc() const;

    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);

    virtual void componentComplete();
    virtual void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry);

public Q_SLOTS:

    /// XXX: add parameter options... Or how does QML interact with a class like this?
    void createDocument();
    void loadDocument();

Q_SIGNALS:

    void doubleTapped();

private:
    class Private;
    Private * const d;

    Q_PRIVATE_SLOT(d, void update())

    QPointF documentToView(const QPointF &point);
    QPointF viewToDocument(const QPointF &point);

private Q_SLOTS:
    virtual void onSingleTap(const QPointF &location);
    virtual void onDoubleTap(const QPointF& location);
    virtual void onLongTap(const QPointF& location);
    virtual void onLongTapEnd(const QPointF& location);
};

#endif // KRITA_SKETCH_CANVAS_H
