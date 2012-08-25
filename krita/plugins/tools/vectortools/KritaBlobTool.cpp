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


KritaBlobTool::KritaBlobTool(KoCanvasBase *canvas)  :  KoToolBase(canvas)
{
    m_shape = 0;
    m_qshape = 0;
}

KritaBlobTool::~KritaBlobTool()
{
}

void KritaBlobTool::paint(QPainter &painter, const KoViewConverter &converter)
{
    /*
    if (m_qshape) {
        KoPathShape *path = KoPathShape::createShapeFromPainterPath(*m_qshape);
        path->setShapeId(KoPathShapeId);
        painter.save();
        KoShapePaintingContext paintContext; //FIXME
        m_shape->paint(painter, converter, paintContext);
        painter.restore();
    }
    */
}

void KritaBlobTool::repaintDecorations()
{
}

void KritaBlobTool::mousePressEvent(KoPointerEvent *event)
{
    QRectF area = QRectF(event->point, QSizeF(5, 5));
    if (!m_qshape) {
        m_qshape = new QPainterPath;
        m_qshape->addEllipse(area);
    }
    /*
    if (!m_shape) {
        m_shape = new KoPathShape();
        m_shape->setShapeId(KoPathShapeId);
        KoShapeStroke * stroke = new KoShapeStroke(canvas()->resourceManager()->activeStroke());
        stroke->setColor(canvas()->resourceManager()->foregroundColor().toQColor());
        m_shape->setStroke(stroke);
    }
    m_shape->lineTo(event->point);
    canvas()->updateCanvas(m_shape->boundingRect());
    */
}

void KritaBlobTool::mouseMoveEvent(KoPointerEvent *event)
{
    qDebug() << "Mouse moved to ";
    qDebug() << event->pos();
    
    if (m_qshape) {
        QRectF area = QRectF(event->point, QSizeF(5, 5));
        QPainterPath elli;
        elli.addEllipse(area);
        *m_qshape = m_qshape->united(elli);
    }
}

void KritaBlobTool::mouseReleaseEvent(KoPointerEvent *event)
{
    qDebug() << "Mouse has been released";

    KoPathShape *path = KoPathShape::createShapeFromPainterPath(*m_qshape);
    path->setShapeId(KoPathShapeId);
    KUndo2Command *cmd = canvas()->shapeController()->addShape(path);
    
    if (cmd) {
        KoSelection *selection = canvas()->shapeManager()->selection();
        selection->deselectAll();
        selection->select(path);
        canvas()->addCommand(cmd);
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

QWidget *KritaBlobTool::createOptionWidget()
{
    QWidget *optionWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(optionWidget);

    QHBoxLayout *modeLayout = new QHBoxLayout;
    modeLayout->setSpacing(3);
    QLabel *modeLabel = new QLabel(i18n("THIS WILL BE A CONFIG WIDGET"), optionWidget);
    modeLayout->addWidget(modeLabel);
    layout->addLayout(modeLayout);

    return optionWidget;
}


#include "KritaBlobTool.moc"
