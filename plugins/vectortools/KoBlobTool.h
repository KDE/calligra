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

#ifndef _KOBLOBTOOL_H_
#define _KOBLOBTOOL_H_

#include <KoToolBase.h>
#include <QRectF>

class KoPathShape;
class KoShapeStroke;
class KoPathPoint;
class QPainterPath;
class KoShapeBackground;

/**
  The purpose of this tool is to create blotch-shaped vector shapes that fuse with any other
  shape on the canvas sharing the same attributes of stroke and background
*/
class KoBlobTool : public KoToolBase
{
    Q_OBJECT
    
public:
    explicit KoBlobTool(KoCanvasBase *canvas);
    ~KoBlobTool();

    void paint(QPainter &painter, const KoViewConverter &converter);
    void repaintDecorations();

    void mousePressEvent(KoPointerEvent *event) ;
    void mouseMoveEvent(KoPointerEvent *event);
    void mouseReleaseEvent(KoPointerEvent *event);
    //void keyPressEvent(QKeyEvent *event);

    void activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes);
    void deactivate();

public slots:
    void slotSetSimplified(int simplified);
    void slotSetDiameter(double diameter);
    void slotSetOptimization(double error);
    
protected:
    QWidget *createOptionWidget();
    
private:
    void addDab(const QPointF &pos);
    void combineBlob();
    
    KoPathShape *m_shape;
    QPainterPath *m_qshape;
    KoShapeStroke *m_stroke;
    KoShapeBackground *m_bg;
    
    qreal m_diameter;
    /// if simplified is activated, then the higher m_error is, the less control points in the final blob 
    qreal m_error;
    bool m_simplified;
};

#endif // _KOBLOBTOOL_H_
