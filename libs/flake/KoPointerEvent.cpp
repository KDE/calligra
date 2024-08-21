/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2006 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2006 C. Boemann Rasmussen <cbo@boemann.dk>
   SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoPointerEvent.h"
#include "KoInputDeviceHandlerEvent.h"
#include <QGraphicsSceneMouseEvent>
#include <QMouseEvent>
#include <QTabletEvent>
#include <QTouchEvent>
#include <QWheelEvent>

class Q_DECL_HIDDEN KoPointerEvent::Private
{
public:
    Private()
        : tabletEvent(nullptr)
        , mouseEvent(nullptr)
        , wheelEvent(nullptr)
        , touchEvent(nullptr)
        , gsMouseEvent(nullptr)
        , gsWheelEvent(nullptr)
        , deviceEvent(nullptr)
        , tabletButton(Qt::NoButton)
        , globalPos(0, 0)
        , pos(0, 0)
        , posZ(0)
        , rotationX(0)
        , rotationY(0)
        , rotationZ(0)
    {
    }
    QTabletEvent *tabletEvent;
    QMouseEvent *mouseEvent;
    QWheelEvent *wheelEvent;
    QTouchEvent *touchEvent;
    QGraphicsSceneMouseEvent *gsMouseEvent;
    QGraphicsSceneWheelEvent *gsWheelEvent;
    KoInputDeviceHandlerEvent *deviceEvent;
    Qt::MouseButton tabletButton;
    QPoint globalPos, pos;
    int posZ;
    int rotationX, rotationY, rotationZ;
};

KoPointerEvent::KoPointerEvent(QMouseEvent *ev, const QPointF &pnt)
    : point(pnt)
    , m_event(ev)
    , d(new Private())
{
    Q_ASSERT(m_event);
    d->mouseEvent = ev;
}

KoPointerEvent::KoPointerEvent(QGraphicsSceneMouseEvent *ev, const QPointF &pnt)
    : point(pnt)
    , m_event(ev)
    , d(new Private())
{
    Q_ASSERT(m_event);
    d->gsMouseEvent = ev;
}

KoPointerEvent::KoPointerEvent(QGraphicsSceneWheelEvent *ev, const QPointF &pnt)
    : point(pnt)
    , m_event(ev)
    , d(new Private())
{
    Q_ASSERT(m_event);
    d->gsWheelEvent = ev;
}

KoPointerEvent::KoPointerEvent(QTabletEvent *ev, const QPointF &pnt)
    : point(pnt)
    , m_event(ev)
    , d(new Private())
{
    Q_ASSERT(m_event);
    d->tabletEvent = ev;
}

KoPointerEvent::KoPointerEvent(QTouchEvent *ev, const QPointF &pnt, const QVector<KoTouchPoint> &_touchPoints)
    : point(pnt)
    , touchPoints(_touchPoints)
    , m_event(ev)
    , d(new Private())
{
    Q_ASSERT(m_event);
    d->touchEvent = ev;
}

KoPointerEvent::KoPointerEvent(QWheelEvent *ev, const QPointF &pnt)
    : point(pnt)
    , m_event(ev)
    , d(new Private())
{
    Q_ASSERT(m_event);
    d->wheelEvent = ev;
}

KoPointerEvent::KoPointerEvent(KoInputDeviceHandlerEvent *ev, int x, int y, int z, int rx, int ry, int rz)
    : m_event(ev)
    , d(new Private())
{
    Q_ASSERT(m_event);
    d->deviceEvent = ev;
    d->pos = QPoint(x, y);
    d->posZ = z;
    d->rotationX = rx;
    d->rotationY = ry;
    d->rotationZ = rz;
}

KoPointerEvent::KoPointerEvent(KoPointerEvent *event, const QPointF &point)
    : point(point)
    , touchPoints(event->touchPoints)
    , m_event(event->m_event)
    , d(new Private(*(event->d)))
{
    Q_ASSERT(m_event);
}

KoPointerEvent::KoPointerEvent(const KoPointerEvent &rhs)
    : point(rhs.point)
    , touchPoints(rhs.touchPoints)
    , m_event(rhs.m_event)
    , d(new Private(*rhs.d))
{
}

KoPointerEvent::~KoPointerEvent()
{
    delete d;
}

Qt::MouseButton KoPointerEvent::button() const
{
    if (d->mouseEvent)
        return d->mouseEvent->button();
    else if (d->tabletEvent || d->touchEvent)
        return d->tabletButton;
    else if (d->deviceEvent)
        return d->deviceEvent->button();
    else if (d->gsMouseEvent)
        return d->gsMouseEvent->button();
    else
        return Qt::NoButton;
}

Qt::MouseButtons KoPointerEvent::buttons() const
{
    if (d->mouseEvent)
        return d->mouseEvent->buttons();
    else if (d->wheelEvent)
        return d->wheelEvent->buttons();
    else if (d->tabletEvent || d->touchEvent)
        return d->tabletButton;
    else if (d->deviceEvent)
        return d->deviceEvent->buttons();
    else if (d->gsMouseEvent)
        return d->gsMouseEvent->buttons();
    else if (d->gsWheelEvent)
        return d->gsWheelEvent->buttons();
    return Qt::NoButton;
}

QPointF KoPointerEvent::globalPosition() const
{
    if (d->mouseEvent)
        return d->mouseEvent->globalPosition();
    else if (d->wheelEvent)
        return d->wheelEvent->globalPosition();
    else if (d->tabletEvent)
        return d->tabletEvent->globalPosition();
    else if (d->gsMouseEvent)
        return d->gsMouseEvent->screenPos();
    else if (d->gsWheelEvent)
        return d->gsWheelEvent->screenPos();
    else
        return d->globalPos;
}

QPointF KoPointerEvent::position() const
{
    if (d->mouseEvent)
        return d->mouseEvent->position();
    else if (d->wheelEvent)
        return d->wheelEvent->position();
    else if (d->tabletEvent)
        return d->tabletEvent->position();
    else if (d->gsMouseEvent)
        return d->gsMouseEvent->pos().toPoint();
    else if (d->gsWheelEvent)
        return d->gsWheelEvent->pos().toPoint();
    else
        return d->pos;
}

qreal KoPointerEvent::pressure() const
{
    if (d->tabletEvent)
        return d->tabletEvent->pressure();
    else
        return 1.0;
}

qreal KoPointerEvent::rotation() const
{
    if (d->tabletEvent)
        return d->tabletEvent->rotation();
    else
        return 0.0;
}

qreal KoPointerEvent::tangentialPressure() const
{
    if (d->tabletEvent)
        return d->tabletEvent->tangentialPressure();
    else
        return 0.0;
}

int KoPointerEvent::x() const
{
    if (d->tabletEvent)
        return d->tabletEvent->x();
    if (d->wheelEvent)
        return d->wheelEvent->position().x();
    else if (d->mouseEvent)
        return d->mouseEvent->x();
    else
        return position().x();
}

int KoPointerEvent::xTilt() const
{
    if (d->tabletEvent)
        return d->tabletEvent->xTilt();
    else
        return 0;
}

int KoPointerEvent::y() const
{
    if (d->tabletEvent)
        return d->tabletEvent->y();
    if (d->wheelEvent)
        return d->wheelEvent->position().y();
    else if (d->mouseEvent)
        return d->mouseEvent->y();
    else
        return position().y();
}

int KoPointerEvent::yTilt() const
{
    if (d->tabletEvent)
        return d->tabletEvent->yTilt();
    else
        return 0;
}

int KoPointerEvent::z() const
{
    if (d->tabletEvent)
        return d->tabletEvent->z();
    else if (d->deviceEvent)
        return d->posZ;
    else
        return 0;
}

int KoPointerEvent::delta() const
{
    if (d->wheelEvent)
        return d->wheelEvent->angleDelta().y();
    else if (d->gsWheelEvent)
        return d->gsWheelEvent->delta();
    else
        return 0;
}

int KoPointerEvent::rotationX() const
{
    return d->rotationX;
}

int KoPointerEvent::rotationY() const
{
    return d->rotationY;
}

int KoPointerEvent::rotationZ() const
{
    return d->rotationZ;
}

Qt::Orientation KoPointerEvent::orientation() const
{
    if (d->wheelEvent)
        return qAbs(d->wheelEvent->angleDelta().y()) > qAbs(d->wheelEvent->angleDelta().x()) ? Qt::Vertical : Qt::Horizontal;
    else if (d->gsWheelEvent)
        return d->gsWheelEvent->orientation();
    else
        return Qt::Horizontal;
}

bool KoPointerEvent::isTabletEvent()
{
    return dynamic_cast<QTabletEvent *>(m_event) != nullptr;
}

void KoPointerEvent::setTabletButton(Qt::MouseButton button)
{
    d->tabletButton = button;
}

Qt::KeyboardModifiers KoPointerEvent::modifiers() const
{
    if (d->tabletEvent)
        return d->tabletEvent->modifiers();
    else if (d->mouseEvent)
        return d->mouseEvent->modifiers();
    else if (d->wheelEvent)
        return d->wheelEvent->modifiers();
    else if (d->deviceEvent)
        return d->deviceEvent->modifiers();
    else if (d->gsMouseEvent)
        return d->gsMouseEvent->modifiers();
    else if (d->gsWheelEvent)
        return d->gsWheelEvent->modifiers();
    else
        return Qt::NoModifier;
}
