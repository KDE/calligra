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

//KF5 Headers
#include <klocalizedstring.h>

//Calligra Headers
#include <KoPointerEvent.h>
#include <KoPACanvas.h>
#include <KoPAViewBase.h>
#include <KoViewConverter.h>
#include <KoShapeManager.h>
#include <KoSelection.h>
#include <KoPathShape.h>
#include <KoPAPageBase.h>
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

KPrAnimationTool::KPrAnimationTool(KoCanvasBase *canvas)
    : KoPathTool(canvas)
    , m_currentMotionPathSelected(0)
    , m_pathShapeManager(0)
    , m_initializeTool(true)
    , m_shapeAnimationWidget(0)
{
}

KPrAnimationTool::~KPrAnimationTool()
{
    cleanMotionPathManager();
    delete m_pathShapeManager;
}

void KPrAnimationTool::paint( QPainter &painter, const KoViewConverter &converter)
{
    foreach (KoShape *shape, canvas()->shapeManager()->selection()->selectedShapes()) {
        painter.save();
        // save the original painter transformation
        QTransform painterMatrix = painter.worldTransform();
        painter.setPen(QPen(Qt::green, 0));
        // apply the shape transformation on top of the old painter transformation
        painter.setWorldTransform(shape->absoluteTransformation(&converter) *painterMatrix);
        // apply the zoom factor
        KoShape::applyConversion(painter, converter);
        // draw the shape bounding rect
        painter.drawRect(QRectF(QPointF(), shape->size()));
        painterMatrix = painter.worldTransform();
        painter.restore();
    }
    // Paint motion paths
    QMapIterator<KoPathShape *, KPrAnimateMotion *> i(m_animateMotionMap);
    while (i.hasNext()) {
        i.next();
        QSizeF pageSize = getPageSize();
        if (i.value()->currentPageSize() != pageSize) {
            i.value()->getPath(1, pageSize);
        }
    }
    if (m_pathShapeManager) {
        m_pathShapeManager->paint(painter, converter, false);
    }
    KoPathTool::paint(painter, converter);
}


void KPrAnimationTool::activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes)
{
    Q_UNUSED(toolActivation);
    Q_UNUSED(shapes);
    useCursor(m_selectCursor);
    repaintDecorations();

    if (!m_pathShapeManager) {
        m_pathShapeManager = new KoShapeManager(canvas());
    }
    if (m_initializeTool) {
        reloadMotionPaths();
        connect((static_cast<KoPACanvas *>(canvas()))->koPAView()->proxyObject, SIGNAL(activePageChanged()),
                this, SLOT(reloadMotionPaths()));
        if (m_shapeAnimationWidget) {
            connect((static_cast<KoPACanvas *>(canvas()))->koPAView()->proxyObject, SIGNAL(activePageChanged()),
                     m_shapeAnimationWidget, SLOT(slotActivePageChanged()));
        }
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
}

void KPrAnimationTool::deactivate()
{
    // Clean shape manager of motion paths
    cleanMotionPathManager();
    disconnect((static_cast<KoPACanvas *>(canvas()))->koPAView()->proxyObject, SIGNAL(activePageChanged()),
               this, SLOT(reloadMotionPaths()));
    disconnect((static_cast<KoPACanvas *>(canvas()))->koPAView()->proxyObject, SIGNAL(activePageChanged()),
             m_shapeAnimationWidget, SLOT(slotActivePageChanged()));
    m_initializeTool = true;
    delete m_pathShapeManager;
    m_pathShapeManager = 0;
    KoPathTool::deactivate();
}

void KPrAnimationTool::mousePressEvent( KoPointerEvent *event )
{
    //If no shape was click deselect all
    KoSelection *selection = canvas()->shapeManager()->selection();
    foreach (KoShape *shape, selection->selectedShapes()) {
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
    // Init Path tool if motion shape is selected
    shape = m_pathShapeManager->shapeAt(event->point);
    if (KoPathShape *pathShape = dynamic_cast<KoPathShape*>(shape)) {
        if (!pathShape->isPrintable()) {
            QSet<KoShape*> shapes;
            shapes << pathShape;
            m_initializeTool = false;
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
    KoPathTool::repaintDecorations();
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

QList<QPointer<QWidget> > KPrAnimationTool::createOptionWidgets()
{
    KPrPageEffectDocker *effectWidget = new KPrPageEffectDocker( );
    effectWidget->setView((static_cast<KoPACanvas *>(canvas()))->koPAView());

    KPrClickActionDocker *clickActionWidget = new KPrClickActionDocker();
    clickActionWidget->setView((static_cast<KoPACanvas *>(canvas()))->koPAView());

    m_shapeAnimationWidget = new KPrShapeAnimationDocker();
    m_shapeAnimationWidget->setView((static_cast<KoPACanvas *>(canvas()))->koPAView());
    connect(m_shapeAnimationWidget, SIGNAL(shapeAnimationsChanged(KoShape*)), this, SLOT(verifyMotionPathChanged(KoShape*)));
    connect(m_shapeAnimationWidget, SIGNAL(motionPathAddedRemoved()), this, SLOT(reloadMotionPaths()));
    connect((static_cast<KoPACanvas *>(canvas()))->koPAView()->proxyObject, SIGNAL(activePageChanged()),
             m_shapeAnimationWidget, SLOT(slotActivePageChanged()));

    QList<QPointer<QWidget> > widgets;
    effectWidget->setWindowTitle(i18n("Slide Transitions"));
    widgets.append(effectWidget);
    clickActionWidget->setWindowTitle(i18n("Shape Click Actions"));
    widgets.append(clickActionWidget);
    m_shapeAnimationWidget->setWindowTitle(i18n("Shape Animations"));
    widgets.append(m_shapeAnimationWidget);
    return widgets;
}

void KPrAnimationTool::initMotionPathShapes()
{
    cleanMotionPathManager();
    //Load motion paths Data
    KPrPageData *pageData = dynamic_cast<KPrPageData *>((static_cast<KoPACanvas *>(canvas()))->koPAView()->activePage());
    Q_ASSERT(pageData);
    KPrShapeAnimations *animations =  &(pageData->animations());
    for (int j = 0; j < animations->rowCount(); j++) {
        KPrShapeAnimation *anim = animations->animationByRow(j);
        if (anim->presetClass() == KPrShapeAnimation::MotionPath) {
            for (int i = 0; i < anim->animationCount(); i++) {
                if (KPrAnimateMotion *motion = dynamic_cast<KPrAnimateMotion *>(anim->animationAt(i))) {
                    // Load motion path
                    QSizeF pageSize = getPageSize();
                    KoPathShape *path = motion->getPath(1, pageSize);
                    m_animateMotionMap.insert(path, motion);
                    m_shapesMap.insert(path, anim->shape());
                    // change stroke appearance
                    KoShapeStroke *stroke = new KoShapeStroke();
                    QVector<qreal> dashes;
                    qreal space = 8;
                    dashes << 1 << space << 3 << space;
                    stroke->setLineStyle(Qt::DashLine, dashes);
                    stroke->setLineWidth(1);
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
    if (!m_pathShapeManager) {
        return;
    }
    pathShape->setPrintable(false);
    m_pathShapeManager->addShape(pathShape);
}

QSizeF KPrAnimationTool::getPageSize()
{
    QSizeF pageSize = static_cast<KoPACanvas *>(canvas())->koPAView()->zoomController()->pageSize();
    return pageSize;
}

void KPrAnimationTool::cleanMotionPathManager()
{
    if (!m_pathShapeManager) {
        return;
    }
    foreach(KoShape *shape, m_pathShapeManager->shapes()) {
        m_pathShapeManager->remove(shape);
    }
    m_animateMotionMap.clear();
    m_shapesMap.clear();
    m_currentMotionPathSelected = 0;
}

void KPrAnimationTool::reloadMotionPaths()
{
    // Remove handles
    m_pointSelection.clear();
    m_pointSelection.setSelectedShapes(QList<KoPathShape*>());
    m_pointSelection.update();
    // Load motion paths
    initMotionPathShapes();
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
