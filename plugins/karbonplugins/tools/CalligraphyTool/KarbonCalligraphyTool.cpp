/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Fela Winkelmolen <fela.kde@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KarbonCalligraphyTool.h"
#include "KarbonCalligraphicShape.h"
#include "KarbonCalligraphyOptionWidget.h"

#include <KoCanvasBase.h>
#include <KoCanvasResourceManager.h>
#include <KoColor.h>
#include <KoColorBackground.h>
#include <KoCurveFit.h>
#include <KoFillConfigWidget.h>
#include <KoPathPoint.h>
#include <KoPathShape.h>
#include <KoPointerEvent.h>
#include <KoSelection.h>
#include <KoShapeController.h>
#include <KoShapeGroup.h>
#include <KoShapeManager.h>
#include <KoShapePaintingContext.h>

#include <KLocalizedString>
#include <QAction>
#include <QDebug>
#include <QPainter>

#include <cmath>

#undef M_PI
const qreal M_PI = 3.1415927;
using std::pow;
using std::sqrt;

KarbonCalligraphyTool::KarbonCalligraphyTool(KoCanvasBase *canvas)
    : KoToolBase(canvas)
    , m_shape(nullptr)
    , m_angle(0)
    , m_selectedPath(nullptr)
    , m_isDrawing(false)
    , m_speed(0, 0)
    , m_lastShape(nullptr)
{
    connect(canvas->shapeManager(), &KoShapeManager::selectionChanged, this, &KarbonCalligraphyTool::updateSelectedPath);

    updateSelectedPath();
}

KarbonCalligraphyTool::~KarbonCalligraphyTool() = default;

void KarbonCalligraphyTool::paint(QPainter &painter, const KoViewConverter &converter)
{
    if (m_selectedPath) {
        painter.save();
        painter.setRenderHints(QPainter::Antialiasing, false);
        painter.setPen(QPen(Qt::red, 0)); // TODO make configurable
        QRectF rect = m_selectedPath->boundingRect();
        QPointF p1 = converter.documentToView(rect.topLeft());
        QPointF p2 = converter.documentToView(rect.bottomRight());
        painter.drawRect(QRectF(p1, p2));
        painter.restore();
    }

    if (!m_shape)
        return;

    painter.save();

    painter.setTransform(m_shape->absoluteTransformation(&converter) * painter.transform());
    KoShapePaintingContext paintContext; // FIXME
    m_shape->paint(painter, converter, paintContext);

    painter.restore();
}

void KarbonCalligraphyTool::mousePressEvent(KoPointerEvent *event)
{
    if (m_isDrawing)
        return;

    m_lastPoint = event->point;
    m_speed = QPointF(0, 0);

    m_isDrawing = true;
    m_pointCount = 0;
    m_shape = new KarbonCalligraphicShape(m_caps);
    m_shape->setBackground(QSharedPointer<KoShapeBackground>(new KoColorBackground(canvas()->resourceManager()->foregroundColor().toQColor())));
    // addPoint( event );
}

void KarbonCalligraphyTool::mouseMoveEvent(KoPointerEvent *event)
{
    if (!m_isDrawing)
        return;

    addPoint(event);
}

void KarbonCalligraphyTool::mouseReleaseEvent(KoPointerEvent *event)
{
    if (!m_isDrawing)
        return;

    if (m_pointCount == 0) {
        // handle click: select shape (if any)
        if (event->point == m_lastPoint) {
            KoShapeManager *shapeManager = canvas()->shapeManager();
            KoShape *selectedShape = shapeManager->shapeAt(event->point);
            if (selectedShape != nullptr) {
                shapeManager->selection()->deselectAll();
                shapeManager->selection()->select(selectedShape);
            }
        }

        delete m_shape;
        m_shape = nullptr;
        m_isDrawing = false;
        return;
    } else {
        m_endOfPath = false; // allow last point being added
        addPoint(event); // add last point
        m_isDrawing = false;
    }

    m_shape->simplifyGuidePath();

    KUndo2Command *cmd = canvas()->shapeController()->addShape(m_shape);
    if (cmd) {
        m_lastShape = m_shape;
        canvas()->addCommand(cmd);
        canvas()->updateCanvas(m_shape->boundingRect());
    } else {
        // don't leak shape when command could not be created
        delete m_shape;
    }

    m_shape = nullptr;
}

void KarbonCalligraphyTool::addPoint(KoPointerEvent *event)
{
    if (m_pointCount == 0) {
        if (m_usePath && m_selectedPath)
            m_selectedPathOutline = m_selectedPath->outline();
        m_pointCount = 1;
        m_endOfPath = false;
        m_followPathPosition = 0;
        m_lastMousePos = event->point;
        m_lastPoint = calculateNewPoint(event->point, &m_speed);
        m_deviceSupportsTilt = (event->xTilt() != 0 || event->yTilt() != 0);
        return;
    }

    if (m_endOfPath)
        return;

    ++m_pointCount;

    setAngle(event);

    QPointF newSpeed;
    QPointF newPoint = calculateNewPoint(event->point, &newSpeed);
    qreal width = calculateWidth(event->pressure());
    qreal angle = calculateAngle(m_speed, newSpeed);

    // add the previous point
    m_shape->appendPoint(m_lastPoint, angle, width);

    m_speed = newSpeed;
    m_lastPoint = newPoint;
    canvas()->updateCanvas(m_shape->lastPieceBoundingRect());

    if (m_usePath && m_selectedPath)
        m_speed = QPointF(0, 0); // following path
}

void KarbonCalligraphyTool::setAngle(KoPointerEvent *event)
{
    if (!m_useAngle) {
        m_angle = (360 - m_customAngle + 90) / 180.0 * M_PI;
        return;
    }

    // setting m_angle to the angle of the device
    if (event->xTilt() != 0 || event->yTilt() != 0)
        m_deviceSupportsTilt = false;

    if (m_deviceSupportsTilt) {
        if (event->xTilt() == 0 && event->yTilt() == 0)
            return; // leave as is
        qDebug() << "using tilt" << m_angle;

        if (event->x() == 0) {
            m_angle = M_PI / 2;
            return;
        }

        // y is inverted in qt painting
        m_angle = std::atan(static_cast<double>(-event->yTilt() / event->xTilt())) + M_PI / 2;
    } else {
        m_angle = event->rotation() + M_PI / 2;
        qDebug() << "using rotation" << m_angle;
    }
}

QPointF KarbonCalligraphyTool::calculateNewPoint(const QPointF &mousePos, QPointF *speed)
{
    if (!m_usePath || !m_selectedPath) { // don't follow path
        QPointF force = mousePos - m_lastPoint;
        QPointF dSpeed = force / m_mass;
        *speed = m_speed * (1.0 - m_drag) + dSpeed;
        return m_lastPoint + *speed;
    }

    QPointF sp = mousePos - m_lastMousePos;
    m_lastMousePos = mousePos;

    // follow selected path
    qreal step = QLineF(QPointF(0, 0), sp).length();
    m_followPathPosition += step;

    qreal t;
    if (m_followPathPosition >= m_selectedPathOutline.length()) {
        t = 1.0;
        m_endOfPath = true;
    } else {
        t = m_selectedPathOutline.percentAtLength(m_followPathPosition);
    }

    QPointF res = m_selectedPathOutline.pointAtPercent(t) + m_selectedPath->position();
    *speed = res - m_lastPoint;
    return res;
}

qreal KarbonCalligraphyTool::calculateWidth(qreal pressure)
{
    // calculate the modulo of the speed
    qreal speed = std::sqrt(pow(m_speed.x(), 2) + pow(m_speed.y(), 2));
    qreal thinning = m_thinning * (speed + 1) / 10.0; // can be negative

    if (thinning > 1)
        thinning = 1;

    if (!m_usePressure)
        pressure = 1.0;

    qreal strokeWidth = m_strokeWidth * pressure * (1 - thinning);

    const qreal MINIMUM_STROKE_WIDTH = 1.0;
    if (strokeWidth < MINIMUM_STROKE_WIDTH)
        strokeWidth = MINIMUM_STROKE_WIDTH;

    return strokeWidth;
}

qreal KarbonCalligraphyTool::calculateAngle(const QPointF &oldSpeed, const QPointF &newSpeed)
{
    // calculate the average of the speed (sum of the normalized values)
    qreal oldLength = QLineF(QPointF(0, 0), oldSpeed).length();
    qreal newLength = QLineF(QPointF(0, 0), newSpeed).length();
    QPointF oldSpeedNorm = !qFuzzyCompare(oldLength + 1, 1) ? oldSpeed / oldLength : QPointF(0, 0);
    QPointF newSpeedNorm = !qFuzzyCompare(newLength + 1, 1) ? newSpeed / newLength : QPointF(0, 0);
    QPointF speed = oldSpeedNorm + newSpeedNorm;

    // angle solely based on the speed
    qreal speedAngle = 0;
    if (speed.x() != 0) { // avoid division by zero
        speedAngle = std::atan(speed.y() / speed.x());
    } else if (speed.y() > 0) {
        // x == 0 && y != 0
        speedAngle = M_PI / 2;
    } else if (speed.y() < 0) {
        // x == 0 && y != 0
        speedAngle = -M_PI / 2;
    }
    if (speed.x() < 0)
        speedAngle += M_PI;

    // move 90 degrees
    speedAngle += M_PI / 2;

    qreal fixedAngle = m_angle;
    // check if the fixed angle needs to be flipped
    qreal diff = fixedAngle - speedAngle;
    while (diff >= M_PI) // normalize diff between -180 and 180
        diff -= 2 * M_PI;
    while (diff < -M_PI)
        diff += 2 * M_PI;

    if (std::abs(diff) > M_PI / 2) // if absolute value < 90
        fixedAngle += M_PI; // += 180

    qreal dAngle = speedAngle - fixedAngle;

    // normalize dAngle between -90 and +90
    while (dAngle >= M_PI / 2)
        dAngle -= M_PI;
    while (dAngle < -M_PI / 2)
        dAngle += M_PI;

    qreal angle = fixedAngle + dAngle * (1.0 - m_fixation);

    return angle;
}

void KarbonCalligraphyTool::activate(ToolActivation, const QSet<KoShape *> &)
{
    useCursor(Qt::CrossCursor);
    m_lastShape = nullptr;
}

void KarbonCalligraphyTool::deactivate()
{
    if (m_lastShape && canvas()->shapeManager()->shapes().contains(m_lastShape)) {
        KoSelection *selection = canvas()->shapeManager()->selection();
        selection->deselectAll();
        selection->select(m_lastShape);
    }
}

QList<QPointer<QWidget>> KarbonCalligraphyTool::createOptionWidgets()
{
    // if the widget don't exists yet create it
    QList<QPointer<QWidget>> widgets;

    KoFillConfigWidget *fillWidget = new KoFillConfigWidget(nullptr);
    fillWidget->setWindowTitle(i18n("Fill"));
    fillWidget->setCanvas(canvas());
    widgets.append(fillWidget);

    KarbonCalligraphyOptionWidget *widget = new KarbonCalligraphyOptionWidget;
    connect(widget, &KarbonCalligraphyOptionWidget::usePathChanged, this, &KarbonCalligraphyTool::setUsePath);

    connect(widget, &KarbonCalligraphyOptionWidget::usePressureChanged, this, &KarbonCalligraphyTool::setUsePressure);

    connect(widget, &KarbonCalligraphyOptionWidget::useAngleChanged, this, &KarbonCalligraphyTool::setUseAngle);

    connect(widget, &KarbonCalligraphyOptionWidget::widthChanged, this, &KarbonCalligraphyTool::setStrokeWidth);

    connect(widget, &KarbonCalligraphyOptionWidget::thinningChanged, this, &KarbonCalligraphyTool::setThinning);

    connect(widget, &KarbonCalligraphyOptionWidget::angleChanged, this, QOverload<int>::of(&KarbonCalligraphyTool::setAngle));

    connect(widget, &KarbonCalligraphyOptionWidget::fixationChanged, this, &KarbonCalligraphyTool::setFixation);

    connect(widget, &KarbonCalligraphyOptionWidget::capsChanged, this, &KarbonCalligraphyTool::setCaps);

    connect(widget, &KarbonCalligraphyOptionWidget::massChanged, this, &KarbonCalligraphyTool::setMass);

    connect(widget, &KarbonCalligraphyOptionWidget::dragChanged, this, &KarbonCalligraphyTool::setDrag);

    connect(this, &KarbonCalligraphyTool::pathSelectedChanged, widget, &KarbonCalligraphyOptionWidget::setUsePathEnabled);

    // add shortcuts
    QAction *action = new QAction(i18n("Calligraphy: increase width"), this);
    action->setShortcut(Qt::Key_Right);
    connect(action, &QAction::triggered, widget, &KarbonCalligraphyOptionWidget::increaseWidth);
    addAction("calligraphy_increase_width", action);

    action = new QAction(i18n("Calligraphy: decrease width"), this);
    action->setShortcut(Qt::Key_Left);
    connect(action, &QAction::triggered, widget, &KarbonCalligraphyOptionWidget::decreaseWidth);
    addAction("calligraphy_decrease_width", action);

    action = new QAction(i18n("Calligraphy: increase angle"), this);
    action->setShortcut(Qt::Key_Up);
    connect(action, &QAction::triggered, widget, &KarbonCalligraphyOptionWidget::increaseAngle);
    addAction("calligraphy_increase_angle", action);

    action = new QAction(i18n("Calligraphy: decrease angle"), this);
    action->setShortcut(Qt::Key_Down);
    connect(action, &QAction::triggered, widget, &KarbonCalligraphyOptionWidget::decreaseAngle);
    addAction("calligraphy_decrease_angle", action);

    // sync all parameters with the loaded profile
    widget->emitAll();
    widget->setObjectName(i18n("Calligraphy"));
    widget->setWindowTitle(i18n("Calligraphy"));
    widgets.append(widget);

    return widgets;
}

void KarbonCalligraphyTool::setStrokeWidth(double width)
{
    m_strokeWidth = width;
}

void KarbonCalligraphyTool::setThinning(double thinning)
{
    m_thinning = thinning;
}

void KarbonCalligraphyTool::setAngle(int angle)
{
    m_customAngle = angle;
}

void KarbonCalligraphyTool::setFixation(double fixation)
{
    m_fixation = fixation;
}

void KarbonCalligraphyTool::setMass(double mass)
{
    m_mass = mass * mass + 1;
}

void KarbonCalligraphyTool::setDrag(double drag)
{
    m_drag = drag;
}

void KarbonCalligraphyTool::setUsePath(bool usePath)
{
    m_usePath = usePath;
    // if ( m_selectedPath )
    //     canvas()->updateCanvas( m_selectedPath->boundingRect() );
}

void KarbonCalligraphyTool::setUsePressure(bool usePressure)
{
    m_usePressure = usePressure;
}

void KarbonCalligraphyTool::setUseAngle(bool useAngle)
{
    m_useAngle = useAngle;
}

void KarbonCalligraphyTool::setCaps(double caps)
{
    m_caps = caps;
}

void KarbonCalligraphyTool::updateSelectedPath()
{
    KoPathShape *oldSelectedPath = m_selectedPath; // save old value

    KoSelection *selection = canvas()->shapeManager()->selection();

    // null pointer if it the selection isn't a KoPathShape
    // or if the selection is empty
    m_selectedPath = dynamic_cast<KoPathShape *>(selection->firstSelectedShape());

    // or if it's a KoPathShape but with no or more than one subpaths
    if (m_selectedPath && m_selectedPath->subpathCount() != 1)
        m_selectedPath = nullptr;

    // or if there ora none or more than 1 shapes selected
    if (selection->count() != 1)
        m_selectedPath = nullptr;

    // emit signal it there wasn't a selected path and now there is
    // or the other way around
    if ((m_selectedPath != nullptr) != (oldSelectedPath != nullptr))
        Q_EMIT pathSelectedChanged(m_selectedPath != nullptr);
}
