/* This file is part of the KDE project
 *
 * Copyright (C) 2009 Thorsten Zachmann <zachmann@kde.org>
 * Copyright (C) 2009 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 * Copyright (C) 2011 Jan Hambrecht <jaham@gmx.net>
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

#include "ConnectionTool.h"
#include "AddConnectionPointCommand.h"
#include "RemoveConnectionPointCommand.h"
#include "ChangeConnectionPointCommand.h"
#include "MoveConnectionPointStrategy.h"
#include "ConnectionToolWidget.h"
#include "ConnectionPointWidget.h"

#include <KoCanvasBase.h>
#include <KoPointerEvent.h>
#include <KoShapeManager.h>
#include <KoShapeFactoryBase.h>
#include <KoShape.h>
#include <KoShapeController.h>
#include <KoShapeLayer.h>
#include <KoShapeRegistry.h>
#include <KoSelection.h>
#include <KoLineBorder.h>
#include <KoResourceManager.h>
#include <KoInteractionStrategy.h>
#include <KAction>
#include <KLocale>
#include <KDebug>
#include <QUndoCommand>
#include <QPointF>
#include <QKeyEvent>

ConnectionTool::ConnectionTool(KoCanvasBase * canvas)
    : KoPathTool(canvas)
    , m_editMode(Idle)
    , m_currentShape(0)
    , m_activeHandle(-1)
    , m_currentStrategy(0)
    , m_oldSnapStrategies(0)
{
    m_alignPercent = new KAction(QString("%"), this);
    m_alignPercent->setCheckable(true);
    addAction("align-relative", m_alignPercent);
    m_alignLeft = new KAction(KIcon("align-horizontal-left"), i18n("Align to left edge"), this);
    m_alignLeft->setCheckable(true);
    addAction("align-left", m_alignLeft);
    m_alignCenterH = new KAction(KIcon("align-horizontal-center"), i18n("Align to horizontal center"), this);
    m_alignCenterH->setCheckable(true);
    addAction("align-centerh", m_alignCenterH);
    m_alignRight = new KAction(KIcon("align-horizontal-right"), i18n("Align to right edge"), this);
    m_alignRight->setCheckable(true);
    addAction("align-right", m_alignRight);
    m_alignTop = new KAction(KIcon("align-vertical-top"), i18n("Align to top edge"), this);
    m_alignTop->setCheckable(true);
    addAction("align-top", m_alignTop);
    m_alignCenterV = new KAction(KIcon("align-vertical-center"), i18n("Align to vertical center"), this);
    m_alignCenterV->setCheckable(true);
    addAction("align-centerv", m_alignCenterV);
    m_alignBottom = new KAction(KIcon("align-vertical-bottom"), i18n("Align to bottom edge"), this);
    m_alignBottom->setCheckable(true);
    addAction("align-bottom", m_alignBottom);

    m_escapeAll = new KAction(KIcon("escape-direction-all"), i18n("Escape in all directions"), this);
    m_escapeAll->setCheckable(true);
    addAction("escape-all", m_escapeAll);
    m_escapeHorizontal = new KAction(KIcon("escape-direction-horizontal"), i18n("Escape in horizonal directions"), this);
    m_escapeHorizontal->setCheckable(true);
    addAction("escape-horizontal", m_escapeHorizontal);
    m_escapeVertical = new KAction(KIcon("escape-direction-vertical"), i18n("Escape in vertical directions"), this);
    m_escapeVertical->setCheckable(true);
    addAction("escape-vertical", m_escapeVertical);
    m_escapeLeft = new KAction(KIcon("escape-direction-left"), i18n("Escape in left direction"), this);
    m_escapeLeft->setCheckable(true);
    addAction("escape-left", m_escapeLeft);
    m_escapeRight = new KAction(KIcon("escape-direction-right"), i18n("Escape in right direction"), this);
    m_escapeRight->setCheckable(true);
    addAction("escape-right", m_escapeRight);
    m_escapeUp = new KAction(KIcon("escape-direction-up"), i18n("Escape in up direction"), this);
    m_escapeUp->setCheckable(true);
    addAction("escape-up", m_escapeUp);
    m_escapeDown = new KAction(KIcon("escape-direction-down"), i18n("Escape in down direction"), this);
    m_escapeDown->setCheckable(true);
    addAction("escape-down", m_escapeDown);

    m_alignHorizontal = new QActionGroup(this);
    m_alignHorizontal->setExclusive(true);
    m_alignHorizontal->addAction(m_alignLeft);
    m_alignHorizontal->addAction(m_alignCenterH);
    m_alignHorizontal->addAction(m_alignRight);
    connect(m_alignHorizontal, SIGNAL(triggered(QAction*)), this, SLOT(horizontalAlignChanged()));

    m_alignVertical = new QActionGroup(this);
    m_alignVertical->setExclusive(true);
    m_alignVertical->addAction(m_alignTop);
    m_alignVertical->addAction(m_alignCenterV);
    m_alignVertical->addAction(m_alignBottom);
    connect(m_alignVertical, SIGNAL(triggered(QAction*)), this, SLOT(verticalAlignChanged()));

    m_alignRelative = new QActionGroup(this);
    m_alignRelative->setExclusive(true);
    m_alignRelative->addAction(m_alignPercent);
    connect(m_alignRelative, SIGNAL(triggered(QAction*)), this, SLOT(relativeAlignChanged()));

    m_escapeDirections = new QActionGroup(this);
    m_escapeDirections->setExclusive(true);
    m_escapeDirections->addAction(m_escapeAll);
    m_escapeDirections->addAction(m_escapeHorizontal);
    m_escapeDirections->addAction(m_escapeVertical);
    m_escapeDirections->addAction(m_escapeLeft);
    m_escapeDirections->addAction(m_escapeRight);
    m_escapeDirections->addAction(m_escapeUp);
    m_escapeDirections->addAction(m_escapeDown);
    connect(m_escapeDirections, SIGNAL(triggered(QAction*)), this, SLOT(escapeDirectionChanged()));

    connect(this, SIGNAL(connectionPointEnabled(bool)), m_alignHorizontal, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(connectionPointEnabled(bool)), m_alignVertical, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(connectionPointEnabled(bool)), m_alignRelative, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(connectionPointEnabled(bool)), m_escapeDirections, SLOT(setEnabled(bool)));

    setEditMode(Idle, 0, -1);
}

ConnectionTool::~ConnectionTool()
{
}

void ConnectionTool::paint(QPainter &painter, const KoViewConverter &converter)
{
    // get the correctly sized rect for painting handles
    QRectF handleRect = handlePaintRect(QPointF());

    painter.setRenderHint(QPainter::Antialiasing, true);

    if (m_currentStrategy) {
        painter.save();
        m_currentStrategy->paint(painter, converter);
        painter.restore();
    }

    if(m_currentShape) {
        // paint connection points or connection handles depending
        // on the shape the mouse is currently
        KoConnectionShape *connectionShape = dynamic_cast<KoConnectionShape*>(m_currentShape);
        if(connectionShape) {
            int radius = canvas()->resourceManager()->handleRadius();
            int handleCount = connectionShape->handleCount();
            for(int i = 0; i < handleCount; ++i) {
                painter.save();
                painter.setPen(Qt::blue);
                Qt::GlobalColor fillColor = Qt::white;
                if(m_editMode == EditConnection) {
                    fillColor = i == m_activeHandle ? Qt::red : Qt::darkGreen;
                } else {
                    fillColor = i == m_activeHandle ? Qt::red : Qt::white;
                }
                painter.setBrush(fillColor);
                //painter.setBrush(i == m_activeHandle ? Qt::red : Qt::white);
                painter.setTransform(connectionShape->absoluteTransformation(&converter) * painter.transform());
                connectionShape->paintHandle(painter, converter, i, radius);
                painter.restore();
            }
        } else {
            painter.save();
            painter.setPen(Qt::black);
            QTransform transform = m_currentShape->absoluteTransformation(0);
            KoShape::applyConversion(painter, converter);
            // Draw all the connection points of the shape
            KoConnectionPoints connectionPoints = m_currentShape->connectionPoints();
            KoConnectionPoints::const_iterator cp = connectionPoints.constBegin();
            KoConnectionPoints::const_iterator lastCp = connectionPoints.constEnd();
            for(; cp != lastCp; ++cp) {
                handleRect.moveCenter(transform.map(cp.value().position));
                Qt::GlobalColor fillColor = Qt::white;
                if(m_editMode == EditConnectionPoint) {
                    fillColor = cp.key() == m_activeHandle ? Qt::red : Qt::darkGreen;
                } else if (m_editMode == CreateConnection) {
                    fillColor = cp.key() == m_activeHandle ? Qt::red : Qt::white;
                }
                painter.setBrush(fillColor);
                painter.drawRect(handleRect);
            }
            painter.restore();
        }
    }
}

void ConnectionTool::repaintDecorations()
{
    if(m_currentShape) {
        repaint(m_currentShape->boundingRect());
        KoConnectionShape * connectionShape = dynamic_cast<KoConnectionShape*>(m_currentShape);
        if(connectionShape) {
            QPointF handlePos = connectionShape->handlePosition(m_activeHandle);
            handlePos = connectionShape->shapeToDocument(handlePos);
            repaint(handlePaintRect(handlePos));
        } else {
            KoConnectionPoints connectionPoints = m_currentShape->connectionPoints();
            KoConnectionPoints::const_iterator cp = connectionPoints.constBegin();
            KoConnectionPoints::const_iterator lastCp = connectionPoints.constEnd();
            for(; cp != lastCp; ++cp) {
                repaint(handleGrabRect(m_currentShape->shapeToDocument(cp.value().position)));
            }
        }
    }
}

void ConnectionTool::mousePressEvent(KoPointerEvent * event)
{
    KoShape * hitShape = findShapeAtPosition(event->point);
    int hitHandle = handleAtPoint(m_currentShape, event->point);

    if(m_editMode == EditConnection && hitHandle >= 0) {
        // create connection handle change strategy
        m_currentStrategy = createStrategy(dynamic_cast<KoConnectionShape*>(m_currentShape), hitHandle);
    } else if (m_editMode == EditConnectionPoint && hitHandle >= KoConnectionPoint::FirstCustomConnectionPoint) {
        // start moving custom connection point
        m_currentStrategy = new MoveConnectionPointStrategy(m_currentShape, hitHandle, this);
    } else if(m_editMode == CreateConnection) {
        // create new connection shape, connect it to the active connection point
        // and start editing the new connection
        // create the new connection shape
        KoShapeFactoryBase *factory = KoShapeRegistry::instance()->value("KoConnectionShape");
        KoShape *shape = factory->createDefaultShape(canvas()->shapeController()->resourceManager());
        KoConnectionShape * connectionShape = dynamic_cast<KoConnectionShape*>(shape);
        if(!connectionShape) {
            delete shape;
            resetEditMode();
            return;
        }
        // get the position of the connection point we start our connection from
        QPointF cp = m_currentShape->shapeToDocument(m_currentShape->connectionPoint(m_activeHandle).position);
        // move both handles to that point
        connectionShape->moveHandle(0, cp);
        connectionShape->moveHandle(1, cp);
        // connect the first handle of the connection shape to our connection point
        if(!connectionShape->connectFirst(m_currentShape, m_activeHandle)) {
            delete shape;
            resetEditMode();
            return;
        }
        // create the connection edit strategy from the path tool
        m_currentStrategy = createStrategy(connectionShape, 1);
        if (!m_currentStrategy) {
            delete shape;
            resetEditMode();
            return;
        }
        // update our handle data
        setEditMode(m_editMode, shape, 1);
        // add connection shape to the shape manager so it gets painted
        canvas()->shapeManager()->addShape(connectionShape);
    } else {
        // pressing on a shape in idle mode switches to corresponding edit mode
        if (hitShape) {
            if (dynamic_cast<KoConnectionShape*>(hitShape)) {
                int hitHandle = handleAtPoint(hitShape, event->point);
                setEditMode(EditConnection, hitShape, hitHandle);
                if(hitHandle >= 0) {
                    // start editing connection shape
                    m_currentStrategy = createStrategy(dynamic_cast<KoConnectionShape*>(m_currentShape), m_activeHandle);
                }
            } else {
                setEditMode(EditConnectionPoint, hitShape, -1);
            }
        } else {
            resetEditMode();
        }
    }
}

void ConnectionTool::mouseMoveEvent(KoPointerEvent *event)
{
    if (m_currentStrategy) {
        repaintDecorations();
        if (m_editMode != EditConnection && m_editMode != CreateConnection) {
            QPointF snappedPos = canvas()->snapGuide()->snap(event->point, event->modifiers());
            m_currentStrategy->handleMouseMove(snappedPos, event->modifiers());
        } else {
            m_currentStrategy->handleMouseMove(event->point, event->modifiers());
        }
        repaintDecorations();
    } else if (m_editMode == EditConnectionPoint) {
        Q_ASSERT(m_currentShape);
        // check if we should highlight another connection point
        int handle = handleAtPoint(m_currentShape, event->point);
        if (handle >= 0)
            setEditMode(m_editMode, m_currentShape, handle);
        else
            updateStatusText();
    } else if (m_editMode == EditConnection) {
        Q_ASSERT(m_currentShape);
        // check if we should highlight another connection handle
        int handle = handleAtPoint(m_currentShape, event->point);
        setEditMode(m_editMode, m_currentShape, handle);
    } else {
        KoShape *hoverShape = findShapeAtPosition(event->point);
        int hoverHandle = -1;
        if (hoverShape)
            hoverHandle = handleAtPoint(hoverShape, event->point);
        setEditMode(Idle, hoverShape, hoverHandle);
    }
}

void ConnectionTool::mouseReleaseEvent(KoPointerEvent *event)
{
    if (m_currentStrategy) {
        if (m_editMode == CreateConnection) {
            // check if connection handles have a minimal distance
            KoConnectionShape * connectionShape = dynamic_cast<KoConnectionShape*>(m_currentShape);
            Q_ASSERT(connectionShape);
            // get both handle positions in document coordinates
            QPointF p1 = connectionShape->shapeToDocument(connectionShape->handlePosition(0));
            QPointF p2 = connectionShape->shapeToDocument(connectionShape->handlePosition(1));
            int grabSensitivity = canvas()->resourceManager()->grabSensitivity();
            // use grabbing sensitivity as minimal distance threshold
            if (squareDistance(p1, p2) < grabSensitivity*grabSensitivity) {
                // minimal distance was not reached, so we have to undo the started work:
                // - cleanup and delete the strategy
                // - remove connection shape from shape manager and delete it
                // - reset edit mode to last state
                delete m_currentStrategy;
                m_currentStrategy = 0;
                repaintDecorations();
                canvas()->shapeManager()->remove(m_currentShape);
                setEditMode(m_editMode, connectionShape->firstShape(), connectionShape->firstConnectionId());
                repaintDecorations();
                delete connectionShape;
                return;
            } else {
                // finalize adding the new connection shape with an undo command
                QUndoCommand * cmd = canvas()->shapeController()->addShape(m_currentShape);
                canvas()->addCommand(cmd);
            }
            resetEditMode();
        }
        m_currentStrategy->finishInteraction(event->modifiers());
        // TODO: Add parent command to KoInteractionStrategy::createCommand
        // so that we can have a single command to undo for the user
        QUndoCommand *command = m_currentStrategy->createCommand();
        if (command)
            canvas()->addCommand(command);
        delete m_currentStrategy;
        m_currentStrategy = 0;
    }
    updateStatusText();
}

void ConnectionTool::mouseDoubleClickEvent(KoPointerEvent *event)
{
    if (!m_currentShape)
        return;

    if (m_editMode == EditConnectionPoint) {
        repaintDecorations();
        int handleId = handleAtPoint(m_currentShape, event->point);
        if (handleId < 0) {
            QPointF mousePos = canvas()->snapGuide()->snap(event->point, event->modifiers());
            QPointF point = m_currentShape->documentToShape(mousePos);
            canvas()->addCommand(new AddConnectionPointCommand(m_currentShape, point));
        } else {
            canvas()->addCommand(new RemoveConnectionPointCommand(m_currentShape, handleId));
        }
        setEditMode(m_editMode, m_currentShape, -1);
    } else if (m_editMode == Idle || m_editMode == EditConnection) {
        if (dynamic_cast<KoConnectionShape*>(m_currentShape)) {
            repaintDecorations();
            QUndoCommand * cmd = canvas()->shapeController()->removeShape(m_currentShape);
            canvas()->addCommand(cmd);
            resetEditMode();
        }
    }
}

void ConnectionTool::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        deactivate();
    }
}

void ConnectionTool::activate(ToolActivation, const QSet<KoShape*> &)
{
    canvas()->canvasWidget()->setCursor(Qt::PointingHandCursor);
    // save old enabled snap strategies, set bounding box snap strategy
    m_oldSnapStrategies = canvas()->snapGuide()->enabledSnapStrategies();
    canvas()->snapGuide()->enableSnapStrategies(KoSnapGuide::BoundingBoxSnapping);
    canvas()->snapGuide()->reset();
}

void ConnectionTool::deactivate()
{
    // Put everything to 0 to be able to begin a new shape properly
    delete m_currentStrategy;
    m_currentStrategy = 0;
    resetEditMode();
    // restore previously set snap strategies
    canvas()->snapGuide()->enableSnapStrategies(m_oldSnapStrategies);
    canvas()->snapGuide()->reset();
}

qreal ConnectionTool::squareDistance(const QPointF &p1, const QPointF &p2)
{
    // Square of the distance
    const qreal dx = p2.x() - p1.x();
    const qreal dy = p2.y() - p1.y();
    return dx*dx + dy*dy;
}

KoShape * ConnectionTool::findShapeAtPosition(const QPointF &position)
{
    QList<KoShape*> shapes = canvas()->shapeManager()->shapesAt(handleGrabRect(position));
    if(!shapes.isEmpty()) {
        qSort(shapes.begin(), shapes.end(), KoShape::compareShapeZIndex);
        // we want to priorize connection shape handles, even if the connection shape
        // is not at the top of the shape stack at the mouse position
        KoConnectionShape *connectionShape = nearestConnectionShape(shapes, position);
        // use best connection shape or first shape from stack if not found
        return connectionShape ? connectionShape : shapes.first();
    }

    return 0;
}

int ConnectionTool::handleAtPoint(KoShape *shape, const QPointF &mousePoint)
{
    if(!shape)
        return -1;

    const QPointF shapePoint = shape->documentToShape(mousePoint);

    KoConnectionShape * connectionShape = dynamic_cast<KoConnectionShape*>(shape);
    if(connectionShape) {
        // check connection shape handles
        return connectionShape->handleIdAt(handleGrabRect(shapePoint));
    } else {
        // check connection points
        int grabSensitivity = canvas()->resourceManager()->grabSensitivity();
        qreal minDistance = HUGE_VAL;
        int handleId = -1;
        KoConnectionPoints connectionPoints = shape->connectionPoints();
        KoConnectionPoints::const_iterator cp = connectionPoints.constBegin();
        KoConnectionPoints::const_iterator lastCp = connectionPoints.constEnd();
        for(; cp != lastCp; ++cp) {
            qreal d = squareDistance(shapePoint, cp.value().position);
            if (d <= grabSensitivity && d < minDistance) {
                handleId = cp.key();
                minDistance = d;
            }
        }
        return handleId;
    }
}

KoConnectionShape * ConnectionTool::nearestConnectionShape(QList<KoShape*> shapes, const QPointF &mousePos)
{
    int grabSensitivity = canvas()->resourceManager()->grabSensitivity();

    KoConnectionShape * nearestConnectionShape = 0;
    qreal minSquaredDistance = HUGE_VAL;
    const qreal maxSquaredDistance = grabSensitivity*grabSensitivity;

    foreach(KoShape *shape, shapes) {
        KoConnectionShape * connectionShape = dynamic_cast<KoConnectionShape*>(shape);
        if (!connectionShape || !connectionShape->isParametricShape())
            continue;

        // convert document point to shape coordinates
        QPointF p = connectionShape->documentToShape(mousePos);
        // our region of interest, i.e. a region around our mouse position
        QRectF roi = handleGrabRect(p);

        // check all segments of this shape which intersect the region of interest
        QList<KoPathSegment> segments = connectionShape->segmentsAt(roi);
        foreach (const KoPathSegment &s, segments) {
            qreal nearestPointParam = s.nearestPoint(p);
            QPointF nearestPoint = s.pointAt(nearestPointParam);
            QPointF diff = p - nearestPoint;
            qreal squaredDistance = diff.x()*diff.x() + diff.y()*diff.y();
            // are we within the allowed distance ?
            if (squaredDistance > maxSquaredDistance)
                continue;
            // are we closer to the last closest point ?
            if (squaredDistance < minSquaredDistance) {
                nearestConnectionShape = connectionShape;
                minSquaredDistance = squaredDistance;
            }
        }
    }

    return nearestConnectionShape;
}

void ConnectionTool::setEditMode(EditMode mode, KoShape *currentShape, int handle)
{
    repaintDecorations();
    m_editMode = mode;
    m_currentShape = currentShape;
    m_activeHandle = handle;
    repaintDecorations();
    updateActions();
    updateStatusText();
}

void ConnectionTool::resetEditMode()
{
    setEditMode(Idle, 0, -1);
}

void ConnectionTool::updateActions()
{
    const bool connectionPointSelected = m_editMode == EditConnectionPoint && m_activeHandle >= 0;
    if (connectionPointSelected) {
        KoConnectionPoint cp = m_currentShape->connectionPoint(m_activeHandle);

        m_alignPercent->setChecked(false);
        foreach(QAction *action, m_alignHorizontal->actions())
            action->setChecked(false);
        foreach(QAction *action, m_alignVertical->actions())
            action->setChecked(false);
        switch(cp.align) {
            case KoConnectionPoint::AlignNone:
                m_alignPercent->setChecked(true);
                break;
            case KoConnectionPoint::AlignTopLeft:
                m_alignLeft->setChecked(true);
                m_alignTop->setChecked(true);
                break;
            case KoConnectionPoint::AlignTop:
                m_alignCenterH->setChecked(true);
                m_alignTop->setChecked(true);
                break;
            case KoConnectionPoint::AlignTopRight:
                m_alignRight->setChecked(true);
                m_alignTop->setChecked(true);
                break;
            case KoConnectionPoint::AlignLeft:
                m_alignLeft->setChecked(true);
                m_alignCenterV->setChecked(true);
                break;
            case KoConnectionPoint::AlignCenter:
                m_alignCenterH->setChecked(true);
                m_alignCenterV->setChecked(true);
                break;
            case KoConnectionPoint::AlignRight:
                m_alignRight->setChecked(true);
                m_alignCenterV->setChecked(true);
                break;
            case KoConnectionPoint::AlignBottomLeft:
                m_alignLeft->setChecked(true);
                m_alignBottom->setChecked(true);
                break;
            case KoConnectionPoint::AlignBottom:
                m_alignCenterH->setChecked(true);
                m_alignBottom->setChecked(true);
                break;
            case KoConnectionPoint::AlignBottomRight:
                m_alignRight->setChecked(true);
                m_alignBottom->setChecked(true);
                break;
        }
        foreach(QAction *action, m_escapeDirections->actions())
            action->setChecked(false);
        switch(cp.escapeDirection) {
            case KoConnectionPoint::AllDirections:
                m_escapeAll->setChecked(true);
                break;
            case KoConnectionPoint::HorizontalDirections:
                m_escapeHorizontal->setChecked(true);
                break;
            case KoConnectionPoint::VerticalDirections:
                m_escapeVertical->setChecked(true);
                break;
            case KoConnectionPoint::LeftDirection:
                m_escapeLeft->setChecked(true);
                break;
            case KoConnectionPoint::RightDirection:
                m_escapeRight->setChecked(true);
                break;
            case KoConnectionPoint::UpDirection:
                m_escapeUp->setChecked(true);
                break;
            case KoConnectionPoint::DownDirection:
                m_escapeDown->setChecked(true);
                break;
        }
    }
    emit connectionPointEnabled(connectionPointSelected);
}

void ConnectionTool::updateStatusText()
{
    switch(m_editMode) {
        case Idle:
            if(m_currentShape) {
                if (dynamic_cast<KoConnectionShape*>(m_currentShape)) {
                    if (m_activeHandle >= 0)
                        emit statusTextChanged(i18n("Drag to edit connection."));
                    else
                        emit statusTextChanged(i18n("Double click connection to remove it."));
                } else if (m_activeHandle < 0) {
                    emit statusTextChanged(i18n("Click to edit connection points."));
                }
            } else {
                emit statusTextChanged("");
            }
            break;
        case EditConnection:
            if (m_activeHandle >= 0)
                emit statusTextChanged(i18n("Drag to edit connection."));
            else
                emit statusTextChanged(i18n("Double click connection to remove it."));
            break;
        case EditConnectionPoint:
            if (m_activeHandle >= KoConnectionPoint::FirstCustomConnectionPoint)
                emit statusTextChanged(i18n("Drag to move connection point. Double click to remove connection point"));
            else if (m_activeHandle >= 0)
                emit statusTextChanged(i18n("Double click to remove connection point"));
            else
                emit statusTextChanged(i18n("Double click to add connection point."));
            break;
        case CreateConnection:
            emit statusTextChanged(i18n("Drag to create new connection."));
            break;
        default:
            emit statusTextChanged("");
    }
}

QMap<QString, QWidget *> ConnectionTool::createOptionWidgets()
{
    QMap<QString, QWidget *> map;

    ConnectionToolWidget *tw = new ConnectionToolWidget();
    ConnectionPointWidget *pw = new ConnectionPointWidget(this);

    map.insert(i18n("Connection"), tw);
    map.insert(i18n("Connection Point"), pw);

    return map;
}

void ConnectionTool::horizontalAlignChanged()
{
    if (m_alignPercent->isChecked()) {
        m_alignPercent->setChecked(false);
        m_alignCenterV->setChecked(true);
    }
    updateConnectionPoint();
}

void ConnectionTool::verticalAlignChanged()
{
    if (m_alignPercent->isChecked()) {
        m_alignPercent->setChecked(false);
        m_alignCenterH->setChecked(true);
    }
    updateConnectionPoint();
}

void ConnectionTool::relativeAlignChanged()
{
    foreach(QAction *action, m_alignHorizontal->actions())
        action->setChecked(false);
    foreach(QAction *action, m_alignVertical->actions())
        action->setChecked(false);
    m_alignPercent->setChecked(true);

    updateConnectionPoint();
}

void ConnectionTool::updateConnectionPoint()
{
    if (m_editMode == EditConnectionPoint && m_currentShape && m_activeHandle >= 0) {
        KoConnectionPoint oldPoint = m_currentShape->connectionPoint(m_activeHandle);
        KoConnectionPoint newPoint = oldPoint;
        if (m_alignPercent->isChecked()) {
            newPoint.align = KoConnectionPoint::AlignNone;
        } else if (m_alignLeft->isChecked() && m_alignTop->isChecked()) {
            newPoint.align = KoConnectionPoint::AlignTopLeft;
        } else if(m_alignCenterH->isChecked() && m_alignTop->isChecked()) {
            newPoint.align = KoConnectionPoint::AlignTop;
        } else if (m_alignRight->isChecked() && m_alignTop->isChecked()) {
            newPoint.align = KoConnectionPoint::AlignTopRight;
        } else if (m_alignLeft->isChecked() && m_alignCenterV->isChecked()) {
            newPoint.align = KoConnectionPoint::AlignLeft;
        } else if (m_alignCenterH->isChecked() && m_alignCenterV->isChecked()) {
            newPoint.align = KoConnectionPoint::AlignCenter;
        } else if (m_alignRight->isChecked() && m_alignCenterV->isChecked()) {
            newPoint.align = KoConnectionPoint::AlignRight;
        } else if (m_alignLeft->isChecked() && m_alignBottom->isChecked()) {
            newPoint.align = KoConnectionPoint::AlignBottomLeft;
        } else if (m_alignCenterH->isChecked() && m_alignBottom->isChecked()) {
            newPoint.align = KoConnectionPoint::AlignBottom;
        } else if (m_alignRight->isChecked() && m_alignBottom->isChecked()) {
            newPoint.align = KoConnectionPoint::AlignBottomRight;
        }

        canvas()->addCommand(new ChangeConnectionPointCommand(m_currentShape, m_activeHandle, oldPoint, newPoint));
    }
}

void ConnectionTool::escapeDirectionChanged()
{
    if (m_editMode == EditConnectionPoint && m_currentShape && m_activeHandle >= 0) {
        KoConnectionPoint oldPoint = m_currentShape->connectionPoint(m_activeHandle);
        KoConnectionPoint newPoint = oldPoint;
        QAction * checkedAction = m_escapeDirections->checkedAction();
        if (checkedAction == m_escapeAll) {
            newPoint.escapeDirection = KoConnectionPoint::AllDirections;
        } else if (checkedAction == m_escapeHorizontal) {
            newPoint.escapeDirection = KoConnectionPoint::HorizontalDirections;
        } else if (checkedAction == m_escapeVertical) {
            newPoint.escapeDirection = KoConnectionPoint::VerticalDirections;
        } else if (checkedAction == m_escapeLeft) {
            newPoint.escapeDirection = KoConnectionPoint::LeftDirection;
        } else if (checkedAction == m_escapeRight) {
            newPoint.escapeDirection = KoConnectionPoint::RightDirection;
        } else if (checkedAction == m_escapeUp) {
            newPoint.escapeDirection = KoConnectionPoint::UpDirection;
        } else if (checkedAction == m_escapeDown) {
            newPoint.escapeDirection = KoConnectionPoint::DownDirection;
        }
        canvas()->addCommand(new ChangeConnectionPointCommand(m_currentShape, m_activeHandle, oldPoint, newPoint));
    }
}
