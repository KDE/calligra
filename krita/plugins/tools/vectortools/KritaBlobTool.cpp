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

#include "KritaBlobTool.h"

#include <KoPathShape.h>
#include <KoParameterShape.h>
#include <KoShapeStroke.h>
#include <KoPointerEvent.h>
#include <KoCanvasBase.h>
#include <KoShapeController.h>
#include <KoShapeManager.h>
#include <KoSelection.h>
#include <KoCanvasResourceManager.h>
#include <KoColor.h>
#include <KoPathPoint.h>
#include <KoPathPointData.h>
#include <KoPathPointMergeCommand.h>
#include <KoShapePaintingContext.h>

#include <knuminput.h>
#include <klocale.h>
#include <kcombobox.h>

#include <QStackedWidget>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QPainter>
#include <QLabel>
#include <QPainterPath>

#include "../../../../karbon/plugins/tools/KarbonCurveFit.cpp"

KritaBlobTool::KritaBlobTool(KoCanvasBase *canvas)
              : KoToolBase(canvas)
{
    m_shape = 0;
    m_qshape = 0;
    m_stroke = new KoShapeStroke(canvas->resourceManager()->activeStroke());
    m_stroke->setColor(canvas->resourceManager()->foregroundColor().toQColor());
}

KritaBlobTool::~KritaBlobTool()
{
}

void KritaBlobTool::paint(QPainter &painter, const KoViewConverter &converter)
{
    if (m_qshape) {
        painter.save();
        m_shape = KoPathShape::createShapeFromPainterPath(*m_qshape);
        m_shape->setShapeId(KoPathShapeId);
        painter.setTransform(m_shape->absoluteTransformation(&converter) * painter.transform());
        KoShapePaintingContext paintContext;
        if (m_shape->stroke()) {
            painter.save();
            m_shape->stroke()->paint(m_shape, painter, converter);
            painter.restore();
        }
    }
}

void KritaBlobTool::repaintDecorations()
{
}

#include <KoCanvasController.h>

void KritaBlobTool::mousePressEvent(KoPointerEvent *event)
{
    QPointF center = event->point;
    center.setX(center.x() - 10);
    center.setY(center.y() - 10);
    QRectF area = QRectF(center, QSizeF(20, 20));
    if (!m_qshape) {
        m_qshape = new QPainterPath;
        m_qshape->addEllipse(area);
    }
    //canvas()->updateCanvas(m_qshape->boundingRect());
}

void KritaBlobTool::mouseMoveEvent(KoPointerEvent *event)
{
    qDebug() << "Mouse moved to ";
    qDebug() << event->pos();
    
    QPointF center = event->point;
    center.setX(center.x() - 10);
    center.setY(center.y() - 10);
    if (m_qshape) {
        QRectF area = QRectF(center, QSizeF(20, 20));
        QPainterPath elli;
        elli.addEllipse(area);
        *m_qshape = m_qshape->united(elli);
    }
    //canvas()->updateCanvas(m_qshape->boundingRect());
}

void KritaBlobTool::mouseReleaseEvent(KoPointerEvent *event)
{
    qDebug() << "Mouse has been released";
    
    KoPathShape *path = 0;
    if (m_simplified) {
        QList<QPointF> heuristicPointList;
        for (qreal t = 0; t <= 1; t += 0.005) {
            heuristicPointList.append(m_qshape->pointAtPercent(t));
        }   
    //heuristicPointList.append(m_qshape->pointAtPercent(0));
    float hardcodedError = 2;
    path = bezierFit(heuristicPointList, hardcodedError);
    path->close();
    path->normalize();
    }
    else {  //loads of control points
        path = KoPathShape::createShapeFromPainterPath(*m_qshape);
    }
    path->setShapeId(KoPathShapeId);
    path->setStroke(m_stroke);
    KUndo2Command *cmd = canvas()->shapeController()->addShape(path);
    
    if (cmd) {
        KoSelection *selection = canvas()->shapeManager()->selection();
        selection->deselectAll();
        selection->select(path);
        canvas()->addCommand(cmd);
        m_qshape = 0;
        m_shape = 0;
    } else {
        canvas()->updateCanvas(path->boundingRect());
        delete path;
    }
}

void KritaBlobTool::keyPressEvent(QKeyEvent *event)
{
}

void KritaBlobTool::activate(ToolActivation, const QSet<KoShape*> &)
{
    useCursor(Qt::ArrowCursor);
}

void KritaBlobTool::deactivate()
{
    delete m_shape;
    delete m_qshape;
    m_shape = 0;
    m_qshape = 0;
}

void KritaBlobTool::slotSetSimplified(int simplified)
{
    if (simplified == Qt::Checked) {
        m_simplified = true;
    } else {
        m_simplified = false;
    }
}

QWidget *KritaBlobTool::createOptionWidget()
{
    QWidget *optionWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(optionWidget);

    QHBoxLayout *modeLayout = new QHBoxLayout;
    QLabel *modeLabel = new QLabel(i18n("Simplified: "), optionWidget);
    QCheckBox *simplified = new QCheckBox();
    modeLayout->addWidget(modeLabel);
    modeLayout->addWidget(simplified);
    layout->addLayout(modeLayout);

    connect(simplified, SIGNAL(stateChanged(int)), this, SLOT(slotSetSimplified(int)));
    
    simplified->setCheckState(Qt::Checked);
    return optionWidget;
}


#include "KritaBlobTool.moc"
