/* This file is part of the KDE project

   Copyright (C) 2008 C. Boemann <cbo@boemann.dk>
   Copyright (C) 2012 Paul Mendez <paulestebanms@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KPrAnimationTool.h"

//Qt Headers
#include <QList>
#include <QLabel>
#include <QPainter>

//KDE Headers
#include <klocale.h>

//Calligra Headers
#include <KoPointerEvent.h>
#include <KoPACanvas.h>
#include <KoPAViewBase.h>
#include <KoPADocument.h>
#include <KoViewConverter.h>
#include <KoShapeManager.h>
#include <KoSelection.h>
#include <KoPathShape.h>
#include <KoPAPage.h>
#include <KoZoomHandler.h>
#include <KoShapePaintingContext.h>
#include <KoShapeStroke.h>
#include <KoZoomController.h>

//internal Headers
#include <KPrPageEffectDocker.h>
#include "KPrClickActionDocker.h"
#include "KPrShapeAnimationDocker.h"
#include "KPrShapeApplicationData.h"
#include "animations/KPrAnimateMotion.h"
#include "KPrPageData.h"

const int HANDLE_DISTANCE = 10;

KPrAnimationTool::KPrAnimationTool( KoCanvasBase *canvas )
    : KoPathTool( canvas )
    , m_currentMotionPathSelected(0)
    , m_reloadMotionPaths(true)
{
}

KPrAnimationTool::~KPrAnimationTool()
{
}

void KPrAnimationTool::paint( QPainter &painter, const KoViewConverter &converter)
{
    foreach (KoShape *shape, canvas()->shapeManager()->selection()->selectedShapes(KoFlake::StrippedSelection)) {
        painter.save();

        // save the original painter transformation
        QTransform painterMatrix = painter.worldTransform();

        painter.setPen(Qt::green);
        // apply the shape transformation on top of the old painter transformation
        painter.setWorldTransform(shape->absoluteTransformation(&converter) *painterMatrix);
        // apply the zoom factor
        KoShape::applyConversion(painter, converter);
        // draw the shape bounding rect
        painter.drawRect(QRectF(QPointF(), shape->size()));
        painterMatrix = painter.worldTransform();
        painter.restore();
    }
    // Restore motion path shapes after animation preview
    foreach(KoPathShape *shape, m_motionPaths) {
        if (!canvas()->shapeManager()->shapes().contains(shape)) {
            canvas()->shapeManager()->addShape(shape);
        }
    }
    KoPathTool::paint(painter, converter);
}


void KPrAnimationTool::activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes)
{
    Q_UNUSED(toolActivation);
    Q_UNUSED(shapes);
    useCursor(Qt::ArrowCursor);
    repaintDecorations();

    if (m_reloadMotionPaths) {
        reloadMotionPaths();
    }

    // Init parent tool if motion path shape is selected
    QList<KoPathShape*> selectedShapes;
    foreach(KoShape *shape, shapes) {
        KoPathShape *pathShape = dynamic_cast<KoPathShape*>(shape);
        if (shape->isEditable() && pathShape && !shape->isPrintable()) {
            if (m_currentMotionPathSelected == pathShape) {
                return;
            }
            selectedShapes.append(pathShape);
        }
    }
    if (!selectedShapes.isEmpty()) {
        KoPathTool::activate(toolActivation, shapes);
    }
    useCursor(Qt::ArrowCursor);
}

void KPrAnimationTool::deactivate()
{
    // Clean shape manager of motion paths
    foreach(KoPathShape *shape, m_motionPaths) {
        canvas()->shapeManager()->remove(shape);
    }
    m_motionPaths.clear();
    m_reloadMotionPaths = true;
    m_pathMap.clear();
    m_shapesMap.clear();
    KoPathTool::deactivate();
}

void KPrAnimationTool::mousePressEvent( KoPointerEvent *event )
{
    //If no shape was click deselect all
    KoSelection *selection = canvas()->shapeManager()->selection();
    foreach (KoShape* shape, selection->selectedShapes()) {
        shape->update();
    }
    selection->deselectAll();
    //Select clicked shape
    KoShape *shape = canvas()->shapeManager()->shapeAt(event->point);
    if (shape) {
        selection->select(shape);
        selection->update();
        shape->update();
    }
    // Init tool if motion shape is selected
    if (KoPathShape *pathShape = dynamic_cast<KoPathShape*>(shape)) {
        if (!pathShape->isPrintable()) {
            QSet<KoShape*> shapes;
            shapes << pathShape;
            m_reloadMotionPaths = false;
            activate(DefaultActivation, shapes);
            m_currentMotionPathSelected = pathShape;
        }
    }
    KoPathTool::mousePressEvent(event);
}

void KPrAnimationTool::repaintDecorations()
{
    if (canvas()->shapeManager()->selection()->count() > 0) {
        canvas()->updateCanvas(handlesSize());
    }
}

QRectF KPrAnimationTool::handlesSize()
{
    QRectF bound = canvas()->shapeManager()->selection()->boundingRect();

    // expansion Border
    if (!canvas() || !canvas()->viewConverter()) return bound;

    QPointF border = canvas()->viewConverter()->viewToDocument(QPointF(HANDLE_DISTANCE, HANDLE_DISTANCE));
    bound.adjust(-border.x(), -border.y(), border.x(), border.y());
    return bound;
}

QList<QWidget *> KPrAnimationTool::createOptionWidgets()
{
    KPrPageEffectDocker *effectWidget = new KPrPageEffectDocker( );
    effectWidget->setView((dynamic_cast<KoPACanvas *>(canvas()))->koPAView());

    KPrClickActionDocker *clickActionWidget = new KPrClickActionDocker();
    clickActionWidget->setView((dynamic_cast<KoPACanvas *>(canvas()))->koPAView());

    KPrShapeAnimationDocker *shapeAnimationWidget = new KPrShapeAnimationDocker();
    shapeAnimationWidget->setView((dynamic_cast<KoPACanvas *>(canvas()))->koPAView());
    connect(shapeAnimationWidget, SIGNAL(shapeAnimationsChanged(KoShape*)), this, SLOT(verifyMotionPathChanged(KoShape*)));
    connect(shapeAnimationWidget, SIGNAL(motionPathAddedRemoved()), this, SLOT(reloadMotionPaths()));

    QList<QWidget *> widgets;
    effectWidget->setWindowTitle(i18n("Slide Transitions"));
    widgets.append(effectWidget);
    clickActionWidget->setWindowTitle(i18n("Shape Click Actions"));
    widgets.append(clickActionWidget);   
    shapeAnimationWidget->setWindowTitle(i18n("Shape Animations"));
    widgets.append(shapeAnimationWidget);
    return widgets;
}

void KPrAnimationTool::loadMotionPathShapes()
{
    // Clear previous Data
    m_motionPaths.clear();
    m_pathMap.clear();
    m_shapesMap.clear();
    //Load motion paths Data
    KPrPageData *pageData = dynamic_cast<KPrPageData *>((dynamic_cast<KoPACanvas *>(canvas()))->koPAView()->activePage());
    Q_ASSERT(pageData);
    KPrShapeAnimations *animations =  &(pageData->animations());
    for (int j = 0; j < animations->rowCount(); j++) {
        KPrShapeAnimation *anim = animations->animationByRow(j);
        if (anim->presetClass() == KPrShapeAnimation::MotionPath) {
            for (int i = 0; i < anim->animationCount(); i++) {
                if (KPrAnimateMotion *motion = dynamic_cast<KPrAnimateMotion *>(anim->animationAt(i))) {
                    // Load motion path
                    QPainterPath outlinePath = motion->path();
                    qreal outlineX = outlinePath.boundingRect().x();
                    qreal outlineY = outlinePath.boundingRect().y();                   
                    QPair<qreal, qreal> scaleCorrection = getScaleCorrection();

                    outlineX = outlineX * scaleCorrection.first;
                    outlineY = outlineY * scaleCorrection.second;
                    outlinePath = outlinePath * QTransform().scale(scaleCorrection.first,
                                                                   scaleCorrection.second);
                    KoPathShape *path = KoPathShape::createShapeFromPainterPath(outlinePath);
                    path->setPosition(QPointF(anim->shape()->position().x() + anim->shape()->size().width() / 2 + outlineX,
                                              anim->shape()->position().y() + anim->shape()->size().height() / 2 + outlineY));
                    m_pathMap.insert(path, motion);
                    m_shapesMap.insert(path, anim->shape());
                    // change stroke apparience
                    KoShapeStroke *stroke = new KoShapeStroke();
                    QVector<qreal> dashes;
                    qreal space = 8;
                    dashes << 1 << space << 3 << space;
                    stroke->setLineStyle(Qt::DashLine, dashes);
                    stroke->setLineWidth(2);
                    stroke->setColor(Qt::gray);
                    path->setStroke(stroke);
                    addPathShape(path);
                }
            }
        }
    }
}

void KPrAnimationTool::addPathShape(KoPathShape *pathShape)
{
    m_motionPaths.append(pathShape);
    pathShape->setPrintable(false);
    canvas()->shapeManager()->addShape(pathShape);
}

void KPrAnimationTool::paintPath(KoPathShape &pathShape, QPainter &painter, const KoViewConverter &converter)
{
    painter.setTransform(pathShape.absoluteTransformation(&converter) * painter.transform());
    painter.save();

    KoShapePaintingContext paintContext; //FIXME
    pathShape.paint(painter, converter, paintContext);
    painter.restore();
    if (pathShape.stroke()) {
        painter.save();
        pathShape.stroke()->paint(&pathShape, painter, converter);
        painter.restore();
    }
}

QPair<qreal, qreal> KPrAnimationTool::getScaleCorrection()
{
    qreal zoom;
    (dynamic_cast<KoPACanvas *>(canvas()))->koPAView()->zoomHandler()->zoom(&zoom, &zoom);
    QSizeF pageSize = dynamic_cast<KoPACanvas *>(canvas())->koPAView()->zoomController()->documentSize();
    return QPair<qreal, qreal>(pageSize.width() * zoom, pageSize.height() * zoom);
}

void KPrAnimationTool::saveMotionPath()
{
    QMapIterator<KoPathShape *, KPrAnimateMotion *> i(m_pathMap);
    while (i.hasNext()) {
        i.next();
        QPainterPath outlinePath = i.key()->outline();

        QPair<qreal, qreal> scaleCorrection = getScaleCorrection();

        qreal offsetX = -(m_shapesMap.value(i.key())->size().width() / 2 +
                          m_shapesMap.value(i.key())->position().x() - i.key()->position().x());
        qreal offsetY = -(m_shapesMap.value(i.key())->size().height() / 2 +
                          m_shapesMap.value(i.key())->position().y() - i.key()->position().y());

        outlinePath = outlinePath * QTransform().translate(offsetX, offsetY);
        outlinePath = outlinePath * QTransform().scale(1/(scaleCorrection.first),
                                                       1/(scaleCorrection.second));
        i.value()->setPath(outlinePath);
    }
}

void KPrAnimationTool::reloadMotionPaths()
{
    // Clean Shapes Manager
    foreach(KoPathShape *shape, m_motionPaths) {
        canvas()->shapeManager()->remove(shape);
    }
    m_motionPaths.clear();
    // Remove handles
    QList<KoPathShape*> shapes;
    m_pointSelection.setSelectedShapes(shapes);
    m_pointSelection.update();
    // Load motion paths
    loadMotionPathShapes();
    connect(canvas()->shapeManager(), SIGNAL(contentChanged()), this, SLOT(saveMotionPath()));
}

void KPrAnimationTool::verifyMotionPathChanged(KoShape *shape)
{
    QMapIterator<KoPathShape *, KoShape *> i(m_shapesMap);
    while (i.hasNext()) {
        i.next();
        if (i.value() == shape) {
            reloadMotionPaths();
        }
    }
}

#include "KPrAnimationTool.moc"
