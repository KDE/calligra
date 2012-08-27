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

#ifndef KIS_GL2_CANVAS_H
#define KIS_GL2_CANVAS_H

#include <GL/glew.h>
#include <QGLWidget>

#include "kis_canvas_widget_base.h"
#include "kis_types.h"
#include "krita_export.h"

class KRITAUI_EXPORT KisGL2Canvas : public QGLWidget, public KisCanvasWidgetBase
{
    Q_OBJECT
public:
    KisGL2Canvas(KisCanvas2 *canvas, KisCoordinatesConverter *coordinatesConverter, KisImageWSP image, QWidget *parent);
    virtual ~KisGL2Canvas();

    virtual bool callFocusNextPrevChild(bool next);
    virtual QWidget* widget();

    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int w, int h);

    QPoint translation() const;
    qreal rotation() const;
    qreal scaling() const;

    static QGLWidget *shareWidget();

private:
    class Private;
    Private * const d;
};

#endif // KIS_GL2_CANVAS_H
