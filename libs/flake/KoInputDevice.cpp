/*
 *  SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoInputDevice.h"

class Q_DECL_HIDDEN KoInputDevice::Private
{
public:
    Private(QInputDevice::DeviceType d, QPointingDevice::PointerType p, qint64 id, bool m)
        : device(d)
        , pointer(p)
        , uniqueTabletId(id)
        , mouse(m)
    {
    }
    QInputDevice::DeviceType device;
    QPointingDevice::PointerType pointer;
    qint64 uniqueTabletId;
    bool mouse;
};

KoInputDevice::KoInputDevice(QInputDevice::DeviceType device, QPointingDevice::PointerType pointer, qint64 uniqueTabletId)
    : d(new Private(device, pointer, uniqueTabletId, false))
{
}

KoInputDevice::KoInputDevice()
    : d(new Private(QInputDevice::DeviceType::Unknown, QPointingDevice::PointerType::Unknown, -1, true))
{
}

KoInputDevice::KoInputDevice(const KoInputDevice &other)
    : d(new Private(other.d->device, other.d->pointer, other.d->uniqueTabletId, other.d->mouse))
{
}

KoInputDevice::~KoInputDevice()
{
    delete d;
}

QInputDevice::DeviceType KoInputDevice::device() const
{
    return d->device;
}

QPointingDevice::PointerType KoInputDevice::pointer() const
{
    return d->pointer;
}

qint64 KoInputDevice::uniqueTabletId() const
{
    return d->uniqueTabletId;
}

bool KoInputDevice::isMouse() const
{
    // sometimes, the system gives us tablet events with NoDevice or UnknownPointer. This is
    // likely an XInput2 bug. However, assuming that if cannot identify the tablet device we've
    // actually got a mouse is reasonable. See https://bugs.kde.org/show_bug.cgi?id=283130.
    return d->mouse || d->device == QInputDevice::DeviceType::Unknown || d->pointer == QPointingDevice::PointerType::Unknown;
}

bool KoInputDevice::operator==(const KoInputDevice &other) const
{
    return d->device == other.d->device && d->pointer == other.d->pointer && d->uniqueTabletId == other.d->uniqueTabletId && d->mouse == other.d->mouse;
}

bool KoInputDevice::operator!=(const KoInputDevice &other) const
{
    return !(operator==(other));
}

KoInputDevice &KoInputDevice::operator=(const KoInputDevice &other)
{
    d->device = other.d->device;
    d->pointer = other.d->pointer;
    d->uniqueTabletId = other.d->uniqueTabletId;
    d->mouse = other.d->mouse;
    return *this;
}

// static
KoInputDevice KoInputDevice::invalid()
{
    KoInputDevice id(QInputDevice::DeviceType::Unknown, QPointingDevice::PointerType::Unknown);
    return id;
}

KoInputDevice KoInputDevice::mouse()
{
    KoInputDevice id;
    return id;
}

// static
KoInputDevice KoInputDevice::stylus()
{
    KoInputDevice id(QInputDevice::DeviceType::Stylus, QPointingDevice::PointerType::Pen);
    return id;
}

// static
KoInputDevice KoInputDevice::eraser()
{
    KoInputDevice id(QInputDevice::DeviceType::Stylus, QPointingDevice::PointerType::Eraser);
    return id;
}

QDebug operator<<(QDebug dbg, const KoInputDevice &device)
{
#ifndef NDEBUG
    if (device.isMouse())
        dbg.nospace() << "mouse";
    else {
        switch (device.pointer()) {
        case QPointingDevice::PointerType::Unknown:
            dbg.nospace() << "unknown pointer";
            break;
        case QPointingDevice::PointerType::Pen:
            dbg.nospace() << "pen";
            break;
        case QPointingDevice::PointerType::Cursor:
            dbg.nospace() << "cursor";
            break;
        case QPointingDevice::PointerType::Eraser:
            dbg.nospace() << "eraser";
            break;
        case QPointingDevice::PointerType::Finger:
            dbg.nospace() << "finger";
            break;
        case QPointingDevice::PointerType::Generic:
            dbg.nospace() << "generic";
            break;
        case QPointingDevice::PointerType::AllPointerTypes:
            dbg.nospace() << "allPointerTypes";
            break;
        }
        switch (device.device()) {
        case QInputDevice::DeviceType::Unknown:
            dbg.space() << "no device";
            break;
        case QInputDevice::DeviceType::Puck:
            dbg.space() << "puck";
            break;
        case QInputDevice::DeviceType::Stylus:
            dbg.space() << "stylus";
            break;
        case QInputDevice::DeviceType::Airbrush:
            dbg.space() << "airbrush";
            break;
        case QInputDevice::DeviceType::Keyboard:
            dbg.space() << "keyboard";
            break;
        case QInputDevice::DeviceType::Mouse:
            dbg.space() << "mouse";
            break;
        case QInputDevice::DeviceType::TouchPad:
            dbg.space() << "touchpad";
            break;
        case QInputDevice::DeviceType::TouchScreen:
            dbg.space() << "touchscreen";
            break;
        case QInputDevice::DeviceType::AllDevices:
            dbg.space() << "all";
            break;
        }
        dbg.space() << "(id: " << device.uniqueTabletId() << ")";
    }
#else
    Q_UNUSED(device);
#endif
    return dbg.space();
}
