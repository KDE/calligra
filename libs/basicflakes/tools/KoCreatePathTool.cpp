/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2006 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2008-2010 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoCreatePathTool.h"
#include "KoCreatePathTool_p.h"

#include "KoDocumentResourceManager.h"
#include "KoPathShape.h"
#include "KoPointerEvent.h"
#include "KoSelection.h"
#include "KoShapePaintingContext.h"
#include "KoShapeStroke.h"
#include <KoColor.h>

#include <KLocalizedString>

#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QPainter>
#include <QSpinBox>

KoCreatePathTool::KoCreatePathTool(KoCanvasBase *canvas)
    : KoToolBase(*(new KoCreatePathToolPrivate(this, canvas)))
{
}

KoCreatePathTool::~KoCreatePathTool() = default;

void KoCreatePathTool::paint(QPainter &painter, const KoViewConverter &converter)
{
    Q_D(KoCreatePathTool);
    if (pathStarted()) {
        KoShapeStroke *stroke(createStroke());
        if (stroke) {
            d->shape->setStroke(stroke);
        }
        painter.save();
        paintPath(*(d->shape), painter, converter);
        painter.restore();

        painter.save();

        painter.setTransform(d->shape->absoluteTransformation(&converter) * painter.transform());

        KoShape::applyConversion(painter, converter);

        painter.setPen(QPen(Qt::blue, 0));
        painter.setBrush(Qt::white); // TODO make configurable

        const bool firstPoint = (d->firstPoint == d->activePoint);
        if (d->pointIsDragged || firstPoint) {
            const bool onlyPaintActivePoints = false;
            KoPathPoint::PointTypes paintFlags = KoPathPoint::ControlPoint2;
            if (d->activePoint->activeControlPoint1())
                paintFlags |= KoPathPoint::ControlPoint1;
            d->activePoint->paint(painter, d->handleRadius, paintFlags, onlyPaintActivePoints);
        }

        // check if we have to color the first point
        if (d->mouseOverFirstPoint)
            painter.setBrush(Qt::red); // TODO make configurable
        else
            painter.setBrush(Qt::white); // TODO make configurable

        d->firstPoint->paint(painter, d->handleRadius, KoPathPoint::Node);

        painter.restore();
    }

    if (d->hoveredPoint) {
        painter.save();
        painter.setTransform(d->hoveredPoint->parent()->absoluteTransformation(&converter), true);
        KoShape::applyConversion(painter, converter);
        painter.setPen(QPen(Qt::blue, 0));
        painter.setBrush(Qt::white); // TODO make configurable
        d->hoveredPoint->paint(painter, d->handleRadius, KoPathPoint::Node);
        painter.restore();
    }
    painter.save();
    KoShape::applyConversion(painter, converter);
    canvas()->snapGuide()->paint(painter, converter);
    painter.restore();
}

void KoCreatePathTool::paintPath(KoPathShape &pathShape, QPainter &painter, const KoViewConverter &converter)
{
    Q_D(KoCreatePathTool);
    painter.setTransform(pathShape.absoluteTransformation(&converter) * painter.transform());
    painter.save();

    KoShapePaintingContext paintContext; // FIXME
    pathShape.paint(painter, converter, paintContext);
    painter.restore();
    if (pathShape.stroke()) {
        painter.save();
        pathShape.stroke()->paint(d->shape, painter, converter);
        painter.restore();
    }
}

void KoCreatePathTool::mousePressEvent(KoPointerEvent *event)
{
    Q_D(KoCreatePathTool);

    // Right click removes last point
    if (event->button() == Qt::RightButton) {
        removeLastPoint();
        return;
    }

    const bool isOverFirstPoint = d->shape && handleGrabRect(d->firstPoint->point()).contains(event->point);
    bool haveCloseModifier = (listeningToModifiers() && (event->modifiers() & Qt::ShiftModifier));

    if ((event->button() == Qt::LeftButton) && haveCloseModifier && !isOverFirstPoint) {
        endPathWithoutLastPoint();
        return;
    }

    d->finishAfterThisPoint = false;

    if (pathStarted()) {
        if (isOverFirstPoint) {
            d->activePoint->setPoint(d->firstPoint->point());
            canvas()->updateCanvas(d->shape->boundingRect());
            canvas()->updateCanvas(canvas()->snapGuide()->boundingRect());

            if (haveCloseModifier) {
                d->shape->closeMerge();
                // we are closing the path, so reset the existing start path point
                d->existingStartPoint = nullptr;
                // finish path
                endPath();
            } else {
                // the path shape will get closed when the user releases
                // the mouse button
                d->finishAfterThisPoint = true;
            }
        } else {
            canvas()->updateCanvas(canvas()->snapGuide()->boundingRect());

            QPointF point = canvas()->snapGuide()->snap(event->point, event->modifiers());

            // check whether we hit an start/end node of an existing path
            d->existingEndPoint = d->endPointAtPosition(point);
            if (d->existingEndPoint.isValid() && d->existingEndPoint != d->existingStartPoint) {
                point = d->existingEndPoint.path->shapeToDocument(d->existingEndPoint.point->point());
                d->activePoint->setPoint(point);
                // finish path
                endPath();
            } else {
                d->activePoint->setPoint(point);
                canvas()->updateCanvas(d->shape->boundingRect());
                canvas()->updateCanvas(canvas()->snapGuide()->boundingRect());
            }
        }
    } else {
        KoPathShape *pathShape = new KoPathShape();
        d->shape = pathShape;
        pathShape->setShapeId(KoPathShapeId);

        KoShapeStroke *stroke = new KoShapeStroke(canvas()->resourceManager()->activeStroke());
        stroke->setColor(canvas()->resourceManager()->foregroundColor().toQColor());

        pathShape->setStroke(stroke);
        canvas()->updateCanvas(canvas()->snapGuide()->boundingRect());
        QPointF point = canvas()->snapGuide()->snap(event->point, event->modifiers());

        // check whether we hit an start/end node of an existing path
        d->existingStartPoint = d->endPointAtPosition(point);
        if (d->existingStartPoint.isValid()) {
            point = d->existingStartPoint.path->shapeToDocument(d->existingStartPoint.point->point());
        }
        d->activePoint = pathShape->moveTo(point);
        d->firstPoint = d->activePoint;
        canvas()->updateCanvas(handlePaintRect(point));
        canvas()->updateCanvas(canvas()->snapGuide()->boundingRect());

        canvas()->snapGuide()->setEditedShape(pathShape);

        d->angleSnapStrategy = new AngleSnapStrategy(d->angleSnappingDelta, d->angleSnapStatus);
        canvas()->snapGuide()->addCustomSnapStrategy(d->angleSnapStrategy);
    }

    if (d->angleSnapStrategy)
        d->angleSnapStrategy->setStartPoint(d->activePoint->point());
}

bool KoCreatePathTool::listeningToModifiers()
{
    Q_D(KoCreatePathTool);
    return d->listeningToModifiers;
}

bool KoCreatePathTool::pathStarted()
{
    Q_D(KoCreatePathTool);
    return ((bool)d->shape);
}

void KoCreatePathTool::mouseDoubleClickEvent(KoPointerEvent *event)
{
    // remove handle
    canvas()->updateCanvas(handlePaintRect(event->point));

    endPathWithoutLastPoint();
}

void KoCreatePathTool::mouseMoveEvent(KoPointerEvent *event)
{
    Q_D(KoCreatePathTool);

    KoPathPoint *endPoint = d->endPointAtPosition(event->point);
    if (d->hoveredPoint != endPoint) {
        if (d->hoveredPoint) {
            QPointF nodePos = d->hoveredPoint->parent()->shapeToDocument(d->hoveredPoint->point());
            canvas()->updateCanvas(handlePaintRect(nodePos));
        }
        d->hoveredPoint = endPoint;
        if (d->hoveredPoint) {
            QPointF nodePos = d->hoveredPoint->parent()->shapeToDocument(d->hoveredPoint->point());
            canvas()->updateCanvas(handlePaintRect(nodePos));
        }
    }

    if (!pathStarted()) {
        canvas()->updateCanvas(canvas()->snapGuide()->boundingRect());
        canvas()->snapGuide()->snap(event->point, event->modifiers());
        canvas()->updateCanvas(canvas()->snapGuide()->boundingRect());

        d->mouseOverFirstPoint = false;
        return;
    }

    d->mouseOverFirstPoint = handleGrabRect(d->firstPoint->point()).contains(event->point);

    canvas()->updateCanvas(d->shape->boundingRect());
    canvas()->updateCanvas(canvas()->snapGuide()->boundingRect());
    QPointF snappedPosition = canvas()->snapGuide()->snap(event->point, event->modifiers());

    d->repaintActivePoint();
    if (event->buttons() & Qt::LeftButton) {
        d->pointIsDragged = true;
        QPointF offset = snappedPosition - d->activePoint->point();
        d->activePoint->setControlPoint2(d->activePoint->point() + offset);
        // pressing <alt> stops controls points moving symmetrically
        if ((event->modifiers() & Qt::AltModifier) == 0)
            d->activePoint->setControlPoint1(d->activePoint->point() - offset);
        d->repaintActivePoint();
    } else {
        d->activePoint->setPoint(snappedPosition);
    }

    canvas()->updateCanvas(d->shape->boundingRect());
    canvas()->updateCanvas(canvas()->snapGuide()->boundingRect());
}

void KoCreatePathTool::mouseReleaseEvent(KoPointerEvent *event)
{
    Q_D(KoCreatePathTool);

    if (!d->shape || (event->buttons() & Qt::RightButton))
        return;

    d->listeningToModifiers = true; // After the first press-and-release
    d->repaintActivePoint();
    d->pointIsDragged = false;
    KoPathPoint *lastActivePoint = d->activePoint;

    if (!d->finishAfterThisPoint) {
        d->activePoint = d->shape->lineTo(event->point);
        canvas()->snapGuide()->setIgnoredPathPoints((QList<KoPathPoint *>() << d->activePoint));
    }

    // apply symmetric point property if applicable
    if (lastActivePoint->activeControlPoint1() && lastActivePoint->activeControlPoint2()) {
        QPointF diff1 = lastActivePoint->point() - lastActivePoint->controlPoint1();
        QPointF diff2 = lastActivePoint->controlPoint2() - lastActivePoint->point();
        if (qFuzzyCompare(diff1.x(), diff2.x()) && qFuzzyCompare(diff1.y(), diff2.y()))
            lastActivePoint->setProperty(KoPathPoint::IsSymmetric);
    }

    if (d->finishAfterThisPoint) {
        d->firstPoint->setControlPoint1(d->activePoint->controlPoint1());
        delete d->shape->removePoint(d->shape->pathPointIndex(d->activePoint));
        d->activePoint = d->firstPoint;
        d->shape->closeMerge();

        // we are closing the path, so reset the existing start path point
        d->existingStartPoint = nullptr;
        // finish path
        endPath();
    }

    if (d->angleSnapStrategy && lastActivePoint->activeControlPoint2()) {
        d->angleSnapStrategy->deactivate();
    }
}

void KoCreatePathTool::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
        Q_EMIT done();
    else
        event->ignore();
}

void KoCreatePathTool::endPath()
{
    Q_D(KoCreatePathTool);

    d->addPathShape();
}

void KoCreatePathTool::endPathWithoutLastPoint()
{
    Q_D(KoCreatePathTool);

    if (d->shape) {
        QRectF dirtyRect = d->shape->boundingRect();
        delete d->shape->removePoint(d->shape->pathPointIndex(d->activePoint));
        canvas()->updateCanvas(dirtyRect);

        d->addPathShape();
    }
}

void KoCreatePathTool::cancelPath()
{
    Q_D(KoCreatePathTool);

    if (d->shape) {
        canvas()->updateCanvas(handlePaintRect(d->firstPoint->point()));
        canvas()->updateCanvas(d->shape->boundingRect());
        d->firstPoint = nullptr;
        d->activePoint = nullptr;
    }
    d->cleanUp();
}

void KoCreatePathTool::removeLastPoint()
{
    Q_D(KoCreatePathTool);

    if ((d->shape)) {
        KoPathPointIndex lastPointIndex = d->shape->pathPointIndex(d->activePoint);

        if (lastPointIndex.second > 1) {
            lastPointIndex.second--;
            delete d->shape->removePoint(lastPointIndex);

            d->hoveredPoint = nullptr;

            d->repaintActivePoint();
            canvas()->updateCanvas(d->shape->boundingRect());
        }
    }
}

void KoCreatePathTool::activate(ToolActivation, const QSet<KoShape *> &)
{
    Q_D(KoCreatePathTool);
    useCursor(Qt::ArrowCursor);

    // retrieve the actual global handle radius
    d->handleRadius = handleRadius();

    // reset snap guide
    canvas()->updateCanvas(canvas()->snapGuide()->boundingRect());
    canvas()->snapGuide()->reset();
}

void KoCreatePathTool::deactivate()
{
    cancelPath();
}

void KoCreatePathTool::documentResourceChanged(int key, const QVariant &res)
{
    Q_D(KoCreatePathTool);

    switch (key) {
    case KoDocumentResourceManager::HandleRadius: {
        d->handleRadius = res.toUInt();
    } break;
    default:
        return;
    }
}

void KoCreatePathTool::addPathShape(KoPathShape *pathShape)
{
    Q_D(KoCreatePathTool);

    KoPathShape *startShape = nullptr;
    KoPathShape *endShape = nullptr;
    pathShape->normalize();

    // check if existing start/end points are still valid
    d->existingStartPoint.validate(canvas());
    d->existingEndPoint.validate(canvas());

    pathShape->setStroke(createStroke());
    if (d->connectPaths(pathShape, d->existingStartPoint, d->existingEndPoint)) {
        if (d->existingStartPoint.isValid())
            startShape = d->existingStartPoint.path;
        if (d->existingEndPoint.isValid() && d->existingEndPoint != d->existingStartPoint)
            endShape = d->existingEndPoint.path;
    }

    KUndo2Command *cmd = canvas()->shapeController()->addShape(pathShape);
    if (cmd) {
        KoSelection *selection = canvas()->shapeManager()->selection();
        selection->deselectAll();
        selection->select(pathShape);
        if (startShape)
            canvas()->shapeController()->removeShape(startShape, cmd);
        if (endShape && startShape != endShape)
            canvas()->shapeController()->removeShape(endShape, cmd);
        canvas()->addCommand(cmd);
    } else {
        canvas()->updateCanvas(pathShape->boundingRect());
        delete pathShape;
    }
}

QList<QPointer<QWidget>> KoCreatePathTool::createOptionWidgets()
{
    Q_D(KoCreatePathTool);

    QList<QPointer<QWidget>> list;

    QWidget *angleWidget = new QWidget();
    angleWidget->setObjectName("Angle Constraints");
    QGridLayout *layout = new QGridLayout(angleWidget);
    layout->setContentsMargins({});
    layout->addWidget(new QLabel(i18n("Angle snapping delta:"), angleWidget), 0, 0);
    QSpinBox *angleEdit = new QSpinBox(angleWidget);
    angleEdit->setValue(d->angleSnappingDelta);
    angleEdit->setRange(1, 360);
    angleEdit->setSingleStep(1);
    angleEdit->setSuffix(QStringLiteral("°"));
    layout->addWidget(angleEdit, 0, 1);
    layout->addWidget(new QLabel(i18n("Activate angle snap:"), angleWidget), 1, 0);
    QCheckBox *angleSnap = new QCheckBox(angleWidget);
    angleSnap->setChecked(false);
    angleSnap->setCheckable(true);
    layout->addWidget(angleSnap, 1, 1);
    QWidget *specialSpacer = new QWidget();
    specialSpacer->setObjectName("SpecialSpacer");
    layout->addWidget(specialSpacer, 2, 1);
    angleWidget->setWindowTitle(i18n("Angle Constraints"));
    list.append(angleWidget);

    d->strokeWidget = new KoStrokeConfigWidget(nullptr);
    d->strokeWidget->setWindowTitle(i18n("Line"));
    d->strokeWidget->setCanvas(canvas());
    d->strokeWidget->setActive(false);
    list.append(d->strokeWidget);

    connect(angleEdit, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int v) {
        d_func()->angleDeltaChanged(v);
    });
    connect(angleSnap, QOverload<int>::of(&QCheckBox::stateChanged), this, [this](int v) {
        d_func()->angleSnapChanged(v);
    });

    return list;
}

KoShapeStroke *KoCreatePathTool::createStroke()
{
    Q_D(KoCreatePathTool);

    KoShapeStroke *stroke = nullptr;
    if (d->strokeWidget) {
        stroke = d->strokeWidget->createShapeStroke();
    }
    return stroke;
}

// have to include this because of Q_PRIVATE_SLOT
#include <moc_KoCreatePathTool.cpp>
