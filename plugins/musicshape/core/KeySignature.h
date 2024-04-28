/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef MUSIC_CORE_KEYSIGNATURE_H
#define MUSIC_CORE_KEYSIGNATURE_H

#include "StaffElement.h"

namespace MusicCore
{

class Staff;

/**
 * This class represents a key signature.
 */
class KeySignature : public StaffElement
{
    Q_OBJECT
public:
    /**
     * Create a new key signature instance that should be placed on a specific staff.
     *
     * @param staff the staff on which the clef should be shown
     * @param startTime the start time
     * @param accidentals the number of accidentals in this key signature, positive values for sharps, negative values
     * for flats.
     * @param cancel the cancel
     */
    KeySignature(Staff *staff, int startTime, int accidentals, int cancel = 0);

    /**
     * Destructor.
     */
    ~KeySignature() override;

    /**
     * Returns the number of accidentals in this key signature. Returns a positive value for sharps or a negative value
     * for flats.
     */
    int accidentals() const;

    /**
     * Returns the accidentals for a note with the given pitch.
     *
     * @param pitch the pitch of the note for which to return the accidentals
     */
    int accidentals(int pitch) const;

    /**
     * Returns the priority of this staff element with regard to order in which it should be sorted.
     */
    int priority() const override;

    int cancel() const;
    int cancel(int pitch) const;
public Q_SLOTS:
    /**
     * Sets the number of accidentals in this key signature. Use positive values for sharps and negative values for
     * flats.
     *
     * @param accidentals the new accidentals for this key signature
     */
    void setAccidentals(int accidentals);

    void setCancel(int cancel);
Q_SIGNALS:
    /**
     * This signal is emitted when the number of accidentals change.
     */
    void accidentalsChanged(int accidentals);

private:
    class Private;
    Private *const d;
};

} // namespace MusicCore

#endif // MUSIC_CORE_KEYSIGNATURE_H
