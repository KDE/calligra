/*
 *  SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoInputDevice.h"

class Q_DECL_HIDDEN KoInputDevice::Private
{
public:
    Private(QPointingDevice::PointerType p, qint64 id, bool m)
            : pointer(p),
            uniqueTabletId(id),
            mouse(m) {
    }
    QPointingDevice::PointerType pointer;
    qint64 uniqueTabletId;
    bool mouse;
};

KoInputDevice::KoInputDevice(QPointingDevice::PointerType pointer, qint64 uniqueTabletId)
        : d(new Private(pointer, uniqueTabletId, false))
{
}

KoInputDevice::KoInputDevice()
        : d(new Private(QPointingDevice::PointerType::Unknown, -1, true))
{
}

KoInputDevice::KoInputDevice(const KoInputDevice &other)
        : d(new Private(other.d->pointer, other.d->uniqueTabletId, other.d->mouse))
{
}


KoInputDevice::~KoInputDevice()
{
    delete d;
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
    return d->mouse || d->pointer == QPointingDevice::PointerType::Generic;
}


bool KoInputDevice::operator==(const KoInputDevice &other) const
{
    return d->pointer == other.d->pointer &&
           d->uniqueTabletId == other.d->uniqueTabletId && d->mouse == other.d->mouse;
}

bool KoInputDevice::operator!=(const KoInputDevice &other) const
{
    return !(operator==(other));
}

KoInputDevice & KoInputDevice::operator=(const KoInputDevice & other)
{
    d->pointer = other.d->pointer;
    d->uniqueTabletId = other.d->uniqueTabletId;
    d->mouse = other.d->mouse;
    return *this;
}

// static
KoInputDevice KoInputDevice::invalid()
{
    KoInputDevice id(QPointingDevice::PointerType::Unknown);
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
    KoInputDevice id(QPointingDevice::PointerType::Pen);
    return id;
}

// static
KoInputDevice KoInputDevice::eraser()
{
    KoInputDevice id(QPointingDevice::PointerType::Eraser);
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
        case QPointingDevice::PointerType::Generic:
            dbg.nospace() << "generic";
            break;
        case QPointingDevice::PointerType::Finger:
            dbg.nospace() << "finger";
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
        case QPointingDevice::PointerType::AllPointerTypes:
            dbg.nospace() << "allPointerType";
            break;
        }
        dbg.space() << "(id: " << device.uniqueTabletId() << ")";
    }
#else
    Q_UNUSED(device);
#endif
    return dbg.space();
}
