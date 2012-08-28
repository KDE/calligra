/* This file is part of the KDE project
 *
 * Copyright (c) 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
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

#ifndef KIS_SKETCH_CANVAS_H
#define KIS_SKETCH_CANVAS_H

#include <QWidget>
#include "kis_canvas_widget_base.h"

class KisSketchCanvas : public QWidget, public KisCanvasWidgetBase
{
    Q_OBJECT
public:
    KisSketchCanvas(KisCanvas2 *canvas, KisCoordinatesConverter *coordinatesConverter, QWidget *parent);
    virtual ~KisSketchCanvas();

    virtual bool callFocusNextPrevChild(bool next);
    virtual QWidget* widget();

    virtual void initialize();
    virtual void resize(int w, int h);

    QPoint translation() const;
    qreal rotation() const;
    qreal scaling() const;

    uint texture() const;

public Q_SLOTS:
    void resizeImpl();

Q_SIGNALS:
    void renderFinished();

private:
    class Private;
    Private * const d;
};

#endif // KIS_SKETCH_CANVAS_H
