/*
 *  SPDX-FileCopyrightText: 2006 Adrian Page <adrian@pagenet.plus.com>
 *  SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KO_INPUT_DEVICE_H_
#define KO_INPUT_DEVICE_H_

#include "flake_export.h"

#include <QDebug>
#include <QHash>
#include <QTabletEvent>

/**
 * This class represents an input device.
 * A user can manipulate flake-shapes using a large variety of input devices. This ranges from
 * a mouse to a paintbrush-like tool connected to a tablet. */
class FLAKE_EXPORT KoInputDevice
{
public:
    /**
     * Copy constructor.
     */
    KoInputDevice(const KoInputDevice &other);

    /**
     * Constructor for a tablet.
     * Create a new input device with one of the many types that the tablet can have.
     * @param device the device as found on a QInputEvent
     * @param pointer the pointer as found on a QTabletEvent
     * @param uniqueTabletId the uniqueId as found on a QTabletEvent
     */
    explicit KoInputDevice(QInputDevice::DeviceType device, QPointingDevice::PointerType pointer, qint64 uniqueTabletId = -1);

    /**
     * Constructor for the mouse as input device.
     */
    KoInputDevice();

    ~KoInputDevice();

    /**
     * Return the tablet device used
     */
    QInputDevice::DeviceType device() const;

    /**
     * Return the pointer used
     */
    QPointingDevice::PointerType pointer() const;

    /**
     * Return the unique tablet id as registered by QTabletEvents.
     */
    qint64 uniqueTabletId() const;

    /**
     * Return if this is a mouse device.
     */
    bool isMouse() const;

    /// equal
    bool operator==(const KoInputDevice &) const;
    /// not equal
    bool operator!=(const KoInputDevice &) const;
    /// assignment
    KoInputDevice &operator=(const KoInputDevice &);

    static KoInputDevice invalid(); ///< invalid input device
    static KoInputDevice mouse(); ///< Standard mouse
    static KoInputDevice stylus(); ///< Wacom style/pen
    static KoInputDevice eraser(); ///< Wacom eraser

private:
    class Private;
    Private *const d;
};

Q_DECLARE_METATYPE(KoInputDevice)

FLAKE_EXPORT QDebug operator<<(QDebug debug, const KoInputDevice &device);

inline size_t qHash(const KoInputDevice &key, size_t seed)
{
    return qHash(QString(":%1:%2:%3:%4")
                     .arg(QString::number((int)key.device()),
                          QString::number((int)key.pointer()),
                          QString::number(key.uniqueTabletId()),
                          QString::number(key.isMouse())),
                 seed);
}

#endif
