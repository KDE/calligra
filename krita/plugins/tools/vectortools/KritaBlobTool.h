/* This file is part of the KDE project
 * Copyright (C) 2012 José Luis Vergara <pentalis@gmail.com>
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

#ifndef _KRITABLOBTOOL_H_
#define _KRITABLOBTOOL_H_

#include <KoToolBase.h>
#include <QRectF>

class KoPathShape;
class KoShapeStroke;
class KoShapeBackground;
class KoPathPoint;
class QPainterPath;

class KritaBlobTool : public KoToolBase
{
    Q_OBJECT
    
public:
    explicit KritaBlobTool(KoCanvasBase *canvas);
    ~KritaBlobTool();

    void paint(QPainter &painter, const KoViewConverter &converter);
    void repaintDecorations();

    void mousePressEvent(KoPointerEvent *event) ;
    void mouseMoveEvent(KoPointerEvent *event);
    void mouseReleaseEvent(KoPointerEvent *event);
    void keyPressEvent(QKeyEvent *event);

    void activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes);
    void deactivate();

protected:
    QWidget *createOptionWidget();
    
private:
    KoPathShape *m_shape;
    QPainterPath *m_qshape;
    KoShapeStroke *m_stroke;
    
};

#endif // _KRITABLOBTOOL_H_
