/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef MUSIC_CORE_TIMESIGNATURE_H
#define MUSIC_CORE_TIMESIGNATURE_H

#include "StaffElement.h"

namespace MusicCore {

class Staff;

/**
 * This class represents a time signature.
 */
class TimeSignature : public StaffElement
{
    Q_OBJECT
public:
    enum TimeSignatureType {
        Classical,
        Number
    };

    /**
     * Create a new time signature instance and place it on a specified staff.
     */
    TimeSignature(Staff* staff, int startTime, int beats, int beat, TimeSignatureType type = Classical);

    /**
     * Destructor.
     */
    ~TimeSignature() override;

    /**
     * Returns the number of beats in this time signature.
     */
    int beats() const;

    /**
     * Returns the beat type of the time signature.
     */
    int beat() const;

    /**
     * Returns the type of the time signature.
     */
    TimeSignatureType type() const;
    int priority() const override;
    
    QList<int> beatLengths() const;
public Q_SLOTS:
    /**
     * Changes the number of beats in this time signature.
     *
     * @param beats the new number of beats in the time signature
     */
    void setBeats(int beats);

    /**
     * Changes the beat type of the time signature.
     *
     * @param beat the new beat type
     */
    void setBeat(int beat);

    /**
     * Changes the type of the time signature.
     */
    void setType(MusicCore::TimeSignature::TimeSignatureType type);
Q_SIGNALS:
    void beatsChanged(int beats);
    void beatChanged(int beat);
    void typeChanged(MusicCore::TimeSignature::TimeSignatureType type);
private:
    class Private;
    Private * const d;
};

} // namespace MusicCore

#endif // MUSIC_CORE_TIMESIGNATURE_H
