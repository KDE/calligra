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

#include "KoBlobTool.h"

#include <KoPathShape.h>
#include <KoShapeStroke.h>
#include <KoPointerEvent.h>
#include <KoCanvasBase.h>
#include <KoShapeController.h>
#include <KoShapeManager.h>
#include <KoSelection.h>
#include <KoCanvasResourceManager.h>
#include <KoColor.h>
#include <KoColorBackground.h>
#include <KoPathPoint.h>
#include <KoPathPointData.h>
#include <KoPathPointMergeCommand.h>
#include <KoShapePaintingContext.h>
#include <KoShapeBackground.h>
#include <knuminput.h>
#include <klocale.h>

#include <QStackedWidget>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QPainter>
#include <QLabel>
#include <QPainterPath>

#include "../../karbon/plugins/tools/KarbonCurveFit.cpp"

KoBlobTool::KoBlobTool(KoCanvasBase *canvas)
              : KoToolBase(canvas)
{
    m_shape = 0;
    m_qshape = 0;
    m_stroke = new KoShapeStroke(canvas->resourceManager()->activeStroke());
    m_stroke->setColor(canvas->resourceManager()->foregroundColor().toQColor());
    m_bg = new KoColorBackground(QColor(0, 0, 0));
}

KoBlobTool::~KoBlobTool()
{
}

void KoBlobTool::paint(QPainter &painter, const KoViewConverter &converter)
{
    if (m_shape) {
        painter.save();
        painter.setTransform(m_shape->absoluteTransformation(&converter) * painter.transform());
        painter.save();
        KoShapePaintingContext paintContext;
        m_shape->paint(painter, converter, paintContext);
        painter.restore();/*
        if (m_shape->stroke()) {
            painter.save();
            m_shape->stroke()->paint(m_shape, painter, converter);
            painter.restore();
        }*/
        painter.restore();
    }
}

void KoBlobTool::repaintDecorations()
{
}

#include <KoCanvasController.h>

void KoBlobTool::mousePressEvent(KoPointerEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        addDab(event->point);
    }
}

void KoBlobTool::mouseMoveEvent(KoPointerEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        addDab(event->point);
    }
}

void KoBlobTool::mouseReleaseEvent(KoPointerEvent *)
{
    qDebug() << "Mouse has been released";
    
    KoPathShape *path = 0;
    if (m_simplified) {
        QList<QPointF> heuristicPointList;
        heuristicPointList = m_qshape->toFillPolygons().at(0).toList();
        /*
        for (qreal t = 0; t <= 1; t += 0.005) {
            heuristicPointList.append(m_qshape->pointAtPercent(t));
            qDebug() << m_qshape->pointAtPercent(t);
        }   
        heuristicPointList.append(m_qshape->pointAtPercent(0));
        */
        path = bezierFit(heuristicPointList, m_error);
        path->close();
        path->normalize();
        path = bezierMultipathFit(*m_qshape, m_error);
    }
    else {  //loads of control points
        KoPathShape * shape = new KoPathShape();

        QPolygonF poly = m_qshape->toFillPolygons().at(0);
        for (int i = 0; i < poly.count(); i++) {
            shape->lineTo(poly.at(i));
        }
        shape->normalize();
        path = shape;
        qDebug() << "FIRE!";
        //path = KoPathShape::createShapeFromPainterPath(*m_qshape);
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
/*
void KoBlobTool::keyPressEvent(QKeyEvent *event)
{
}
*/

void KoBlobTool::activate(ToolActivation, const QSet<KoShape*> &)
{
    useCursor(Qt::ArrowCursor);
}

void KoBlobTool::deactivate()
{
    delete m_shape;
    delete m_qshape;
    m_shape = 0;
    m_qshape = 0;
}

void KoBlobTool::slotSetSimplified(int simplified)
{
    if (simplified == Qt::Checked) {
        m_simplified = true;
    } else {
        m_simplified = false;
    }
}

void KoBlobTool::slotSetDiameter(double diameter)
{
    m_diameter = diameter;
}

void KoBlobTool::slotSetOptimization(double error)
{
    m_error = error;
}


void KoBlobTool::addDab(const QPointF &pos)
{
    QPointF center;
    center.setX(pos.x() - m_diameter/2);
    center.setY(pos.y() - m_diameter/2);
    QRectF area = QRectF(center, QSizeF(m_diameter, m_diameter));
    
    if (!m_qshape) {
        m_qshape = new QPainterPath;
        m_qshape->addEllipse(area);
    }
    else {
        QPainterPath elli;
        elli.addEllipse(area);
        *m_qshape = m_qshape->united(elli);
    }
    m_shape = KoPathShape::createShapeFromPainterPath(*m_qshape);
    m_shape->setShapeId(KoPathShapeId);
    m_shape->setStroke(m_stroke);
    m_shape->setBackground(m_bg);
    canvas()->updateCanvas(m_shape->boundingRect());
}


void KoBlobTool::combineBlob()
{
    /*
    This function is left blank until we correct our shape styling inconsistencies.
    The blob tool depends on proper shape styling to work, since it will only fuse with shapes sharing the same stroke/background.
    */
}

QWidget *KoBlobTool::createOptionWidget()
{
    QWidget         *optionWidget   = new QWidget();
    QVBoxLayout     *layout         = new QVBoxLayout(optionWidget);

    QHBoxLayout     *line1          = new QHBoxLayout;
    QLabel          *labelDiameter  = new QLabel(i18n("Diameter: "), optionWidget);
    KDoubleNumInput *diameterSlider = new KDoubleNumInput(1, 1000, 20, optionWidget, 10, 1);
    diameterSlider->setSliderEnabled(true);
    diameterSlider->setExponentRatio(2);
    
    
    line1->addWidget(labelDiameter);
    line1->addWidget(diameterSlider);
    
    QLabel          *optimiLabel    = new QLabel(i18n("Simplification (higher for less control points): "), optionWidget);
    
    QHBoxLayout     *line2          = new QHBoxLayout;
    QCheckBox       *simplified     = new QCheckBox("Simplify", optionWidget);
    KDoubleNumInput *simplifySlider = new KDoubleNumInput(1, 20, 2, optionWidget, 1, 2);
    simplifySlider->setSliderEnabled(true);
    line2->addWidget(simplified);
    line2->addWidget(simplifySlider);
    
    layout->addLayout(line1);
    layout->addWidget(optimiLabel);
    layout->addLayout(line2);
    layout->addStretch(1);

    connect(simplified,     SIGNAL(stateChanged(int)),    this          , SLOT(slotSetSimplified(int))      );
    connect(simplified,     SIGNAL(clicked(bool)),        simplifySlider, SLOT(setEnabled(bool))            );
    connect(simplifySlider, SIGNAL(valueChanged(double)), this          , SLOT(slotSetOptimization(double)) );
    connect(diameterSlider, SIGNAL(valueChanged(double)), this          , SLOT(slotSetDiameter(double))     );
    
    simplified->setCheckState(Qt::Checked);
    
    m_diameter = diameterSlider->value();
    m_error    = simplifySlider->value();
    return optionWidget;
}


#include "KoBlobTool.moc"
